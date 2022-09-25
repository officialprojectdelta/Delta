#include <stdio.h>

int foo(int, int);

int main(void)
{
    foo(2, 3);
    int x = 1;
    int y = 1;
    printf("%d, %d, %d", x++ + 2, y, y);
}
