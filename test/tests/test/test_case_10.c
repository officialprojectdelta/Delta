unsigned int global_var = 15;

int test() {
    unsigned int a = 10;
    unsigned int *ptr = &a;

    *ptr = *ptr + global_var;
    return *ptr;
}
