int test() {
    int a = 2;
    {
        a = 3;
        int a = 0;
    }
    return a;
}