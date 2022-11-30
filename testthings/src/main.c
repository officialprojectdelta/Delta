#include <stdio.h>

int test(int x) {
    x ? "98" : 8;
}

int main(void)
{
    printf("%d", test(3));
}
