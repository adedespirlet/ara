
#include "kmeans.h"



void assignPointsToClusters(const int64_t *points,  const int64_t *centers, int64_t *clusters,unsigned long int num_points, unsigned long int dimension) {
    //printf("Assign Points to Clusters\n");
    for (unsigned long int i = 0; i < num_points; i++) {
        int64_t minDist =INT64_MAX;
        for (int c = 0; c < NUM_CLUSTERS; c++) {
            int64_t sum = 0;
            for (uint64_t j = 0; j < dimension; j++) {
                // Access the j-th coordinate of the i-th point
                int64_t diff = points[j * num_points + i] - centers[ j * NUM_CLUSTERS + c];
                sum += diff * diff;
            }
            int64_t dist = sum;
            if (dist < minDist) {
                minDist = dist;
                clusters[i] = c;
            }
        }
    }
}


void updateClusterCenters(const int64_t *points, int64_t *centers, int64_t *clusters,unsigned long int num_points, unsigned long int dimension) {
    int64_t sum[NUM_CLUSTERS][dimension];
    int numbersInCluster[NUM_CLUSTERS];

    for (unsigned long int i = 0; i < NUM_CLUSTERS; i++) {
        for (uint64_t j = 0; j < dimension; j++) {
            sum[i][j] = 0;
        }
    }

      for (unsigned long int i = 0; i < NUM_CLUSTERS; i++) {
            numbersInCluster[i] = 0;
    }



    // Accumulate sum of points for each cluster and count numbers in each cluster
    for (unsigned long int j = 0; j < num_points; j++) {
        int cluster = clusters[j];
        if (cluster >= 0 && cluster < NUM_CLUSTERS) {
            for (unsigned long int d = 0; d < dimension; d++) {
                // Corrected indexing for accessing j-th point's d-th dimension
                sum[cluster][d] += points[d * num_points + j];
            }
            numbersInCluster[cluster]++;
        }
    }

    // Calculate new cluster centers
    for (int i = 0; i < NUM_CLUSTERS; i++) {
        
        if (numbersInCluster[i] > 0) {

            for (unsigned long int d = 0; d < dimension; d++) {
                // Cast to int64_t if necessary, or consider changing the type of centers to double
                centers[ d * NUM_CLUSTERS + i] = (int64_t)(sum[i][d] / numbersInCluster[i]);
            }
        }
    }

    //Print cluster centers
    // printf("Cluster Centers:\n");
    
    // for (unsigned long int i = 0; i < NUM_CLUSTERS; i++) {
    //     printf("(");
    //     for (unsigned long int d = 0; d < dimension; d++) {
            
    //         printf("%ld", centers[d * NUM_CLUSTERS + i]);
    //         if (d < dimension - 1) {
    //             printf(", ");
    //         }
    //     }
    //     printf(")\n");
    // }
}


void assessQualityCluster(const int64_t *points,  int64_t *centers, int64_t *clusters,unsigned long int num_points, unsigned long int dimension) {
    int64_t totalVariation = 0;
    int64_t clusterVariance = 0;

    // For each cluster
    for (int clusterIndex = 0; clusterIndex < NUM_CLUSTERS; clusterIndex++) {
        int64_t sumOfSquaredDistances = 0.0;
        int64_t numPointsInCluster = 0;

        // For each data point
        for (unsigned long int dataIndex = 0; dataIndex < num_points; dataIndex++) {
            // Check if the data point belongs to the current cluster
            if (clusters[dataIndex] == clusterIndex) {
                // Calculate the squared distance between the data point and the cluster center
                int64_t squaredDistance = 0;
                for (unsigned long int d = 0; d < dimension; d++) {
                    // Adjust indexing to match the row-wise layout of points
                    int64_t diff = points[d * num_points + dataIndex] - centers[d * NUM_CLUSTERS + clusterIndex];
                    squaredDistance += diff * diff;
                }
                sumOfSquaredDistances += squaredDistance;
                numPointsInCluster++;
            }
        }

        // Calculate the variance for the current cluster and add it to the total variation
        if (numPointsInCluster > 0) {
           // clusterVariance = sumOfSquaredDistances / numPointsInCluster;
            clusterVariance = sumOfSquaredDistances;
            totalVariation += clusterVariance;
            //printf("Cluster %d Variance: %ld\n", clusterIndex, clusterVariance);
        }
    }

    //printf("Total Variation: %ld\n", totalVariation);
}


void custom_memcpy(int64_t*dest, int64_t *src, size_t size){
   // printf("custom memcpy\n");
   
    for (size_t i = 0; i < size; i++) {
        dest[i] = src[i];
    }
}

bool custom_memcmp(const int64_t *array1, const int64_t *array2, size_t size){
    for (size_t i = 0; i < size; i++) {
        if (array1[i] != array2[i]) {
            return false;
        }
    }
    return true;
}



kmeans_result kmeans( const int64_t *points,  int64_t *centers,  int64_t *clusters,int64_t *clusters_last,unsigned long int num_points, unsigned long int dimension){
    int iterations = 0;
    int max_iteration=20;
    
    //printf("MAIN\n");
    size_t clusters_sz = num_points;
    
    while (1)
    {
        //printf("iteration number: %d \n", iterations);


        
        /* Store the previous state of the clustering */
        custom_memcpy(clusters_last, clusters, clusters_sz);


        assignPointsToClusters(points, centers,clusters,num_points,dimension);

        printf("---");
        for (uint64_t i = 0; i < num_points; ++i) {
            
            printf("%ld,", clusters[i]);
        }
        printf("---\n");
        

        // printf("Matrix clusters copied version:\n");
        // for (uint64_t i = 0; i < num_points; ++i) {
            
        //     printf("%ld ", clusters_last[i]);
        //     printf("\t");
        // }
        // printf("\n");


        updateClusterCenters(points, centers,clusters,num_points,dimension);
        assessQualityCluster(points,centers,clusters,num_points,dimension);

      
        /*
         * if all the cluster numbers are unchanged since last time,
         * we are at a stable solution, so we can stop here
         */
        if (custom_memcmp(clusters_last, clusters, clusters_sz)){
        
            //kmeans_free(clusters_last);
            //total_iterations = iterations;
            //printf("KMEANS succeeded");
            return KMEANS_OK;
        }

        if (iterations++ > max_iteration)
        {   
            //kmeans_free(clusters_last);
            //total_iterations = iterations;
            //printf("Exceeded Max Iterations");
            return KMEANS_EXCEEDED_MAX_ITERATIONS;
            
        }
    }

    //kmeans_free(clusters_last);
    //total_iterations = iterations;
    printf("ERROR");
    return KMEANS_ERROR;
}
