int global = 42;

int test1(int x)
{
    return x + 4;
}

int test()
{
    int x = 4;
    x = global;
    x = test1(x);

    while (x != 4)
    {
        x--;
        if (x) 
        {
            if (1 == 0) break;
        }
    }

    do {
        x--;
        if (x) 
        {
            if (1 == 0) break;
        }
    } while (x != 4);

    return x;
}