int global = 42;
char gb2 = 3213;

int test1(int x)
{
    return x + 4;
}

int test()
{
    char li = -5;
    int x = 4;
    x = global;
    x = test1(x + li);

    return x;
}