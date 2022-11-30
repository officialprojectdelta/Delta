int t = 37;

int spread(int x);

int test() 
{
    int ret = spread(3) + t;
    return ret + t - 8;
}

int spread(int x)
{
    for (int i = 0; i < 4; i++)
    {
        x = x + i / 4;
    }

    return x;
}