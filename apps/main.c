#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h> // Include the header for DBL_MAX
#include <time.h>
#include "kmeansNEW.h"
#include <string.h>



void assignPointsToClusters(double** points, double** centers, int* clusters) {
    for (int i = 0; i < NUM_POINTS; i++) {
        double minDist = DBL_MAX;
        for (int c = 0; c < NUM_CLUSTERS; c++) {
            double sum=0;
            for (int j=0 ; j<SIZE_DATAPOINT; j++){
                sum+= pow(points[i][j] - centers[c][j], 2);

            }
            double dist= sqrt(sum);
            if (dist < minDist) {
                minDist = dist;
                clusters[i] = c;
            }
        }}
}

void updateClusterCenters(double** points, double** centers, int* clusters) {
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
                sum[cluster][d] += points[j][d];
            }
            numbersInCluster[cluster]++;
        }
    }

    for (int i = 0; i < NUM_CLUSTERS; i++) {
        if (numbersInCluster[i] > 0) {
            for (int d = 0; d < SIZE_DATAPOINT; d++) {
                centers[i][d] = sum[i][d] / numbersInCluster[i];
            }
        }
    }
}


void assessQualityCluster(double** points, double** centers, int* clusters) {
    double variation = 0.0;

    for (int i = 0; i < NUM_CLUSTERS; i++) {
        double sumOfSquaredDistances = 0.0;
        int numPointsInCluster = 0;

        for (int j = 0; j < NUM_POINTS; j++) {
            if (clusters[j] == i) {
                // Calculate the squared distance between the point and the cluster center
                double squaredDistance = 0.0;
                for (int d = 0; d < SIZE_DATAPOINT; d++) {
                    double diff = points[j][d] - centers[i][d];
                    squaredDistance += diff * diff;
                }
                sumOfSquaredDistances += squaredDistance;
                numPointsInCluster++;
            }
        }

        if (numPointsInCluster > 0) {
            // Calculate the average squared distance (variance) within the cluster
            double clusterVariance = sumOfSquaredDistances / numPointsInCluster;
            variation += clusterVariance;
            printf("Cluster %d Variance: %lf\n", i, clusterVariance);
        }
    }

    printf("Total Variation: %lf\n", variation);
}


kmeans_result kmeans( double** points, double** centers,int* clusters){
	int iterations = 0;
	int *clusters_last;
	size_t clusters_sz = NUM_POINTS;


	/* Zero out cluster numbers, just in case user forgets */
	memset(clusters, 0, clusters_sz);

	/*
	 * Previous cluster state array. At this time, r doesn't mean anything
	 * but it's ok
	 */
	clusters_last = kmeans_malloc(clusters_sz);

	while (1)
	{
		/* Store the previous state of the clustering */
		memcpy(clusters_last, clusters, clusters_sz);

#ifdef KMEANS_THREADED
		update_r_threaded(config);
		update_means_threaded(config);
#else
		assignPointsToClusters(points, centers,clusters);
		updateClusterCenters(points, centers,clusters);
        assessQualityCluster(points,centers,clusters);
       
#endif
		/*
		 * if all the cluster numbers are unchanged since last time,
		 * we are at a stable solution, so we can stop here
		 */
		if (memcmp(clusters_last, clusters, clusters_sz) == 0)
		{
			kmeans_free(clusters_last);
			//total_iterations = iterations;
			return KMEANS_OK;
		}

		if (iterations++ > MAX_ITERATIONS)
		{
			kmeans_free(clusters_last);
			//total_iterations = iterations;
			return KMEANS_EXCEEDED_MAX_ITERATIONS;
		}
	}

	kmeans_free(clusters_last);
	//total_iterations = iterations;
	return KMEANS_ERROR;
}


int main() {
    // unsigned int* total_iterations;
  
    kmeans_result result;
    
    // Allocate memory 
    double** points = (double**)malloc(NUM_POINTS * sizeof(double*));
    double** centers= (double**)malloc(NUM_CLUSTERS * sizeof(double*));
    int* clusters= (int*)malloc(NUM_POINTS* sizeof(int));
    srand(time(NULL)); // Set the seed based on the current time

    if (points) {
        // Populate the array with coordinate values
        for (int i = 0; i < NUM_POINTS; i++) {
            points[i] = (double*)malloc(SIZE_DATAPOINT * sizeof(double));

            if (!points[i]) {
                // Handle allocation failure
                return 1;
            }

            // Fill in the coordinates for the current point
            for (int j = 0; j < SIZE_DATAPOINT; j++) {
                points[i][j] = (rand() % 1000) / 10.0;  // Example: Assigning the same value for simplicity
                //printf("%f\n",points[i][j]);
            }
        }
    }
    if (centers){
    // Populate the array with coordinate values
        for (int i = 0; i < NUM_CLUSTERS; i++) {
            centers[i] = (double*)malloc(SIZE_DATAPOINT * sizeof(double));

            if (!centers[i]) {
                // Handle allocation failure
                return 1;
            }

            // Fill in the coordinates for the current point
            for (int j = 0; j < SIZE_DATAPOINT; j++) {
                int randomIndex = rand() % NUM_POINTS;
                centers[i][j] = points[randomIndex][j]; 
                printf("%f\n",centers[i][j]);

            }
        }
    }

    
    result= kmeans(points, centers, clusters) ;
    printf("K-Means result: %d\n", result);

    //assessQualityCluster(points, NUM_POINTS, clusters, NUM_CLUSTERS);

    // for (int i = 0; i < NUM_CLUSTERS; i++) {
    //         totalVariation += clusters[i].variation;
    //     }
    //     printf("for Init Points %d, the total Variation we get is : %f\n",init,totalVariation);


/////////////////////free points
    for (int i = 0; i < NUM_POINTS; i++) {
                free(points[i]);
    }
    free(points);
    for (int i = 0; i < NUM_CLUSTERS; i++) {
                free(centers[i]);
    }
    free(centers);
    free(clusters);

        return 0;
}
