int test()
{
    int* a;
    int b = 3;
    a = &b;
    *a = 4;
    return b;
}