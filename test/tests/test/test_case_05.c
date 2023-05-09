int test() {
    int x = 5;
    unsigned int y = 10;
    int *p1 = &x;
    unsigned int *p2 = &y;
    return *p1 + *p2;
}
