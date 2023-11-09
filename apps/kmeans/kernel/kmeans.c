#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h> // Include the header for DBL_MAX
#include <time.h>
#include "kmeans.h"
#include <string.h>




void assignPointsToClusters(const int64_t *points,  const int64_t *centers, int64_t *clusters) {
    for (int i = 0; i < NUM_POINTS; i++) {
        double minDist = DBL_MAX;
        for (int c = 0; c < NUM_CLUSTERS; c++) {
            double sum = 0;
            for (int j = 0; j < SIZE_DATAPOINT; j++) {
                // Access the j-th coordinate of the i-th point
                double diff = (double)points[j * NUM_POINTS + i] - (double)centers[ j * NUM_CLUSTERS + c];
                sum += diff * diff;
            }
            double dist = sum;
            if (dist < minDist) {
                minDist = dist;
                clusters[i] = c;
            }
        }
    }
}


void updateClusterCenters(const int64_t *points, int64_t *centers, int64_t *clusters) {
    double sum[NUM_CLUSTERS][SIZE_DATAPOINT] = {0};
    int numbersInCluster[NUM_CLUSTERS] = {0};

    // Reset sums and counters
    for (int i = 0; i < NUM_CLUSTERS; i++) {
        for (int d = 0; d < SIZE_DATAPOINT; d++) {
            sum[i][d] = 0.0;
        }
        numbersInCluster[i] = 0;
    }

    // Accumulate sum of points for each cluster and count numbers in each cluster
    for (int j = 0; j < NUM_POINTS; j++) {
        int cluster = clusters[j];
        if (cluster >= 0 && cluster < NUM_CLUSTERS) {
            for (int d = 0; d < SIZE_DATAPOINT; d++) {
                // Corrected indexing for accessing j-th point's d-th dimension
                sum[cluster][d] += (double)points[d * NUM_POINTS + j];
            }
            numbersInCluster[cluster]++;
        }
    }

    // Calculate new cluster centers
    for (int i = 0; i < NUM_CLUSTERS; i++) {
        if (numbersInCluster[i] > 0) {
            for (int d = 0; d < SIZE_DATAPOINT; d++) {
                // Cast to int64_t if necessary, or consider changing the type of centers to double
                centers[ d * NUM_CLUSTERS + i] = (int64_t)(sum[i][d] / numbersInCluster[i]);
            }
        }
    }
}



void assessQualityCluster(const int64_t *points,  int64_t *centers, int64_t *clusters) {
    int64_t totalVariation = 0;
    int64_t clusterVariance = 0;

    // For each cluster
    for (int clusterIndex = 0; clusterIndex < NUM_CLUSTERS; clusterIndex++) {
        int64_t sumOfSquaredDistances = 0.0;
        int64_t numPointsInCluster = 0;

        // For each data point
        for (int dataIndex = 0; dataIndex < NUM_POINTS; dataIndex++) {
            // Check if the data point belongs to the current cluster
            if (clusters[dataIndex] == clusterIndex) {
                // Calculate the squared distance between the data point and the cluster center
                int64_t squaredDistance = 0;
                for (int dimension = 0; dimension < SIZE_DATAPOINT; dimension++) {
                    // Adjust indexing to match the row-wise layout of points
                    int64_t diff = points[dimension * NUM_POINTS + dataIndex] - centers[dimension * NUM_CLUSTERS + clusterIndex];
                    squaredDistance += diff * diff;
                }
                sumOfSquaredDistances += squaredDistance;
                numPointsInCluster++;
            }
        }

        // Calculate the variance for the current cluster and add it to the total variation
        if (numPointsInCluster > 0) {
            clusterVariance = sumOfSquaredDistances / numPointsInCluster;
            totalVariation += clusterVariance;
            printf("Cluster %d Variance: %lf\n", clusterIndex, clusterVariance);
        }
    }

    printf("Total Variation: %lf\n", totalVariation);
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
