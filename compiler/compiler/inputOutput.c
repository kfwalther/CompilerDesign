#include <stdio.h>
#include <stdint.h>

int32_t input() {
    int val = 0;
    printf("Read in: ");
    scanf("%d", &val);
    return val;
}

void output(int32_t val) {
    printf("Output: %d\n", val);
}