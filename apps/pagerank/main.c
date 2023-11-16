// main.c

#include <stdio.h>
#include "pagerank.h"


extern uint64_t num_pages;

extern double a[] __attribute__((aligned(32 * NR_LANES), section(".l2")));  //linking matrix
extern double pr[] __attribute__((aligned(32 * NR_LANES), section(".l2")));  //score vector
extern double m[] __attribute__((aligned(32 * NR_LANES), section(".l2"))); // mean vector


int main() {
  
    init_link_matrix(num_pages, link_matrix);   
    calculate_page_rank(num_pages, a, pr,m);

    // Print the PageRank scores
    printf("PageRank Scores:\n");
    for (int i = 0; i < num_pages; i++) {
        printf("Page %d: %f\n", i + 1, score_column[i]);
    }

    return 0;
}
