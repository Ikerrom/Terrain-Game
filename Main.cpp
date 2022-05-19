#include "header.h"
#include "ConsoleEngine.cpp"
#include "Tile.cpp"
#include "Map.cpp"
#include <unistd.h>
using namespace std;

int screensize = width * fontsize;
float screenratio = height / width;
int zoomtilenumber = 3, seed;
float toaddheight, toaddwidth;
char worldname[101];
bool running = false;
int selectedelement = 1;
string sfps;

void SandPhysics()
{
	for (int i = 0; i < seentiles.size(); i++)
	{
		if (seentiles.at(i).getMaterial() == SAND && seentiles.at(i).getTileY() >= 0 && seentiles.at(i).getTileY() < (int)height && seentiles.at(i).getTileX() >= 0 && seentiles.at(i).getTileX() < (int)width)
		{
			int filled = 0;
			int lbfilled = 0;
			int rbfilled = 0;
			for (int k = 0; k < seentiles.size(); k++)
			{
				if (seentiles.at(i).getTileY() + 1 == seentiles.at(k).getTileY() && seentiles.at(i).getTileX() == seentiles.at(k).getTileX() && seentiles.at(k).getMaterial() != WATER)
				{
					filled = 1;
				}
				else if (seentiles.at(i).getTileY() + 1 == seentiles.at(k).getTileY() && seentiles.at(i).getTileX() + 1 == seentiles.at(k).getTileX() && seentiles.at(k).getMaterial() != WATER)
				{
					rbfilled = 1;
				}
				else if (seentiles.at(i).getTileY() + 1 == seentiles.at(k).getTileY() && seentiles.at(i).getTileX() - 1 == seentiles.at(k).getTileX() && seentiles.at(k).getMaterial() != WATER)
				{
					lbfilled = 1;
				}
			}

			if (filled == 0)
			{
				seentiles.at(i).setTileY((seentiles.at(i).getTileY() + 1));
			}
			else if (lbfilled == 0)
			{
				seentiles.at(i).setTileY((seentiles.at(i).getTileY() + 1));
				seentiles.at(i).setTileX((seentiles.at(i).getTileX() - 1));
			}
			else if (rbfilled == 0)
			{
				seentiles.at(i).setTileY((seentiles.at(i).getTileY() + 1));
				seentiles.at(i).setTileX((seentiles.at(i).getTileX() + 1));
			}
		}
	}
}

void WaterPhysics()
{
	for (int i = 0; i < seentiles.size(); i++)
	{
		if (seentiles.at(i).getMaterial() == WATER && seentiles.at(i).getTileY() >= 0 && seentiles.at(i).getTileY() < (int)height && seentiles.at(i).getTileX() >= 0 && seentiles.at(i).getTileX() < (int)width)
		{
			int filled = 0;
			int lfilled = 0;
			int rfilled = 0;
			int rbfilled = 0;
			int lbfilled = 0;
			int infilled = 0;
			for (int k = 0; k < seentiles.size(); k++)
			{
				if (seentiles.at(i).getTileY() == seentiles.at(k).getTileY() && seentiles.at(i).getTileX() == seentiles.at(k).getTileX())
				{
					if (i != k)
					{
						infilled = 1;
					}
				}
				if (seentiles.at(i).getTileY() + 1 == seentiles.at(k).getTileY() && seentiles.at(i).getTileX() == seentiles.at(k).getTileX())
				{
					filled = 1;
				}
				else if (seentiles.at(i).getTileY() + 1 == seentiles.at(k).getTileY() && seentiles.at(i).getTileX() + 1 == seentiles.at(k).getTileX())
				{
					rbfilled = 1;
				}
				else if (seentiles.at(i).getTileY() + 1 == seentiles.at(k).getTileY() && seentiles.at(i).getTileX() - 1 == seentiles.at(k).getTileX())
				{
					lbfilled = 1;
				}
				else if (seentiles.at(i).getTileY() == seentiles.at(k).getTileY() && seentiles.at(i).getTileX() + 1 == seentiles.at(k).getTileX())
				{
					rfilled = 1;
				}
				else if (seentiles.at(i).getTileY() == seentiles.at(k).getTileY() && seentiles.at(i).getTileX() - 1 == seentiles.at(k).getTileX())
				{
					lfilled = 1;
				}
			}

			if (infilled == 1)
			{
				seentiles.at(i).setTileY((seentiles.at(i).getTileY() - 1));
			}
			if (filled == 0)
			{
				seentiles.at(i).setTileY((seentiles.at(i).getTileY() + 1));
			}
			else if (lbfilled == 0)
			{
				seentiles.at(i).setTileY((seentiles.at(i).getTileY() + 1));
				seentiles.at(i).setTileX((seentiles.at(i).getTileX() - 1));
			}
			else if (rbfilled == 0)
			{
				seentiles.at(i).setTileY((seentiles.at(i).getTileY() + 1));
				seentiles.at(i).setTileX((seentiles.at(i).getTileX() + 1));
			}
			else if (lfilled == 0 && rfilled == 0)
			{
				int a = rand() % 2;
				if (a == 0)
				{
					seentiles.at(i).setTileX((seentiles.at(i).getTileX() - 1));
				}
				if (a == 1)
				{
					seentiles.at(i).setTileX((seentiles.at(i).getTileX() + 1));
				}
			}
			else if (lfilled == 0)
			{
				seentiles.at(i).setTileX((seentiles.at(i).getTileX() - 1));
			}
			else if (rfilled == 0)
			{
				seentiles.at(i).setTileX((seentiles.at(i).getTileX() + 1));
			}
		}
	}
}

void ChunkGen()
{
	if (playerx >= (int)(pmapwidth - (int)chunkwidth / 2) - width)
	{
		GenerateInfinite();
		AddTilesToSeen(width, height);
	}
	if (playerx <= (int)(nmapwidth + (int)chunkwidth / 2) + width)
	{
		GenerateInfinite();
		AddTilesToSeen(width, height);
	}
}

void StartGame(string filename, int seed)
{
	Generate(filename, seed);
	AddTilesToSeen(width, height);
	clock_t current_ticks, delta_ticks;
	clock_t fps = 0;
	while (running)
	{
		current_ticks = clock();
		usleep(100000 / 75);
		CheckConsoleInput();

		if (leftpressed)
		{
			int filled = 0;
			for (int i = 0; i < seentiles.size(); i++)
			{
				if (mouseY == seentiles.at(i).getTileY() && mouseX == seentiles.at(i).getTileX())
				{
					filled = 1;
				}
			}
			if (filled == 0)
			{
				if (selectedelement == 1)
				{
					Tile t(mouseX, mouseY, GRASS);
					seentiles.push_back(t);
				}
				else if (selectedelement == 2)
				{
					Tile t(mouseX, mouseY, STONE);
					seentiles.push_back(t);
				}
				else if (selectedelement == 3)
				{
					Tile t(mouseX, mouseY, WATER);
					seentiles.push_back(t);
				}
				else if (selectedelement == 4)
				{
					Tile t(mouseX, mouseY, SAND);
					seentiles.push_back(t);
				}
			}
		}
		if (rigthpressed)
		{
			for (int i = 0; i < seentiles.size(); i++)
			{
				if (mouseY == seentiles.at(i).getTileY() && mouseX == seentiles.at(i).getTileX())
				{
					seentiles.erase(seentiles.begin() + i);
				}
			}
		}
		switch (keynum)
		{
		case 'd':
			AddChangesToTiles(width, height);
			for (int i = 0; i < tiles.size(); i++)
			{
				tiles.at(i).setTileX(tiles.at(i).getTileX() - 1);
			}
			AddTilesToSeen(width, height);
			playerx++;
			keynum = 0;
			break;
		case 'a':
			AddChangesToTiles(width, height);
			for (int i = 0; i < tiles.size(); i++)
			{
				tiles.at(i).setTileX(tiles.at(i).getTileX() + 1);
			}
			AddTilesToSeen(width, height);
			playerx--;
			keynum = 0;
			break;
		case 'w':
			AddChangesToTiles(width, height);
			for (int i = 0; i < tiles.size(); i++)
			{
				tiles.at(i).setTileY(tiles.at(i).getTileY() + 1);
			}
			AddTilesToSeen(width, height);
			playery++;
			keynum = 0;
			break;
		case 's':
			AddChangesToTiles(width, height);
			for (int i = 0; i < tiles.size(); i++)
			{
				tiles.at(i).setTileY(tiles.at(i).getTileY() - 1);
			}
			AddTilesToSeen(width, height);
			playery--;
			keynum = 0;
			break;
		case '-':
			if (width < 70)
			{
				AddChangesToTiles(width, height);
				toaddwidth += zoomtilenumber;
				if ((int)toaddwidth % 2 == 0 && toaddwidth != 0)
				{
					width += (int)toaddwidth;
				}

				toaddheight += zoomtilenumber * screenratio;
				if ((int)toaddheight % 2 == 0 && toaddheight != 0)
				{
					height += (int)toaddheight;
					addtogenheigth += (int)(toaddheight / 2);
				}

				fontsize = screensize / width;
				SetWindow((int)width, (int)height, (int)fontsize, (int)fontsize);

				if ((int)toaddwidth % 2 == 0 && toaddwidth != 0)
				{
					for (int i = 0; i < tiles.size(); i++)
					{
						tiles.at(i).setTileX(tiles.at(i).getTileX() + (int)(abs(toaddwidth / 2)));
					}
					toaddwidth = 0;
				}

				if ((int)toaddheight % 2 == 0 && toaddheight != 0)
				{
					for (int i = 0; i < tiles.size(); i++)
					{
						tiles.at(i).setTileY(tiles.at(i).getTileY() + (int)(toaddheight / 2));
					}
					toaddheight = 0;
				}

				AddTilesToSeen(width, height);
			}
			keynum = 0;
			break;
		case '+':
			if (width > 30)
			{
				AddChangesToTiles(width, height);
				toaddwidth -= zoomtilenumber;
				if (abs((int)toaddwidth) % 2 == 0 && toaddwidth != 0)
				{
					width -= abs((int)toaddwidth);
				}

				toaddheight -= zoomtilenumber * screenratio;
				if (abs((int)toaddheight) % 2 == 0 && toaddheight != 0)
				{
					height -= abs((int)toaddheight);
					addtogenheigth -= (int)abs(toaddheight / 2);
				}

				fontsize = screensize / width;
				SetWindow((int)width, (int)height, (int)fontsize, (int)fontsize);

				if (abs((int)toaddwidth) % 2 == 0 && toaddwidth != 0)
				{
					for (int i = 0; i < tiles.size(); i++)
					{
						tiles.at(i).setTileX(tiles.at(i).getTileX() - (int)(abs(toaddwidth / 2)));
					}
					toaddwidth = 0;
				}

				if (abs((int)toaddheight) % 2 == 0 && toaddheight != 0)
				{
					for (int i = 0; i < tiles.size(); i++)
					{
						tiles.at(i).setTileY(tiles.at(i).getTileY() - (int)(abs(toaddheight / 2)));
					}
					toaddheight = 0;
				}
				AddTilesToSeen(width, height);
			}
			keynum = 0;
			break;
		case 'z':
			Save();
			keynum = 0;
			break;
		case 'e':

			keynum = 0;
			break;
		case 'p':
			running = false;
			keynum = 0;
			break;
		case '1':
			selectedelement = 1;
			keynum = 0;
			break;
		case '2':
			selectedelement = 2;
			keynum = 0;
			break;
		case '3':
			selectedelement = 3;
			keynum = 0;
			break;
		case '4':
			selectedelement = 4;
			keynum = 0;
			break;

		case 0:
			break;
		}

		SandPhysics();
		WaterPhysics();
		ChunkGen();

		Fill(0, 0, width, height, SKY);
		DrawFilledCircle(width - 4, 3, 2, SAND);

		delta_ticks = clock() - current_ticks;

		if (delta_ticks > 0)
		{
			fps = CLOCKS_PER_SEC / delta_ticks;
		}

		sfps = " |ConsoleGameEngine| ";
		sfps += " FPS: ";
		sfps += to_string(fps);
		sfps += " X: ";
		sfps += to_string(playerx);
		sfps += " Y: ";
		sfps += to_string(playery);
		char tab2[256];
		strcpy(tab2, sfps.c_str());
		SetConsoleName(tab2);

		for (int i = 0; i < seentiles.size(); i++)
		{
			Draw(seentiles.at(i).getTileX(), seentiles.at(i).getTileY(), seentiles.at(i).getMaterial());
		}

		DrawToScreen();
	}
}

int main()
{
	printf("World\n");
	scanf("%100s %d", worldname, &seed);

	SetWindow(width, height, fontsize, fontsize);
	while (1)
	{

		string filename = worldname;
		filename += ".txt";
		ShowConsoleCursor(false);
		// STONE
		SetConsoleColor(8, 143, 143, 143);
		// GRASS
		SetConsoleColor(2, 60, 201, 60);
		// WATER
		SetConsoleColor(9, 59, 156, 255);
		// SKY
		SetConsoleColor(3, 50, 182, 227);
		// SAND
		SetConsoleColor(6, 232, 210, 130);
		// WOOD
		SetConsoleColor(4, 162, 109, 63);
		// BRIGHT LEAVES
		SetConsoleColor(10, 90, 207, 58);
		// DARK LEAVES
		SetConsoleColor(11, 12, 159, 65);
		// PLAYER
		SetConsoleColor(13, 235, 77, 210);
		SetWindow(width, height, fontsize, fontsize);
		running = true;
		StartGame(filename, seed);
	}

	return 0;
}
