double global_var = 5.5;

int test() {
    double a = 2.5;
    double *ptr = &a;

    *ptr = *ptr + global_var;
    return (int)*ptr;
}
