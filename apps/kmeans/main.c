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
  printf("\n");
  printf("=============\n");
  printf("=  KMEANS  =\n");
  printf("=============\n");
  printf("\n");
  printf("\n");


    printf("Matrix k:\n");
    for (uint64_t i = 0; i < 3; ++i) {
        for (uint64_t j = 0; j < N; ++j) {
            printf("%ld ", k[i * N + j]);
        }
        printf("\n");
    }

    printf("Matrix c:\n");
    for (uint64_t i = 0; i < M; ++i) {
        
        printf("%ld ", c[i]);
        printf("\t");
    }

    kmeans_result result;

    // Matrices are initialized --> Start calculating
    printf("Calculating kmeans...\n");
    //start_timer();
    //imatmul(c, a, b, s, s, s);
    result= kmeans(a, k, c,b) ;
    printf("K-Means result: %d\n", result);
    //stop_timer();

  //   // Metrics
  //   int64_t runtime = get_timer();
  //   float performance = 2.0 * s * s * s / runtime;
  //   float utilization = 100 * performance / (2.0 * NR_LANES);

  //   printf("The execution took %d cycles.\n", runtime);
  //   printf("The performance is %f OP/cycle (%f%% utilization).\n", performance,
  //          utilization);

  //   // Verify the result only for s == M (to keep it simple)
  //   if (s == M) {
  //     // Verify the result
  //     printf("Verifying result...\n");
  //     int error = verify_matrix(c, g, s, s);
  //     if (error != 0) {
  //       printf("Error code %d\n", error);
  //       printf("c[%d]=%d\n", error, c[error]);
  //       return error;
  //     } else {
  //       printf("Passed.\n");
  //     }
  //   }
  // }

  return 0;
}
