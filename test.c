int global = 42;

int test1(int x)
{
    return x + 4;
}

int main()
{
    int x = 4;
    x = global;
    float g = 2.3;
    if (x == 4)
    {
        x = test1(x);
    }
    return x;
}