#ifndef KMEANS_H
#define KMEANS_H
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h> // Include the header for DBL_MAX
#include <time.h>
#include <stddef.h>
#include <stdbool.h>

#define NUM_POINTS 100
#define NUM_CLUSTERS 3
#define MAX_ITERATIONS 70   // How many times is the cluster updated
#define NUM_RANDOMPOINTS 3  // Picking different initialization values 
#define SIZE_DATAPOINT 3


typedef enum {
	KMEANS_OK,
	KMEANS_EXCEEDED_MAX_ITERATIONS,
	KMEANS_ERROR
} kmeans_result;


// Function prototypes for distance and centroid methods

void assignPointsToClusters(const int64_t *points, const int64_t *centers, int64_t *clusters);
void updateClusterCenters(const int64_t *points, int64_t *centers, int64_t *clusters);
void assessQualityCluster(const int64_t *points, int64_t *centers, int64_t *clusters);
void custom_memcpy(int64_t *dest, int64_t *src, size_t size);
bool custom_memcmp(const int64_t *array1, const int64_t *array2, size_t size);

kmeans_result kmeans(const int64_t *points, int64_t *centers, int64_t *clusters, int64_t *clusters_last);


#endif
