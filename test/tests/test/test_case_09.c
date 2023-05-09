int global_var = 10;

int test() {
    int a = 5;
    int *ptr = &a;

    *ptr = *ptr + global_var;
    return *ptr;
}
