
#ifndef KMEANS_H
#define KMEANS_H
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <float.h> // Include the header for DBL_MAX
#include <time.h>
#include <stddef.h>
#include <stdbool.h>


#define NUM_CLUSTERS 3
#define MAX_ITERATIONS 50   // How many times is the cluster updated

typedef enum {
   KMEANS_OK,
   KMEANS_EXCEEDED_MAX_ITERATIONS,
   KMEANS_ERROR
} kmeans_result;


// Function prototypes for distance and centroid methods

void assignPointsToClusters(const int64_t *points, const int64_t *centers, int64_t *clusters,unsigned long int num_points, unsigned long int dimension);
void updateClusterCenters(const int64_t *points, int64_t *centers, int64_t *clusters,unsigned long int num_points, unsigned long int dimension);
void assessQualityCluster(const int64_t *points, int64_t *centers, int64_t *clusters,unsigned long int num_points, unsigned long int dimension);
void custom_memcpy(int64_t *dest, int64_t *src, size_t size);
bool custom_memcmp(const int64_t *array1, const int64_t *array2, size_t size);


kmeans_result kmeans(const int64_t *points, int64_t *centers, int64_t *clusters, int64_t *clusters_last,unsigned long int num_points, unsigned long int dimension);


#endif
