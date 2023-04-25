int test() {
    int a = 1;
    {
        int a = 2;
    }
    {
        return a;
    }
}