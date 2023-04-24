#include <stdio.h>

extern int test();

int main(void)
{
    printf("%d", test());
}

