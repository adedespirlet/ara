
// main.c

#include <string.h>

#include "kernel/pagerank.h"
#include "runtime.h"
#include "util.h"

#ifndef SPIKE
#include "printf.h"
#else 
#include <stdio.h>
#endif

extern uint64_t num_pages;

extern double data_array[] __attribute__((aligned(32 * NR_LANES), section(".l2")));  //CSR data
extern uint64_t col_array[] __attribute__((aligned(32 * NR_LANES), section(".l2")));  //CSR col array
extern uint64_t row_ptr[] __attribute__((aligned(32 * NR_LANES), section(".l2")));  //CSR rowpointer array
extern int64_t distances[] __attribute__((aligned(32 * NR_LANES), section(".l2"))); // mean vector
extern int64_t B[] __attribute__((aligned(32 * NR_LANES), section(".l2")));  //score vector new
extern int64_t List[] __attribute__((aligned(32 * NR_LANES), section(".l2")));  //score vector
extern int64_t S[] __attribute__((aligned(32 * NR_LANES), section(".l2")));

int main() {
 	printf("\n");
	printf("=============\n");
	printf("=  PAGERANK  =\n");
	printf("=============\n");
	printf("\n");
	printf("\n");
  
	sssp(num_pages, data_array,col_array,row_ptr, pr,m,pr_new);

	// Print the PageRank scores
	printf("\nPageRank Scores:\n");
	for (uint64_t i = 0; i < 10; i++) {
    	printf("Page %d: %ld \t", i + 1,(int64_t)(pr[i]*10000));
    	//printf("Page %d: %e \n", i + 1,pr[i]*100);
  	 
	}

	

	return 0;
}