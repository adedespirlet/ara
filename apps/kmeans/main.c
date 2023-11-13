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
extern uint64_t M;
extern uint64_t N;
extern uint64_t P;

extern int64_t a[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
extern int64_t k[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
extern int64_t c[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
extern int64_t b[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
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
    // printf("\n");
    // printf("=============\n");
    // printf("=  KMEANS  =\n");
    // printf("=============\n");
    // printf("\n");
    // printf("\n");
    int64_t runtime_s, runtime_v;   

    printf("Matrix k:\n");
    for (uint64_t i = 0; i < 3; ++i) {
        for (uint64_t j = 0; j < N; ++j) {
            printf("%ld ", k[i * N + j]);
        }
        printf("\n");
    
    printf("Matrix A:\n");
    printf("---");
    for (uint64_t i = 0; i < 3; ++i) {
        for (uint64_t j = 0; j < M; ++j) {
            printf("%ld", a[i * N + j]);
        }
        printf("\n");
    }
    

    // printf("Matrix c:\n");
    // for (uint64_t i = 0; i < M; ++i) {
        
    //     printf("%ld ", c[i]);
    //     printf("\t");
    // }

    kmeans_result result;

    // Matrices are initialized --> Start calculating
    //printf("Calculating kmeans...\n");
    //start_timer();
    //imatmul(c, a, b, s, s, s);
    result= kmeans(a, k, c,b) ;
    //printf("K-Means result: %d\n", result);
    //stop_timer();

    //runtime_s = get_timer();
   // printf("Scalar runtime: %ld\n", runtime_s);
 

  return 0;
}
