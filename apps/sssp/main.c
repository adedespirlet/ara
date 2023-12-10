#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "kernel/sssp.h"
#include "runtime.h"
#include "util.h"

#ifndef SPIKE
#include "printf.h"
#endif

extern uint64_t num_nodes;
extern int64_t delta;
extern uint64_t source;
extern int64_t data_array[] __attribute__((aligned(32 * NR_LANES), section(".l2")));  //CSR data
extern uint64_t col_array[] __attribute__((aligned(32 * NR_LANES), section(".l2")));  //CSR col array
extern uint64_t row_ptr[] __attribute__((aligned(32 * NR_LANES), section(".l2")));  //CSR rowpointer array
extern int64_t distances[] __attribute__((aligned(32 * NR_LANES), section(".l2"))); // mean vector
extern int64_t B[] __attribute__((aligned(32 * NR_LANES), section(".l2")));  //score vector new
extern int64_t List[] __attribute__((aligned(32 * NR_LANES), section(".l2")));  //score vector
extern int64_t ReqdL[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
extern int64_t ReqdH[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
extern int64_t ReqvL[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
extern int64_t ReqvH[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
extern int64_t golden_o[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
// Gold results
//extern int64_t g[] __attribute__((aligned(32 * NR_LANES), section(".l2")));



int main() {
    printf("\n");
    printf("=============\n");
    printf("=  SSSP  =\n");
    printf("=============\n");
    printf("\n");
    printf("\n");
    int64_t runtime_s, runtime_v;   
    uint64_t nonzeros=118;

    printf("Data array:\n");
    
    for (uint64_t j = 0; j < nonzeros; ++j) {
        printf("%ld ", data_array[j]);
    }
    printf("\n");
    

    start_timer();
    sssp(data_array,col_array,row_ptr,distances,B,List,num_nodes,delta, source, ReqdL, ReqdH,ReqvL,ReqvH) ;
 	stop_timer();

	// Performance metrics
  	int64_t runtime = get_timer();

  	//float num_nonzero_elements=2563;
  
	//float performance = (num_pages+num_iterations*(num_nonzero_elements+3*num_pages+num_nonzero_elements-num_pages))/ runtime;
  	//float utilization = 100 * performance / (2.0 * NR_LANES);
   

    printf("distances array:\n");
 
    for (uint64_t j = 0; j < num_nodes; ++j) {
        printf("%ld ", distances[j]);
    }
    printf("\n");

    printf("Golden Output\n");
     for (uint64_t j = 0; j < num_nodes; ++j) {
        printf("%ld ", golden_o[j]);
    }

    
    // printf("Matrix c:\n");
    // for (uint64_t i = 0; i < data_points; ++i) {
        
    //     printf("%ld ,", c[i]);
    // }

 

    printf("\nfinished printing");


  return 0;
}