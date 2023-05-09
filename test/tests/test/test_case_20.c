int test() {
    int a = 1;
    int b = 2;
    int c = 3;
    int result;

    if (a < b && b < c) {
        result = a * b * c;
    } else {
        result = a + b + c;
    }

    return result;
}
