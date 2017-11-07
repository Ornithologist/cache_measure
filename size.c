#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MAX_ARRAY_SIZE 100000
#define MIN_ARRAY_SIZE 3
#define REPEAT 1000
#define STRIDE 2
#define JUMP 10
#define GNU_CMDS 4

unsigned long long timer(int n, int s);

int main(int argc, char **argv)
{
    int one_m_itrs_ms, i, j;
    unsigned long long time_ms;
    FILE *temp = fopen("size.dat", "w");
    char *gnuplot_cmds[] = {
        "set term png", "set output \"size.png\"",
        "set title \"Time for 1 million iteraion per array size\"",
        "plot 'size.dat'"};

    // calculate
    for (i = MIN_ARRAY_SIZE; i < MAX_ARRAY_SIZE; i += JUMP) {
        time_ms = 0;
        for (j = 0; j < REPEAT; j++) {
            time_ms += timer(i, STRIDE);
        }
        one_m_itrs_ms = (time_ms * 1000 * 1000) / (REPEAT * i);
        fprintf(temp, "%d %d \n", i, one_m_itrs_ms);
        // printf("%d: %d ms\n", i, one_m_itrs_ms);
    }

    // draw
    FILE *gnu_pipe = popen("gnuplot -persistent", "w");
    for (i = 0; i < GNU_CMDS; i++) {
        fprintf(gnu_pipe, "%s \n", gnuplot_cmds[i]);
    }

    // finish
    printf("Size of an array element is %d byte, finish plotting.\n",
           (int)sizeof(int));

    return 0;
}

unsigned long long timer(int n, int s)
{
    int i, *array = (int *)malloc(sizeof(int) * n);

    for (i = 0; i < n; i++) {
        array[i] = 0;
    }

    clock_t start = clock(), diff;
    for (i = 0; i < n; i += s) {
        array[i] += 1;
    }
    diff = clock() - start;

    free(array);
    return diff;
}