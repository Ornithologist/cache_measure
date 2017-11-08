#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define SIZE_MAGNITUDE 16
#define MIN_ARRAY_SIZE 1024
#define REPEAT 1 << 19
#define STRIDE 32
#define GNU_CMDS 4
#define REGURGITATE 80

void setup_array(int n, int s, int *array);
int timer(int n, int s);

int main(int argc, char **argv)
{
    int one_m_itrs_ms, i, j;
    int time_ms;
    FILE *temp = fopen("size.dat", "w");
    char *gnuplot_cmds[] = {
        "set term png", "set output \"size.png\"",
        "set title \"Time for 1 million iteraion per array size\"",
        "plot 'size.dat'"};

    // warm up
    for (i = 0; i < (SIZE_MAGNITUDE / 2); i++) {
        int array_size = MIN_ARRAY_SIZE * (1 << i);
        time_ms = timer(array_size, STRIDE);
    }

    // calculate
    for (i = 0; i < SIZE_MAGNITUDE; i++) {
        int array_size = MIN_ARRAY_SIZE * (1 << i);
        time_ms = timer(array_size, STRIDE);
        printf("%d: %d ms\n", array_size, time_ms);
    }

    // draw
    // FILE *gnu_pipe = popen("gnuplot -persistent", "w");
    // for (i = 0; i < GNU_CMDS; i++) {
    //     fprintf(gnu_pipe, "%s \n", gnuplot_cmds[i]);
    // }

    // finish
    printf("Size of an array element is %d byte, finish plotting.\n",
           (int)sizeof(int));

    return 0;
}

void setup_array(int n, int s, int *array) {
    // zero all
    int i, step, current, next;
    for (i = 0; i < n; i++) {
        array[i] = 0;
    }

    // golden ratio times the number of items to access, ceil
    step = (int) ((n / s) * 0.61803398875 + 1);
    step *= s;

    // generate permutation
    current = 0;
    for (i = 0; i < n; i += s) {
        next = (current + step) % n;
        array[current / 4] = next / 4;
        current = next;
    }

    // clean up cache
    for (i = 0; i < n; i++) {
        array[i] |= 0;
    }
}

int timer_inner(int n, int s, int *array) {
    int i, p, out, rounds = (REPEAT / (n / s)) + 1;
    i = p = 0;

    clock_t start = clock(), diff;
    while(1) {
        p = array[p];
        p = array[p];
        p = array[p];
        p = array[p];
        // iterated through full permutation
        if (p == 0) {
            i += 1;
            if (i == rounds) {
                break;
            }
        }
    }
    diff = clock() - start;
    out = (1000 * 1000 * diff) / (rounds * (n / s));
    
    return out;
}

int timer(int n, int s)
{
    int i, best, cur, *array = (int *) malloc(sizeof(int) * n);

    setup_array(n, s, array);

    for (i = 0; i < REGURGITATE; i++) {
        cur = timer_inner(n, s, array);
        if (best == 0 || cur < best) {
            best = cur;
        }
    }

    // free(array);
    return best;
}