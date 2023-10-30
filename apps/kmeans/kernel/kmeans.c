#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h> // Include the header for DBL_MAX
#include <time.h>
#include "kmeans.h"
#include <string.h>



void assignPointsToClusters(const int64_t *points, const int64_t *centers, int64_t *clusters) {
    for (int i = 0; i < NUM_POINTS; i++) {
        double minDist = DBL_MAX;
        for (int c = 0; c < NUM_CLUSTERS; c++) {
            double sum = 0;
            for (int j = 0; j < SIZE_DATAPOINT; j++) {
                sum += pow(points[i * SIZE_DATAPOINT + j] - centers[c * SIZE_DATAPOINT + j], 2);
            }
            double dist = sqrt(sum);
            if (dist < minDist) {
                minDist = dist;
                clusters[i] = c;
            }
        }
    }
}


void updateClusterCenters(const int64_t *points, int64_t *centers, int64_t *clusters) {
    double sum[NUM_CLUSTERS][SIZE_DATAPOINT];
    double numbersInCluster[NUM_CLUSTERS];

    for (int i = 0; i < NUM_CLUSTERS; i++) {
        for (int d = 0; d < SIZE_DATAPOINT; d++) {
            sum[i][d] = 0;
        }
        numbersInCluster[i] = 0;
    }

    for (int j = 0; j < NUM_POINTS; j++) {
        int cluster = clusters[j];
        if (cluster >= 0 && cluster < NUM_CLUSTERS) {
            for (int d = 0; d < SIZE_DATAPOINT; d++) {
                // Assuming your points are represented as a 2D array,
                // you would access elements like this:
                sum[cluster][d] += points[j * SIZE_DATAPOINT + d];
            }
            numbersInCluster[cluster]++;
        }
    }

    for (int i = 0; i < NUM_CLUSTERS; i++) {
        if (numbersInCluster[i] > 0) {
            for (int d = 0; d < SIZE_DATAPOINT; d++) {
                centers[i * SIZE_DATAPOINT + d] = sum[i][d] / numbersInCluster[i];
            }
        }
    }
}


void assessQualityCluster(const int64_t *points, int64_t *centers, int64_t *clusters) {
    int64_t totalVariation = 0.0;
    int64_t clusterVariance=0.0;

    // For each cluster
    for (int clusterIndex = 0; clusterIndex < NUM_CLUSTERS; clusterIndex++) {
        int64_t sumOfSquaredDistances = 0.0;
        int64_t numPointsInCluster = 0;

        // For each data point
        for (int dataIndex = 0; dataIndex < NUM_POINTS; dataIndex++) {
            // Check if the data point belongs to the current cluster
            if (clusters[dataIndex] == clusterIndex) {
                // Calculate the squared distance between the data point and the cluster center
                int64_t squaredDistance = 0.0;
                for (int dimension = 0; dimension < SIZE_DATAPOINT; dimension++) {
                    int64_t diff = points[dataIndex * SIZE_DATAPOINT + dimension] - centers[clusterIndex * SIZE_DATAPOINT + dimension];
                    squaredDistance += diff * diff;
                }
                sumOfSquaredDistances += squaredDistance;
                numPointsInCluster++;
            }
        }

        if (numPointsInCluster > 0) {
            // Calculate the average squared distance (variance) within the cluster
            clusterVariance = sumOfSquaredDistances / numPointsInCluster;
            totalVariation += clusterVariance;
            printf("Cluster %d Variance: %ld\n", clusterIndex, clusterVariance);
        }
    }

    printf("Total Variation: %ld\n", totalVariation);
}


void custom_memcpy(int64_t*dest, int64_t *src, size_t size) {
    char *d = (char *)dest;
    const char *s = (const char *)src;
    for (size_t i = 0; i < size; i++) {
        d[i] = s[i];
    }
}

bool custom_memcmp(const int64_t *array1, const int64_t *array2, size_t size) {
    for (size_t i = 0; i < size; i++) {
        if (array1[i] != array2[i]) {
            return false;
        }
    }
    return true;
}


kmeans_result kmeans( const int64_t *points,  int64_t *centers,  int64_t *clusters,int64_t *clusters_last){
	int iterations = 0;
	
    size_t clusters_sz = NUM_POINTS * sizeof(int);


	
	while (1)
	{
		/* Store the previous state of the clustering */
	        custom_memcpy(clusters_last, clusters, clusters_sz);

		assignPointsToClusters(points, centers,clusters);
		printf("Matrix C:\n");
		for (uint64_t i = 0; i < NUM_POINTS; ++i) {
        		printf("%ld ", clusters[i]);
			printf("\t");
		}
		updateClusterCenters(points, centers,clusters);
       		 assessQualityCluster(points,centers,clusters);

		/*
		 * if all the cluster numbers are unchanged since last time,
		 * we are at a stable solution, so we can stop here
		 */
		if (custom_memcmp(clusters_last, clusters, clusters_sz)){
		
			//kmeans_free(clusters_last);
			//total_iterations = iterations;
			return KMEANS_OK;
		}

		if (iterations++ > MAX_ITERATIONS)
		{
			//kmeans_free(clusters_last);
			//total_iterations = iterations;
			return KMEANS_EXCEEDED_MAX_ITERATIONS;
		}
	}

	//kmeans_free(clusters_last);
	//total_iterations = iterations;
	return KMEANS_ERROR;
}


// int main() {
//     // unsigned int* total_iterations;
  
//     kmeans_result result;
    
//     // Allocate memory 
//     double** points = (double**)malloc(NUM_POINTS * sizeof(double*));
//     double** centers= (double**)malloc(NUM_CLUSTERS * sizeof(double*));
//     int* clusters= (int*)malloc(NUM_POINTS* sizeof(int));
//     srand(time(NULL)); // Set the seed based on the current time

//     if (points) {
//         // Populate the array with coordinate values
//         for (int i = 0; i < NUM_POINTS; i++) {
//             points[i] = (double*)malloc(SIZE_DATAPOINT * sizeof(double));

//             if (!points[i]) {
//                 // Handle allocation failure
//                 return 1;
//             }

//             // Fill in the coordinates for the current point
//             for (int j = 0; j < SIZE_DATAPOINT; j++) {
//                 points[i][j] = (rand() % 1000) / 10.0;  // Example: Assigning the same value for simplicity
//                 //printf("%f\n",points[i][j]);
//             }
//         }
//     }
//     if (centers){
//     // Populate the array with coordinate values
//         for (int i = 0; i < NUM_CLUSTERS; i++) {
//             centers[i] = (double*)malloc(SIZE_DATAPOINT * sizeof(double));

//             if (!centers[i]) {
//                 // Handle allocation failure
//                 return 1;
//             }

//             // Fill in the coordinates for the current point
//             for (int j = 0; j < SIZE_DATAPOINT; j++) {
//                 int randomIndex = rand() % NUM_POINTS;
//                 centers[i][j] = points[randomIndex][j]; 
//                 printf("%f\n",centers[i][j]);

//             }
//         }
//     }

    
//     result= kmeans(points, centers, clusters) ;
//     printf("K-Means result: %d\n", result);

//     //assessQualityCluster(points, NUM_POINTS, clusters, NUM_CLUSTERS);

//     // for (int i = 0; i < NUM_CLUSTERS; i++) {
//     //         totalVariation += clusters[i].variation;
//     //     }
//     //     printf("for Init Points %d, the total Variation we get is : %f\n",init,totalVariation);


// ///////////////////free points
//     for (int i = 0; i < NUM_POINTS; i++) {
//                 free(points[i]);
//     }
//     free(points);
//     for (int i = 0; i < NUM_CLUSTERS; i++) {
//                 free(centers[i]);
//     }
//     free(centers);
//     free(clusters);

//         return 0;
// }
