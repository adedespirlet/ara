#ifndef SSSP_H
#define SSSP_H

#include <stdint.h>

#ifndef SPIKE
#include "printf.h"
#else
#include <stdio.h>
#endif

// Assuming a predefined maximum number of vertices
#define MAX_VERTICES 1000
#define INFINITY 1000000
#define DELTA 10  // Example delta value

// Node structure for the bucket's linked list
typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

// Function declarations
void addToBucket(Node *List, Node **B, int64_t vertex, int64_t bucketid, uint64_t num_nodes);
int findSmallestNonEmptyBucket(Node **B, uint64_t num_nodes, int64_t delta);
void processBucket(int64_t *data_array, uint64_t *col_array, uint64_t *row_ptr, Node **B, int64_t bucketIndex, uint64_t num_nodes, int64_t delta, int64_t *distances, int64_t *Req_l, int64_t *Req_h,Node *List);
void relax(int64_t v, int64_t new_dist,  int64_t delta,  int64_t *distances,Node **B, Node *List, uint64_t num_nodes);
void sssp(int64_t *data_array, uint64_t *col_array, uint64_t *row_ptr, int64_t *distances, int64_t *B, int64_t *List, uint64_t num_nodes, int64_t delta, uint64_t source, int64_t *ReqL, int64_t *ReqH);

#endif // SSSP_H
