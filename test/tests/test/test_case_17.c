int test() {
    unsigned int a = 15;
    unsigned int b = 20;
    unsigned int result;

    while (a < b) {
        result = a * b;
        a++;
    }

    return result;
}
