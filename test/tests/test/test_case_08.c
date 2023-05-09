int product(unsigned int a, unsigned int b) {
    return a * b;
}

int test() {
    unsigned int x = 3;
    unsigned int y = 4;
    unsigned int result = product(x, y);
    return product(result, y);
}
