int test1[5];

int test() {
    int ba = 5;
    int* aptr = &ba;
    int a = aptr[0];

    return *(test1 + 1);
}
