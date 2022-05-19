class Tile
{
public:
    int x;
    int y;
    int material;

    Tile(int tx, int ty, int tmaterial)
    {
        x = tx;
        y = ty;
        material = tmaterial;
    }

    // Setter
    void setTilePos(int tx, int ty)
    {
        x = tx;
        y = ty;
    }
    void setTileX(int tx)
    {
        x = tx;
    }
    void setTileY(int ty)
    {
        y = ty;
    }
    void setMaterial(int tmaterial)
    {
        material = tmaterial;
    }


    // Getter
    int getTilePos()
    {
        return x, y;
    }
    int getTileX()
    {
        return x;
    }
    int getTileY()
    {
        return y;
    }
    int getMaterial()
    {
        return material;
    }
};