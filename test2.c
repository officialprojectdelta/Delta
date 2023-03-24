double t = 1;

int spread(int x);

int test() 
{
    int ret = spread(3) + t;
    return ret + t - 8;
}

// int spread(int x)
// {
//     for (int i = 0; i < 4; i++)
//     {
//         x = x + i / 4;
//     }

//     return x;
// }

int* blah(int* x)
{
    int y = spread(*x);
    x = &y;
    return x;
}

float test1()
{
    float x = 9.823;
    x += 8;
    x += 2.9;
    int y = 23;
    return x + y;
}