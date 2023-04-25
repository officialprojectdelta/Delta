int foo(int a);

int test(){
    return foo(3);
}

int foo(int a){
    return a + 1;
}