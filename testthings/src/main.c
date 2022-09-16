#include <stdio.h>

int main(void)
{
    int a = 3;
    int b = 4;
    int c = 7;
    a = b * (c = 2 * 234);
    printf("%d", a);
}