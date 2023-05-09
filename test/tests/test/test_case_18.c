int test() {
    float a = 7.0;
    float b = 3.0;
    int result;

    for (int i = 0; i < 5; i++) {
        a = a * 2.0;
        b = b / 2.0;
        result = (int)(a + b);
    }

    return result;
}
