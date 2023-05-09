int* get_max(int *a, int *b) {
    if (*a > *b) 
    {
        return a;
    } else {
        return b;
    }
}

int test() {
    int a = 5;
    int b = 10;
    int *max_ptr = get_max(&a, &b);

    return *max_ptr;
}
