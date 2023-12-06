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
extern uint64_t delta;
extern uint64_t source;
extern uint64_t data_array[] __attribute__((aligned(32 * NR_LANES), section(".l2")));  //CSR data
extern uint64_t col_array[] __attribute__((aligned(32 * NR_LANES), section(".l2")));  //CSR col array
extern uint64_t row_ptr[] __attribute__((aligned(32 * NR_LANES), section(".l2")));  //CSR rowpointer array
extern uint64_t distances[] __attribute__((aligned(32 * NR_LANES), section(".l2"))); // mean vector
extern uint64_t B[] __attribute__((aligned(32 * NR_LANES), section(".l2")));  //score vector new
extern uint64_t List[] __attribute__((aligned(32 * NR_LANES), section(".l2")));  //score vector
extern uint64_t S[] __attribute__((aligned(32 * NR_LANES), section(".l2")));

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

    printf("Matrix k:\n");
    for (uint64_t i = 0; i < 3; ++i) {
        for (uint64_t j = 0; j < dimension; ++j) {
            printf("%ld ", k[i * dimension + j]);
        }
        printf("\n");
    }
    
    printf("Matrix A:\n");
    printf("---");
    for (uint64_t i = 0; i < 3; ++i) {
        for (uint64_t j = 0; j < data_points; ++j) {
            printf("%ld", a[i * dimension + j]);
        }
        printf("\n");
    }
   
    sssp(data_array,col_array,row_ptr,distances,B,List,S,num_nodes,delta, source) ;
    

    printf("Matrix c:\n");
    for (uint64_t i = 0; i < data_points; ++i) {
        
        printf("%ld ,", c[i]);
    }

 

    printf("\nfinished printing");


  return 0;
}