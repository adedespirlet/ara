

#include <stdlib.h>
#include <math.h>
#include <float.h> // Include the header for DBL_MAX
#include <time.h>
#include "kmeans.h"
#include <string.h>

void assignPointsToClusters(const int64_t *points, const int64_t *centers, int64_t *clusters,unsigned long int num_points, unsigned long int dimension, unsigned long int num_cluster){
    // printf("AssignPoints to Clusters:\n");
    size_t avl=num_points;
    size_t vl;
    //stripmine
    asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));

    int64_t *points_ = (int64_t *)points ;
    int64_t *clusters_ = (int64_t *)clusters;
    

    ///// MAKING THREE CLUSTERS
    for (; avl > 0; avl -= vl) {
        // printf("Vl value is: %ld and avl value is: %ld \n", vl,avl);
        
        asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));

        //setp1. compute distance^2 to cluster0 center -> v4, and prepare array for cluster assignment -> v8
        asm volatile("vmv.v.i v4, 0");
        asm volatile("vmv.v.i v8, 0");
        for (unsigned int i=0;i<dimension; i++){
            int64_t centers0_ = *((int64_t *)centers +i*num_cluster);
            asm volatile("vle64.v v20,  (%0)" ::"r"(points_ + i*num_points));
            asm volatile("vsub.vx v20, v20, %0":: "r"(centers0_));
            asm volatile("vmacc.vv v4, v20, v20");
        }

        //iterate every cluster center
        for (unsigned int c=1;c<num_cluster; c++){
            //step2. calculate distance to cluster c center -> v12
            asm volatile("vmv.v.i v12, 0");
            for (unsigned int i=0;i<dimension; i++){
                int64_t centersC_ = *((int64_t *)centers +i*num_cluster+ c);
                asm volatile("vle64.v v20,  (%0)" ::"r"(points_ + i*num_points));
                asm volatile("vsub.vx v20, v20, %0":: "r"(centersC_));
                asm volatile("vmacc.vv v12, v20, v20");
            }
            //step3. Compare v4 and v12 and set mask if v12 < v4
            asm volatile("vmslt.vv v0, v12, v4");

            //step4. masked update distance array v4
            asm volatile("vmerge.vvm v4, v4, v12, v0");

            //step5. masked update cluster assign array v8
            asm volatile("vmerge.vxm v8,v8,%0,v0":: "r"(c));
        }

        //step6. store back cluster assignment
        asm volatile("vse64.v   v8, (%0)"::"r"(clusters_));

        //step7. update pointer of points and cluster
        points_+=vl;
        clusters_+=vl;  
    }
}  

void updateClusterCentersVector(const int64_t *points, int64_t *centers, int64_t *clusters,unsigned long int num_points, unsigned long int dimension, unsigned long int num_cluster){
    // printf("update clusters centers\n");
    
    size_t vl;
    int64_t *points_ = (int64_t *)points;
    int64_t *clusters_ = (int64_t *)clusters;
    int64_t *centers_ = (int64_t *)centers;
    
    //Loop over all elements to accumulate centers
    for (unsigned int i=0;i<dimension; i++){
        size_t avl=num_points;
        points_= points+ i*num_points; 
        centers_= centers + i*num_cluster; 
        
        asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));

        for (int64_t c=0;c<num_cluster; c++){
            avl=num_points;
            asm volatile("vmv.v.i v12, 0");
            asm volatile("vmv.v.i v0, 0");
            for (; avl > 0; avl -= vl){
                asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));
                asm volatile("vle64.v v8, (%0)" ::"r"(points_)); // Load vector cooridnate x
                asm volatile("vle64.v v4, (%0)" ::"r"(clusters_)); // Load clusters 
                asm volatile("vmseq.vx v0, v4, %0" :: "r"(c));
                asm volatile("vredsum.vs v12, v8, v12, v0.t");
                points_+=vl;
                clusters_+=vl;
            }

            points_= points+ i*num_points;
            clusters_ = (int64_t *)clusters;

            asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(1));

            int64_t *centers_C= centers_+c;

            asm volatile("vse64.v   v12, (%0)" :: "r"(centers_C));
            
        }

    }

    //count numerb of points per cluster
    int64_t cnt_array[num_cluster];
    for (int64_t i=0;i<(num_cluster); i++){
        cnt_array[i] = 0;
    }
    for (int64_t i = 0; i < num_points; i++)
    {
        cnt_array[clusters_[i]] += 1;
    }
    centers_ = (int64_t *)centers;
    for (int64_t i=0;i<(dimension * num_cluster); i++){
        centers_[i] = centers_[i]/cnt_array[i%num_cluster];
    }
    
    
}

        

void custom_memcpy(int64_t*dest, int64_t *src, size_t size){
    int64_t *d = (int64_t *)dest;
    const int64_t *s = (const int64_t *)src;
    for (size_t i = 0; i < size; i++) {
        d[i] = s[i];
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



kmeans_result kmeans( const int64_t *points,  int64_t *centers,  int64_t *clusters,int64_t *clusters_last,unsigned long int num_points, unsigned long int dimension, unsigned long int num_cluster){
    int iterations = 0;
    int max_iteration=20;
    
    printf("MAIN\n");
    size_t clusters_sz = num_points;
    
    while (1)
    {

        // printf("iteration number %d \n",iterations);
        /* Store the previous state of the clustering */
        custom_memcpy(clusters_last, clusters, clusters_sz);

        assignPointsToClusters(points, centers,clusters,num_points,dimension,num_cluster);
        updateClusterCenters(points, centers,clusters,num_points,dimension,num_cluster);
        /*
         * if all the cluster numbers are unchanged since last time,
         * we are at a stable solution, so we can stop here
         */
        if (custom_memcmp(clusters_last, clusters, clusters_sz)){
        
            //kmeans_free(clusters_last);
            //total_iterations = iterations;
            printf("KMEANS succeeded");
            return KMEANS_OK;
        }

        if (iterations++ >= max_iteration)
        {   
            //kmeans_free(clusters_last);
            //total_iterations = iterations;
            printf("Exceeded Max Iterations");
            return KMEANS_EXCEEDED_MAX_ITERATIONS;
            
        }
    }

    //kmeans_free(clusters_last);
    //total_iterations = iterations;
    printf("ERROR");
    return KMEANS_ERROR;
}

