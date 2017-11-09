#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define BILLION 1000000000L
#define BASE 2
#define SIZE_ORDER 18  // 256 MiB, max size of the array
#define MIN_ARRAY_SIZE 256  // 1 KiB if sizeof(int) = 4
#define STRIDE 8
#define GNU_CMDS 6
#define REGURGITATE 16

int timer(int n, int s);

int REPEAT = 16777216;  // just a big number

int main(int argc, char **argv)
{
    int one_m_itrs_ns, i, j;
    int time_ns;
    FILE *temp = fopen("size.dat", "w");
    char *gnuplot_cmds[] = {
        "set term png",
        "set output \"size.png\"",
        "set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 pi -1 ps 1.5",
        "set pointintervalbox 3",
        "set title \"Y Axis: access time | X Axis: order of array "
        "size",
        "plot 'size.dat' with linespoints ls 1"};

    // calculate
    for (i = 0; i < SIZE_ORDER; i++) {
        int array_size = MIN_ARRAY_SIZE * (1 << i);
        time_ns = timer(array_size, STRIDE);
        fprintf(temp, "%d: %d \n", i, time_ns);
    }

    // draw
    FILE *gnu_pipe = popen("gnuplot -persistent", "w");
    for (i = 0; i < GNU_CMDS; i++) {
        fprintf(gnu_pipe, "%s \n", gnuplot_cmds[i]);
    }

    // finish
    printf("Size of an array element is %d byte. Finished plotting.\n",
           (int)sizeof(double));
    return 0;
}

void setup_array(int n, int s, int *array)
{
    int i;
    for (i = 0; i < n; i++) {
        array[i] = 1;
    }
}

int timer_inner(int n, int s)
{
    int out, i, nmask = n - 1;
    int *array = (int *)malloc(sizeof(int) * n);
    unsigned long long diff;
    struct timespec start, end;

    // touch evefything for once to warm up
    setup_array(n, s, array);

    // measure access for REPEAT times
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (i = 0; i < REPEAT; i++) {
        ++array[(i * s) & nmask];
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
    out = (int)(diff / 1000);

    free(array);
    return out;
}

int timer(int n, int s)
{
    int i, cur = 0, best = 0;

    for (i = 0; i < REGURGITATE; i++) {
        cur = timer_inner(n, s);
        if (best == 0 || cur < best) {
            best = cur;
        }
    }

    return best;
}