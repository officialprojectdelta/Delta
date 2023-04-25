int a = 3;

int test() {
    int ret = 0;
    if (a) {
        int a = 0;
        ret = 4;
    }
    return ret;
}