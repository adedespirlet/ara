#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "kernel/kmeans.h"
#include "runtime.h"
#include "util.h"

#ifndef SPIKE
#include "printf.h"
#endif

// Define Matrix dimensions:
// C = AB with A=[MxN], B=[NxP], C=[MxP]
extern uint64_t data_points;
extern uint64_t dimension;
extern uint64_t P;

extern int64_t a[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
extern int64_t k[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
extern int64_t c[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
extern int64_t b[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
extern int64_t golden_o[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
// Gold results
//extern int64_t g[] __attribute__((aligned(32 * NR_LANES), section(".l2")));

// // Verify the matrix
// int verify_matrix(int64_t *result, int64_t *gold, size_t R, size_t C) {
//   for (uint64_t i = 0; i < R; ++i) {
//     for (uint64_t j = 0; j < C; ++j) {
//       uint64_t idx = i * C + j;
//       if (result[idx] != gold[idx]) {
//         return (i + j) == 0 ? -1 : idx;
//       }
//     }
//   }
//   return 0;
// }

int main() {
    printf("\n");
    printf("=============\n");
    printf("=  KMEANS  =\n");
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
    

    // printf("Matrix c:\n");
    // for (uint64_t i = 0; i < data_points; ++i) {
        
    //     printf("%ld ", c[i]);
    //     printf("\t");
    // }

    kmeans_result result;

    // Matrices are initialized --> Start calculating
    //printf("Calculating kmeans...\n");
    //start_timer();
    //imatmul(c, a, b, s, s, s);
    result= kmeans(a, k, c,b,data_points,dimension) ;
    

    printf("Matrix c:\n");
    for (uint64_t i = 0; i < data_points; ++i) {
        
        printf("%ld ,", c[i]);
    }

    printf("\n Golden Model output for Cluster vector:\n");
    for (uint64_t i = 0; i < data_points; ++i) {
        
        printf("%ld ,", golden_o[i]);
    }

    printf("\nfinished printing");

    //printf("K-Means result: %d\n", result);
    //stop_timer();

    //runtime_s = get_timer();
   // printf("Scalar runtime: %ld\n", runtime_s);
 

  return 0;
}
