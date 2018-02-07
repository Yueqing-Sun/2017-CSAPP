#include <stdio.h>

int cpu_bits(void *dummy1, void *dummy2);

int main() {
    printf("%d\n", cpu_bits(NULL, NULL));
    printf("64\n");
    printf("%d\n", 8 * sizeof(long *));
    return 0;
}

int cpu_bits(void *dummy1, void *dummy2) {
    long offset = (long) &dummy2 - (long) &dummy1;
    int ret = 0;
    if (8 == offset)
        ret = 64;
    else if (4 == offset)
        ret = 32;
    else if (2 == offset)
        ret = 16;
    else if (1 == offset)
        ret = 8;
    else
        ret = -1;
    return ret;
}