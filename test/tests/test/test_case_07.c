int sum(int a, int b) {
    return a + b;
}

int test() {
    int x = 5;
    int y = 10;
    int sumed = sum(x, y);
    return sum(sumed, y);
}
