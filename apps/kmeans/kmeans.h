#ifndef KMEANSNEW_H
#define KMEANSNEW_H
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h> // Include the header for DBL_MAX
#include <time.h>

#define NUM_POINTS 100
#define NUM_CLUSTERS 3
#define MAX_ITERATIONS 10   // How many times is the cluster updated
#define NUM_RANDOMPOINTS 3  // Picking different initialization values 
#define SIZE_DATAPOINT 2
#define kmeans_free(ptr) free(ptr)
#define kmeans_malloc(size) malloc(size)


typedef enum {
	KMEANS_OK,
	KMEANS_EXCEEDED_MAX_ITERATIONS,
	KMEANS_ERROR
} kmeans_result;

typedef void * Pointer;

// Function prototypes for distance and centroid methods

void assignPointsToClusters(double** points, double** centers, int* clusters);
void updateClusterCenters(double** points, double** centers, int* clusters);
kmeans_result kmeans(double** points, double** centers, int* clusters);
void assessQualityCluster(double** points, double** centers, int* clusters);


#endif