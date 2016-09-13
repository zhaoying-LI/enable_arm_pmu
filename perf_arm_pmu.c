/** compile with -std=gnu99 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "armpmu_lib.h"

/* Simple loop body to keep things interested. Make sure it gets inlined. */
static inline int
loop(int* __restrict__ a, int* __restrict__ b, int n)
{
        unsigned sum = 0;
        for (int i = 0; i < n; ++i)
                if(a[i] > b[i])
                        sum += a[i] + 5;
        return sum;
}

int
main(int ac, char **av)
{
        uint32_t time_start = 0;
        uint32_t time_end   = 0;
	uint32_t cnt_start0 = 0;
	uint32_t cnt_end0 = 0;
	uint32_t cnt_start1 = 0;
	uint32_t cnt_end1 = 0;

        int *a  = NULL;
        int *b  = NULL;
        int len = 0;
        int sum = 0;

        if (ac != 2) return -1;
        len = atoi(av[1]);
	printf("%s: len = %d\n", av[0], len);

        a = malloc(len*sizeof(*a));
        b = malloc(len*sizeof(*b));

        for (int i = 0; i < len; ++i) {
                a[i] = i+128;
                b[i] = i+64;
        }

        printf("%s: beginning loop\n", av[0]);
        time_start = rdtsc32();
        sum = loop(a, b, len);
        time_end   = rdtsc32();
        printf("%s: done. sum = %d; time delta = %u\n", av[0], sum, time_end - time_start);

        printf("%s: beginning loop\n", av[0]);
	enable_pmu0(0x008);
	enable_pmu1(0x013);
        cnt_start0 = read_pmu0();
        cnt_start1 = read_pmu1();
        sum = loop(a, b, len);
        cnt_end0   = read_pmu0();
        cnt_end1   = read_pmu1();
	disable_pmu0(0x008);
	disable_pmu1(0x013);
        printf("%s: done. sum = %d; event 0x%03x delta = %u\n", av[0], sum, 0x008, cnt_end0 - cnt_start0);
        printf("%s: done. sum = %d; event 0x%03x delta = %u\n", av[0], sum, 0x008, cnt_end1 - cnt_start1);

        free(a); free(b);
        return 0;
}
