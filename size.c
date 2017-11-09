#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define BILLION 1000000000L
#define BASE 2
#define REPEAT 16777216  // 2 ^ 24, just a bit number
#define SIZE_ORDER 16  // 512 MiB, max size of the array
#define MIN_ARRAY_SIZE 128  // 1 KiB if sizeof(double) = 8
#define STRIDE 1
#define GNU_CMDS 6
#define REGURGITATE 10
#define TIME_TO_ORDER(time) (int)((log(time) / log(BASE)) + 1)

struct time_in {
    unsigned long long timens;
    int accessc;
};

void setup_array(int n, int s, double *array);
int timer(int n, int s);
int timer_outer(int n, int s);
struct time_in timer_inner(int n, int s);

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
        "set title \"X Axis: access time | Y Axis: order of array "
        "size",
        "plot 'size.dat' with linespoints ls 1"};

    // calculate
    for (i = 0; i < SIZE_ORDER; i++) {
        int order, array_size = MIN_ARRAY_SIZE * (1 << i);
        time_ns = timer(array_size, STRIDE);
        printf("%d: %d ns\n", i, time_ns);
        order = (time_ns <= 0) ? 1 : TIME_TO_ORDER(time_ns);
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

void setup_array(int n, int s, double *array)
{
    int i;
    for (i = 0; i < n; i++) {
        array[i] = 1.0;
    }
}

struct time_in timer_inner(int n, int s)
{
    int out, i, p = 0;
    double *array = (double *)malloc(sizeof(double) * n);
    struct time_in curtime = {0, 0};

    // touch evefything for once to warm up
    setup_array(n, s, array);

    unsigned long long diff;
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start); /* mark start time */
    while (p < n) {
        array[p] *= 1.5;
        i++;
        p += s;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);   /* mark the end time */

    diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
    curtime.timens = diff;
    curtime.accessc = i;

    free(array);
    return curtime;
}

int timer_outer(int n, int s)
{
    int access_count = 0;
    unsigned long long total_time = 0;

    while (access_count < REPEAT) {
        struct time_in curtime = timer_inner(n, s);
        access_count += curtime.accessc;
        total_time += curtime.timens;
    }

    int out = (int)((1000 * total_time) / access_count);
    printf("out: %d, access_count: %d\n", out, access_count);
    return out;
}

int timer(int n, int s)
{
    int i, total = 0;

    for (i = 0; i < REGURGITATE; i++) {
        total += timer_outer(n, s);
        // if (best == 0 || cur < best) {
        //     best = cur;
        // }
    }

    total /= REGURGITATE;
    return total;
}