#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h> // Include the header for DBL_MAX
#include <time.h>
#include "kmeans.h"
#include <string.h>


void assignPointsToClusters(const int64_t *points, const int64_t *centers, int64_t *clusters) {

    size_t avl=NUM_POINTS;
    size_t vl;

    //stripmine
    asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));
    int64_t counter=0;

    ///// MAKING THREE CLUSTERS
    for (; avl > 0; avl -= vl) {
        counter++;

        double minDist = DBL_MAX;
        //set distance for each datapoint to each cluster to zero
        asm volatile("vmv.s.x v4, zero");
        asm volatile("vmv.s.x v8, zero");
        asm volatile("vmv.s.x v12, zero");

        //initialize ,cluster number to 0
        asm volatile("vmv.s.x v16, zero");

        int64_t *clusters_ = (int64_t *)clusters +(counter-1)*vl*8;


        asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));
       
            //////////////// REPEATING THIS FOR EACH CLUSTER CENTER POINT //////////////// n*0
        for (unsigned int i=0;i<SIZE_DATAPOINT; i++){
            int64_t *points_ = (int64_t *)points +i*NUM_POINTS*8;// for each coordinate go to the next row which is number of datapoint times bytes per point
            int64_t *centers_ = (int64_t *)centers +i*NUM_CLUSTERS*8;
            //LOAD first coordinate
            asm volatile("vle64.v v20,  (%0)" ::"r"(points_ ));
            asm volatile("ld %[scalar], (%[pointer])": [scalar] "=r"(r1): [pointer] "r"(centers_); //load center coordinate to r1

            //Broadcast the scalar value to a vector register
            asm volatile("vsetvli t1, t0, e32, m1") ; //Set VL to 1 (scalar value)
            asm volatile("vmv.v.x v24, r1 ")  ;      //Move the scalar to vector register v1

            asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));

            //Subtract the scalar value from all elements of the vector
            asm volatile("vsub.vv v20, v20, v24") ; //Subtract vector v1 from vector v0 
            asm volatile("vmul.vv v20, v20, v20");
            asm volatile("vadd.vv v4 , v4, v20");  //accumulate v4 with first coordinate     

            //take the sqrt of the accumulation vector 
        asm volatile (vsqrt.vv v4, v4);
       
        
        //////////////// REPEATING THIS FOR EACH CLUSTER CENTER POINT //////////////// n*1
        for (unsigned int i=0;i<SIZE_DATAPOINT; i++){
            int64_t *centers_ = (int64_t *)centers +i*NUM_CLUSTERS*8+1*8;
            int64_t *points_ = (int64_t *)points +i*NUM_POINTS*8;// for each coordinate go to the next row which is number of datapoint times bytes per point
            
            //LOAD first coordinate
            asm volatile("vle64.v v20,  (%0)" ::"r"(points_ ));
            asm volatile("ld %[scalar], (%[pointer])": [scalar] "=r"(r1): [pointer] "r"(centers_); //load center coordinate to r1

            //Broadcast the scalar value to a vector register
            asm volatile("vsetvli t1, t0, e32, m1") ; //Set VL to 1 (scalar value)
            asm volatile("vmv.v.x v24, r1 ")  ;      //Move the scalar to vector register v1

            asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));

            //Subtract the scalar value from all elements of the vector
            asm volatile("vsub.vv v20, v20, v24") ; //Subtract vector v1 from vector v0 
            asm volatile("vmul.vv v20, v20, v20");
            asm volatile("vadd.vv v8 , v8, v20");  //accumulate v0 with first coordinate    

        //take the sqrt of the accumulation vector 
        asm volatile (vsqrt.vv v8, v8);

        asm volatile("vmslt.vv v0, v8, v4");    //mask vector set if elements in v8 are smaller than v4

        asm volatile("vmv.vi v16, 1, v0"); //set cluster number to 1 if mask is set
        asm volatile("vmerge.vvm v4, v8, v4, v0") //replace elements in v4 by v8 if mask is set



        //////////////// REPEATING THIS FOR EACH CLUSTER CENTER POINT //////////////// n*2
        for (unsigned int i=0;i<SIZE_DATAPOINT; i++){
            int64_t *centers_ = (int64_t *)centers +i*NUM_CLUSTERS*8+2*8;
            int64_t *points_ = (int64_t *)points +i*NUM_POINTS*8;// for each coordinate go to the next row which is number of datapoint times bytes per point
            
            //LOAD first coordinate
            asm volatile("vle64.v v20,  (%0)" ::"r"(points_ ));
            asm volatile("ld %[scalar], (%[pointer])": [scalar] "=r"(r1): [pointer] "r"(centers_); //load center coordinate to r1

            //Broadcast the scalar value to a vector register
            asm volatile("vsetvli t1, t0, e32, m1") ; //Set VL to 1 (scalar value)
            asm volatile("vmv.v.x v24, r1 ")  ;      //Move the scalar to vector register v1

            asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));

            //Subtract the scalar value from all elements of the vector
            asm volatile("vsub.vv v20, v20, v24") ; //Subtract vector v1 from vector v0 
            asm volatile("vmul.vv v20, v20, v20");
            asm volatile("vadd.vv v12 , v12, v20");  //accumulate v0 with first coordinate    

        //take the sqrt of the accumulation vector 
        asm volatile (vsqrt.vv v12, v12);

        asm volatile("vmslt.vv v0, v12, v4");    //mask vector set if elements in v12 are smaller than v4
        asm volatile("vmv.vi v16, 2, v0"); //set cluster number to 2 if mask is set
        
        asm volatile("vse64.v   vs3, (clusters_)");  
        //Store clusters back 

        }
    
    }



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
    double totalVariation = 0.0;
    double clusterVariance=0.0;

    // For each cluster
    for (int clusterIndex = 0; clusterIndex < NUM_CLUSTERS; clusterIndex++) {
        double sumOfSquaredDistances = 0.0;
        int numPointsInCluster = 0;

        // For each data point
        for (int dataIndex = 0; dataIndex < NUM_POINTS; dataIndex++) {
            // Check if the data point belongs to the current cluster
            if (clusters[dataIndex] == clusterIndex) {
                // Calculate the squared distance between the data point and the cluster center
                double squaredDistance = 0.0;
                for (int dimension = 0; dimension < SIZE_DATAPOINT; dimension++) {
                    double diff = points[dataIndex * SIZE_DATAPOINT + dimension] - centers[clusterIndex * SIZE_DATAPOINT + dimension];
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
