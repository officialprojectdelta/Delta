double t = 1;

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

    float y = 2.3;
    volatile int x1 = !y;
    x1 = -y;
    int z = x1++ + 3;
    short three = z;

    return x;
}

int* blah(int* t)
{
    int** test = &t;
    int y = spread(*t);
    t = &y;
    return t;
}

float test1()
{
    float x = 9.823;
    x += 8;
    x += 2.9;
    int y = 23;
    t = 9;
    return x + y;
}