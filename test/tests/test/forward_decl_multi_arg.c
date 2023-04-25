int foo(int a, int b);

int test() {
    return foo(1, 2);
}

int foo(int x, int y){
    return x - y;
}