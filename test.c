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

    return x;
}