#include <fstream>
#include <vector>
#include <time.h>
#include <direct.h>
using namespace std;

vector<Tile> tiles;
vector<Tile> seentiles;

vector<float> perlinNoise;
vector<float> noiseSeed;

ofstream outfile;
ifstream iffile;

string mapfilepath;
string seedfilepath;

string pmapwidthpath;
string nmapwidthpath;

string mapheightpath;

string plastgenpath;
string nlastgenpath;

string playerpospath;
string playerzoompath;

int mapseed;
int pmapwidth;
int nmapwidth = -chunkwidth;
int mapheight;

void GeneratePerlinNoise(int count, int nOctaves)
{
    for (int x = 0; x < count; x++)
    {
        float noise = 0.0f;
        float scale = 1.0f;
        float scaleacc = 0.0f;
        for (int o = 0; o < nOctaves; o++)
        {
            int pitch = count >> o;
            int sample1 = (x / pitch) * pitch;
            int sample2 = (sample1 + pitch) % count;
            float blend = (float)(x - sample1) / (float)pitch;
            float fsample = (1.0f - blend) * noiseSeed.at(sample1) + blend * noiseSeed.at(sample2);
            noise += fsample * scale;
            scaleacc += scale;
            scale = scale / 1.0f;
        }
        perlinNoise.push_back(noise / scaleacc);
    }
}

void ReWriteFile()
{
    outfile.open(mapfilepath);
    for (int i = 0; i < tiles.size(); i++)
    {
        outfile << tiles.at(i).getTileX();
        outfile << "\n";
        outfile << tiles.at(i).getTileY();
        outfile << "\n";
        outfile << tiles.at(i).getMaterial();
        outfile << "\n";
    }
    outfile.close();
}

void Save()
{
    ReWriteFile();
    outfile.open(playerpospath);
    outfile << playerx;
    outfile << "\n";
    outfile << playery;
    outfile << "\n";
    outfile.close();

    outfile.open(playerzoompath);
    outfile << width;
    outfile << "\n";
    outfile << height;
    outfile << "\n";
    outfile << fontsize;
    outfile << "\n";
    outfile << addtogenheigth;
    outfile.close();
}

void GenerateTrees()
{
    vector<int> alreadytree;
    
    for (int i = 0; i < rand() % 1000; i++)
    {
        int ymax;
        int x;
        int theresatree = 0;
        srand(rand());
        if (playerx >= 0)
        {
            x = (rand() % chunkwidth) + pmapwidth - playerx - chunkwidth / 2;
            ymax = chunkheight + playery + addtogenheigth;
        }
        else
        {
            x = (rand() % chunkwidth) + nmapwidth - playerx - chunkwidth / 2;
            ymax = chunkheight + playery + addtogenheigth;
        }

        for (int k = 0; k < alreadytree.size(); k++)
        {
            if (x >= alreadytree.at(k) - 5 && x <= alreadytree.at(k) + 5)
            {
                theresatree = 1;
            }
        }

        if (theresatree == 0)
        {
            alreadytree.push_back(x);
            
            for (int k = 0; k < tiles.size(); k++)
            {
                if (tiles.at(k).getTileX() == x && tiles.at(k).getTileY() < ymax)
                {
                    ymax = tiles.at(k).getTileY();
                }
            }

            if (ymax <= chunkheight * 0.2 + playery + addtogenheigth)
            {
                int treeheigth = rand() % 6 + 4;
                int stepheight = rand() % 2 + 1;
                int stepcount = rand() % 4 + 1;
                int leaveswidht = stepcount;
                if (stepcount % 2 == 0)
                {
                    for (int k = 1; k < treeheigth; k++)
                    {
                        Tile t(x, ymax - k, WOOD);
                        tiles.push_back(t);
                    }
                    for (int i = 0; i <= stepcount; i++)
                    {
                        for (int k = (stepheight * i) - stepheight; k <= stepheight * i; k++)
                        {
                            for (int w = -(leaveswidht / 2); w <= (leaveswidht / 2); w++)
                            {
                                Tile t(x + w, (ymax - treeheigth) - k, DLEAVES);
                                tiles.push_back(t);
                            }
                        }
                        leaveswidht--;
                    }
                }
            }
            else if (ymax <= chunkheight * 0.4 + playery + addtogenheigth)
            {
                int treeheigth = rand() % 5 + 3;
                int leaveswidth = rand() % 3 + 1;
                int leavesheigth = rand() % 4 + 2;
                for (int k = 1; k < treeheigth; k++)
                {
                    Tile t(x, ymax - k, WOOD);
                    tiles.push_back(t);
                }

                for (int k = treeheigth; k < treeheigth + leavesheigth; k++)
                {
                    for (int w = -leaveswidth; w <= leaveswidth; w++)
                    {
                        Tile t(x + w, ymax - k, BLEAVES);
                        tiles.push_back(t);
                    }
                }
            }
        }
    }
    ReWriteFile();
}

void FromFileToTiles()
{
    vector<int> poses;
    tiles.clear();
    seentiles.clear();
    int adjustedsize;
    int tmp;

    iffile.open(mapfilepath);
    while (iffile >> tmp)
    {
        poses.push_back(tmp);
    }

    for (int i = 0; i < poses.size(); i += 3)
    {
        Tile t(poses.at(i), poses.at(i + 1), poses.at(i + 2));
        tiles.push_back(t);
    }
    iffile.close();
}

void GenerateInfinite()
{
    float lastf;
    float firstf;
    noiseSeed.clear();
    perlinNoise.clear();
    int iterator = 0;
    Save();

    if (playerx >= 0)
    {
        iffile.open(plastgenpath);
        if (iffile)
        {
            float tmp;
            while (iffile >> tmp)
            {
                lastf = tmp;
            }
            srand(rand());

            noiseSeed.push_back(lastf);
            for (int i = 0; i < chunkwidth - 1; i++)
            {
                noiseSeed.push_back((float)rand() / (float)RAND_MAX);
            }
        }
        else
        {
            srand(mapseed);
            for (int i = 0; i < chunkwidth; i++)
            {
                noiseSeed.push_back((float)rand() / (float)RAND_MAX);
            }
        }
        iffile.close();

        iffile.open(pmapwidthpath);
        if (iffile)
        {
            int tmp;
            while (iffile >> tmp)
            {
                pmapwidth = tmp;
            }
        }
        iffile.close();
    }

    if (playerx < 0)
    {
        iffile.open(nlastgenpath);
        if (iffile)
        {
            float tmp;
            while (iffile >> tmp)
            {
                firstf = tmp;
            }
            srand(rand());

            noiseSeed.push_back(firstf);
            for (int i = 0; i < chunkwidth - 1; i++)
            {
                noiseSeed.push_back((float)rand() / (float)RAND_MAX);
            }
        }
        else
        {
            srand(mapseed);
            for (int i = 0; i < chunkwidth; i++)
            {
                noiseSeed.push_back((float)rand() / (float)RAND_MAX);
            }
        }
        iffile.close();

        iffile.open(nmapwidthpath);
        if (iffile)
        {
            int tmp;
            while (iffile >> tmp)
            {
                nmapwidth = tmp;
            }
        }
        iffile.close();
    }

    outfile.open(playerpospath);
    outfile << playerx;
    outfile << "\n";
    outfile << playery;
    outfile << "\n";
    outfile.close();

    GeneratePerlinNoise(chunkwidth, 6);

    lastf = perlinNoise.at(perlinNoise.size() - 1);
    outfile.open(plastgenpath);
    outfile << lastf;
    outfile.close();

    firstf = perlinNoise.at(0);
    outfile.open(nlastgenpath);
    outfile << firstf;
    outfile.close();

    outfile.open(mapfilepath, ios::out | ios::app);
    if (playerx >= 0)
    {
        for (int x = (int)(width / 2) - (int)(chunkwidth / 2); x < (int)(width / 2) - (int)(chunkwidth / 2) + chunkwidth; x++)
        {

            int y = (perlinNoise.at(iterator) * (float)chunkheight - (float)chunkheight / 6) + playery + addtogenheigth;

            for (int k = y; k < chunkheight; k++)
            {
                if (k < y + 3 && y > chunkheight * 0.4 + playery + addtogenheigth)
                {
                    outfile << pmapwidth + x - playerx;
                    outfile << "\n";
                    outfile << k;
                    outfile << "\n";
                    outfile << SAND;
                    outfile << "\n";
                }
                else if (k < y + 3 && y < chunkheight * 0.1 + playery + addtogenheigth)
                {
                    outfile << pmapwidth + x - playerx;
                    outfile << "\n";
                    outfile << k;
                    outfile << "\n";
                    outfile << SNOW;
                    outfile << "\n";
                }
                else if (k < y + 3)
                {
                    outfile << pmapwidth + x - playerx;
                    outfile << "\n";
                    outfile << k;
                    outfile << "\n";
                    outfile << GRASS;
                    outfile << "\n";
                }
                else if (k >= y + 3)
                {
                    outfile << pmapwidth + x - playerx;
                    outfile << "\n";
                    outfile << k;
                    outfile << "\n";
                    outfile << STONE;
                    outfile << "\n";
                }
            }
            if (y > chunkheight * 0.44 + playery + addtogenheigth)
            {
                for (int k = chunkheight * 0.44 + playery + addtogenheigth; k < y; k++)
                {
                    outfile << pmapwidth + x - playerx;
                    outfile << "\n";
                    outfile << k;
                    outfile << "\n";
                    outfile << WATER;
                    outfile << "\n";
                }
            }
            iterator++;
        }
    }

    if (playerx < 0)
    {
        for (int x = (int)(width / 2) - (int)(chunkwidth / 2); x < (int)(width / 2) - (int)(chunkwidth / 2) + chunkwidth; x++)
        {
            int y = (perlinNoise.at(iterator) * (float)chunkheight - (float)chunkheight / 6) + playery + addtogenheigth;

            for (int k = y; k < chunkheight; k++)
            {
                if (k < y + 3 && y > chunkheight * 0.4 + playery + addtogenheigth)
                {
                    outfile << nmapwidth + x - playerx;
                    outfile << "\n";
                    outfile << k;
                    outfile << "\n";
                    outfile << SAND;
                    outfile << "\n";
                }
                else if (k < y + 3 && y < chunkheight * 0.1 + playery + addtogenheigth)
                {
                    outfile << nmapwidth + x - playerx;
                    outfile << "\n";
                    outfile << k;
                    outfile << "\n";
                    outfile << SNOW;
                    outfile << "\n";
                }
                else if (k < y + 3)
                {
                    outfile << nmapwidth + x - playerx;
                    outfile << "\n";
                    outfile << k;
                    outfile << "\n";
                    outfile << GRASS;
                    outfile << "\n";
                }
                else if (k >= y + 3)
                {
                    outfile << nmapwidth + x - playerx;
                    outfile << "\n";
                    outfile << k;
                    outfile << "\n";
                    outfile << STONE;
                    outfile << "\n";
                }
            }
            if (y > chunkheight * 0.44 + playery + addtogenheigth)
            {
                for (int k = chunkheight * 0.44 + playery + addtogenheigth; k < y; k++)
                {
                    outfile << nmapwidth + x - playerx;
                    outfile << "\n";
                    outfile << k;
                    outfile << "\n";
                    outfile << WATER;
                    outfile << "\n";
                }
            }
            iterator++;
        }
    }
    outfile.close();
    FromFileToTiles();
    GenerateTrees();
    if (playerx >= 0)
    {
        pmapwidth = pmapwidth + chunkwidth;
        outfile.open(pmapwidthpath);
        outfile << pmapwidth;
        outfile.close();
    }

    if (playerx < 0)
    {
        nmapwidth = nmapwidth - chunkwidth;
        outfile.open(nmapwidthpath);
        outfile << nmapwidth;
        outfile.close();
    }
}

void Generate(string filename, int seed)
{
    int firsttime = 0;
    mapseed = seed;
    int tmp;

    if (mkdir("Saves") == -1)
    {
    }

    string noext = filename.substr(0, filename.size() - 4);
    string path = "./Saves/" + noext;

    const char *cstr = path.c_str();
    if (mkdir(cstr) == -1)
    {
    }

    mapfilepath = path + "/" + "world.txt";
    pmapwidthpath = path + "/" + "pmapwidth.txt";
    nmapwidthpath = path + "/" + "nmapwidth.txt";
    plastgenpath = path + "/" + "plastgen.txt";
    nlastgenpath = path + "/" + "nlastgen.txt";
    playerpospath = path + "/" + "playerpos.txt";
    playerzoompath = path + "/" + "zoomsettings.txt";

    iffile.open(playerpospath);
    if (iffile)
    {
        int tmp;
        int tmplist[1];
        int i = 0;
        while (iffile >> tmp)
        {
            tmplist[i] = tmp;
            i++;
        }
        playerx = tmplist[0];
        playery = tmplist[1];
    }
    iffile.close();

    iffile.open(playerzoompath);
    if (iffile)
    {
        int tmp;
        int tmplist[3];
        int i = 0;
        while (iffile >> tmp)
        {
            tmplist[i] = tmp;
            i++;
        }
        width = tmplist[0];
        height = tmplist[1];
        fontsize = tmplist[2];
        addtogenheigth = tmplist[3];
        SetWindow(width,height,fontsize,fontsize);
    }
    iffile.close();

    iffile.open(pmapwidthpath);
    if (iffile)
    {
        int tmp;
        while (iffile >> tmp)
        {
            pmapwidth = tmp;
        }
    }
    iffile.close();

    iffile.open(nmapwidthpath);
    if (iffile)
    {
        int tmp;
        while (iffile >> tmp)
        {
            nmapwidth = tmp;
        }
    }
    iffile.close();

    iffile.open(mapfilepath);
    if (!iffile)
    {
        firsttime = 1;
    }
    iffile.close();

    if (firsttime == 1)
    {
        GenerateInfinite();
    }
    FromFileToTiles();
}

void AddTilesToSeen(int width, int height)
{
    seentiles.clear();
    for (int i = 0; i < tiles.size(); i++)
    {
        if (tiles.at(i).getTileX() >= -1 && tiles.at(i).getTileX() < width + 1 && tiles.at(i).getTileY() >= -1 && tiles.at(i).getTileY() < height + 1)
        {
            seentiles.push_back(tiles.at(i));
        }
    }
}

void AddChangesToTiles(int width, int height)
{
    int i;
    while (i < tiles.size())
    {
        if (tiles.at(i).getTileX() >= -1 && tiles.at(i).getTileX() < width + 1 && tiles.at(i).getTileY() >= -1 && tiles.at(i).getTileY() < height + 1)
        {
            tiles.erase(tiles.begin() + i);
        }
        else
        {
            i++;
        }
    }
    for (int i = 0; i < seentiles.size(); i++)
    {
        tiles.push_back(seentiles.at(i));
    }
}
