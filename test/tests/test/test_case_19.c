int test() {
    double a = 0.5;
    double b = 1.5;
    int result;

    do {
        a = a + 0.1;
        b = b - 0.1;
        result = (int)(a * b);
    } while (a < b);

    return result;
}
