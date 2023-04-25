int foo(int b);

int test(){
    return foo(3);
}

int foo(int a){
    return a + 1;
}