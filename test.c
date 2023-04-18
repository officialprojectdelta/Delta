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

    for (int i = 0; i < 5; i++)
    {
        if (i == 4) continue;
        x = x * 2;
    }

    return x;
}