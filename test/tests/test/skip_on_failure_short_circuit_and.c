int test() {
    int a = 0;
    int b = 0;
    a && (b = 5);
    return b;
}