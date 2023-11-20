// main.c

#include <stdio.h>
#include <string.h>

#include "kernel/pagerank.h"
#include "runtime.h"
#include "util.h"

#ifndef SPIKE
#include "printf.h"
#endif





extern uint64_t num_pages;

extern double a[] __attribute__((aligned(32 * NR_LANES), section(".l2")));  //linking matrix
extern double pr[] __attribute__((aligned(32 * NR_LANES), section(".l2")));  //score vector
extern double m[] __attribute__((aligned(32 * NR_LANES), section(".l2"))); // mean vector
extern double pr_new[] __attribute__((aligned(32 * NR_LANES), section(".l2")));  //score vector
extern double golden_o[] __attribute__((aligned(32 * NR_LANES), section(".l2")));

int main() {
     printf("\n");
    printf("=============\n");
    printf("=  PAGERANK  =\n");
    printf("=============\n");
    printf("\n");
    printf("\n");

    printf("Link matrix:\n");
    printf("---");
    for (uint64_t i = 0; i < 25; ++i) {
        for (uint64_t j = 0; j < 25; ++j) {
            printf("%ld \t", (int64_t)(a[i * 25 + j]*10000));
            //printf("%a \t", a[i * 25 + j]);
            //printf("%f \t", a[i * 25 + j]);
        }
        printf("\n");
    }

    //init_link_matrix(num_pages, a);   
    calculate_page_rank(num_pages, a, pr,m,pr_new);

    // Print the PageRank scores
    printf("PageRank Scores:\n");
    for (uint64_t i = 0; i < num_pages; i++) {
        printf("Page %d: %ld \n", i + 1,(int64_t)(pr[i]*10000));
        //printf("Page %d: %e \n", i + 1,pr[i]*100);
       
    }

    // Print the PageRank scores from goldenm model
    printf("Golden Model PageRank Scores:\n");
    for (uint64_t i = 0; i < num_pages; i++) {
        printf("Page %d: %ld \n", i + 1,(int64_t)(golden_o[i]*10000));
        //printf("Page %d: %e \n", i + 1,pr[i]*100);
       
    }

    return 0;
}
