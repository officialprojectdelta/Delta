int* get_max(int *a, int *b) {
    return *a > *b ? a : b;
}

int a[] = {1, 2, 3, 4, 5};

int test() {
    int ae[] = {1, 2, 3, 4, 5};
    int a = 5;
    int b = 10;
    char c = '\n';
    int *max_ptr = get_max(&a, &b);

    return *max_ptr + c;
}
