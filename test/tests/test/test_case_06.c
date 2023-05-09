int test() {
    double x = 3.14;
    float y = 6.28;
    double *p1 = &x;
    float *p2 = &y;
    return (int)(*p1 + *p2);
}
