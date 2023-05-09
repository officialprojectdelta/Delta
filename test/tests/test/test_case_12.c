int add(int *a, int *b) {
    return *a + *b;
}

int test() {
    int a = 5;
    int b = 10;
    int result = add(&a, &b);

    return result;
}
