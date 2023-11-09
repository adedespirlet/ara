#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h> // Include the header for DBL_MAX
#include <time.h>
#include "kmeans.h"
#include <string.h>




void assignPointsToClusters(const int64_t *points, const int64_t *centers, int64_t *clusters){
    printf("AssignPoints to Clusters:\n");
    size_t avl=NUM_POINTS;
    size_t vl;
    //stripmine
    asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));

    int64_t *points_ = (int64_t *)points ;
    int64_t *clusters_ = (int64_t *)clusters;

    printf("Matrix A:\n");
    for (uint64_t i = 0; i < SIZE_DATAPOINT; ++i) {
        for (uint64_t j = 0; j < NUM_POINTS; ++j) {
            printf("%ld ", points[i * NUM_POINTS + j]);
        }
    printf("\n");
    }

    ///// MAKING THREE CLUSTERS
    for (; avl > 0; avl -= vl) {
        printf("Vl value is: %ld and avl value is: %ld \n", vl,avl);
        
        double minDist = DBL_MAX;
        //set distance vector to each cluster to 0
        asm volatile("vmv.v.i v4, 0");
        asm volatile("vmv.v.i v8, 0");
        asm volatile("vmv.v.i  v12, 0");
        asm volatile("vmv.v.i v0, 0"); 
        asm volatile("vmv.v.i v16, 0"); 
        //initialize ,cluster number to 0
        //asm volatile("vmv.v.i v16, 0");
        
        asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));
        int64_t acc_vctor0[100]={0};

        int64_t acc_vctor1[100]={0};
        int64_t acc_vctor2[100]={0};
            //////////////// REPEATING THIS FOR EACH CLUSTER CENTER POINT //////////////// n*0
        for (unsigned int i=0;i<SIZE_DATAPOINT; i++){
           //we dont know what the size datapoint is so we load it repeatedly but if knows this could be done in advance before the stripming
            int64_t centers0_ = *((int64_t *)centers +i*NUM_CLUSTERS);
            int64_t centers1_ = *((int64_t *)centers +i*NUM_CLUSTERS+1);
            int64_t centers2_ = *((int64_t *)centers +i*NUM_CLUSTERS+2);

            printf("center0= %ld \n", centers0_);
            printf("center1  %ld\n",centers1_);
            printf("center2= %ld \n", centers2_);

            //LOAD first coordinate
            asm volatile("vle64.v v20,  (%0)" ::"r"(points_ )); //load datapoints to v20
            asm volatile("vmv.v.v v24, v20");
            asm volatile("vmv.v.v v28, v20");


            //Subtract the scalar value from all elements of the vector
            asm volatile("vsub.vx v20, v20, %0":: "r"(centers0_));
            asm volatile("vmul.vv v20, v20, v20");
            asm volatile("vadd.vv v4 , v4, v20");  //accumulate v4 with first coordinate 

            // asm volatile("vse64.v   v4, (%0)"::"r"(acc_vctor));  
            // asm volatile("vse64.v   v20, (%0)"::"r"(load_vctor));  
            
            // printf("distance value second datapoint%ld \n", load_vctor[1]);
            // printf("acc second datapoint%ld \n", acc_vctor[1]);
            
            //Subtract the scalar value from all elements of the vector
            asm volatile("vsub.vx v24, v24, %0":: "r"(centers1_));
            asm volatile("vmul.vv v24, v24, v24");
            asm volatile("vadd.vv v8 , v8, v24");  //accumulate v0 with first coordinate   

            //Subtract the scalar value from all elements of the vector
            asm volatile("vsub.vx v28, v28, %0":: "r"(centers2_));
            asm volatile("vmul.vv v28, v28, v28");
            asm volatile("vadd.vv v12 , v12, v28");  //accumulate v0 with first coordinate   

            points_+=NUM_POINTS;
        }
        asm volatile("vse64.v   v4, (%0)"::"r"(acc_vctor0));  
        asm volatile("vse64.v   v8, (%0)"::"r"(acc_vctor1)); 
        asm volatile("vse64.v   v12, (%0)"::"r"(acc_vctor2)); 

        printf("acc fourth datapoint %ld \n", acc_vctor0[3]);
        printf("acc fourth datapoint %ld \n", acc_vctor1[3]);
        printf("acc fourth datapoint %ld \n", acc_vctor2[3]);

        printf("acc last datapoint %ld \n", acc_vctor0[99]);
        printf("acc last datapoint %ld \n", acc_vctor1[99]);
        printf("acc last datapoint %ld \n", acc_vctor2[99]);
            
        
        //check to which cluster the data points is closest and assign cluster number accordingly

        asm volatile("vmslt.vv v0, v8, v4");    //mask vector set if elements in v8 are smaller than v4
        asm volatile("vmerge.vim v16,v16,1,v0");//set cluster number to 1 if mask is set
        asm volatile("vmerge.vvm v4, v4, v8, v0") ;//replace elements in v4 by v8 if mask is set
        asm volatile("vmslt.vv v0, v12, v4");    //mask vector set if elements in v12 are smaller than v4
        asm volatile("vmerge.vim v16,v16,2,v0");//set cluster number to 2 if mask is set
        //store resuls back
        asm volatile("vse64.v   v16, (%0)"::"r"(clusters_));  
    }


       printf("Assigned clusters are:\n");
       for (uint64_t i = 0; i < NUM_POINTS; ++i) {
            
           printf("%ld \t", clusters_[i]);
           
       }
        //fetch next group
        points_+=vl;
        //clusters_+=vl;
        
    }



void updateClusterCenters(const int64_t *points, int64_t *centers, int64_t *clusters){
    printf("update clusters centers\n");
    
    size_t vl;
    int64_t *points_ = (int64_t *)points;
    int64_t *clusters_ = (int64_t *)clusters;
    int64_t *centers_ = (int64_t *)centers;
    
    //Loop over all elements feature per feature
    for (unsigned int i=0;i<SIZE_DATAPOINT; i++){
        size_t avl=NUM_POINTS;
        points_= points+ i*NUM_POINTS; 
        
        asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));
        // int64_t mask[100]={0};
        // int64_t mask1[100]={0};
        // int64_t mask2[100]={0};
        int64_t vectorCount=0, vectorCount0=0, vectorCount1=0, vectorCount2=0;

        asm volatile("vmv.v.i v12, 0"); // Initialize group2 to zero
        asm volatile("vmv.v.i v16, 0"); // Initialize group15 to zero
        asm volatile("vmv.v.i v20, 0"); // Initialize group0 to zero
        asm volatile("vmv.v.i v24, 0"); // Initialize group0 to zero
        asm volatile("vmv.v.i v0, 0"); // Initialize group0 to zero
   
        //asm volatile("vmv.v.i v8, 0"); // Initialize group0 to zero

        
    
        for (; avl > 0; avl -= vl) {
            printf("Vl value is: %ld and avl value is: %ld \n", vl,avl);
            asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));
            
            asm volatile("vle64.v v8, (%0)" ::"r"(points_)); // Load vector cooridnate x
            asm volatile("vle64.v v4, (%0)" ::"r"(clusters_)); // Load clusters 

            //Find elements assigned to CLUSTER 0 and add their coordinates
            asm volatile("vmseq.vi v0, v4, 0");  //set mask if elements in v4 (cluster) is equal to 0
            asm volatile ("vcpop.m %0, v0" :"=r"(vectorCount));
            asm volatile("vredsum.vs v20, v8, v20, v0.t"); //accumulate in v20
            printf("vectorcount vpop0 %ld \n", vectorCount);
            vectorCount0+=vectorCount;
           // asm volatile("vse64.v   v0, (%0)"::"r"(mask));  
           // printf("cluster 0 mask %lx,%lx\n", mask[0],mask[1]);
           
            
            //Find elements assigned to CLUSTER 1 and add their coordinates
            asm volatile("vmseq.vi v0,v4,1");  //vmseq(vd vs imm vm)
            asm volatile ("vcpop.m %0, v0" :"=r"(vectorCount));
            vectorCount1+=vectorCount;
            asm volatile("vredsum.vs v16, v8, v16, v0.t"); //accumulate in v16
            printf("vectorcount vpop1 %ld \n", vectorCount);
            
            //group2
            asm volatile("vmseq.vi v0,v4,2");  //vmseq(vd vs imm vm)
            asm volatile ("vcpop.m %0, v0" :"=r"(vectorCount));
            asm volatile("vredsum.vs v12, v8, v12, v0.t"); //accumulate in v12
            printf("vectorcount vpop1 %ld \n", vectorCount);
            vectorCount2+=vectorCount;
            points_+=vl;
    
        }
       
        // int64_t acc[100]={0};
        // int64_t acc1[100]={0};
        // int64_t acc2[100]={0};

       // asm volatile("vse64.v   v20, (%0)"::"r"(acc));  
        // asm volatile("vse64.v   v16, (%0)"::"r"(acc1));  
        // asm volatile("vse64.v   v12, (%0)"::"r"(acc2));  
        // printf("accumulation cluster0 %ld\n", acc[0]);
        // printf("accumulation cluster1 %ld\n", acc1[0]);
        // printf("accumulation cluster2 %ld\n", acc2[0]);


        //divide total sum by number of elements for each cluster
        asm volatile("vdivu.vx v20, v20, %0"::"r"(vectorCount0)); 
        //asm volatile("vse64.v   v20, (%0)"::"r"(acc)); 
        //printf("division%ld\n", acc[0]);


        asm volatile("vdivu.vx v16, v16, %0":: "r"(vectorCount1)); 
        asm volatile("vdivu.vx v12, v12, %0":: "r"(vectorCount2)); 
       
        int64_t *centers1_= centers_+1; //pointer to second element of array (coordinate of second cluster)
        int64_t *centers2_=centers_+2; // pointer to third element of array (coordinate of third cluster)

        asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(1));

        // Store back each new computed cluster center
        asm volatile("vse64.v   v20, (%0)" :: "r"(centers_));  
        asm volatile("vse64.v   v16, (%0)" :: "r"(centers1_));  
        asm volatile("vse64.v   v12, (%0)" :: "r"(centers2_));
        printf("new computed center value center0 : %ld, center1: %ld, center2 : %ld \n", centers_[0],centers1_[0],centers2_[0]);

        centers_+=NUM_CLUSTERS; 

    }
}

void assessQualityCluster(const int64_t *points, int64_t *centers, int64_t *clusters)
{
    printf("Assess Quality Cluster");
    size_t avl=NUM_POINTS;
    size_t vl;
   
    asm volatile("vmv.v.i v4, 0"); // Initialize group0 to zero (accumulation group)
    asm volatile("vmv.v.i v8, 0"); // Initialize group1 to zero
    asm volatile("vmv.v.i v12, 0"); // Initialize group2 to zero

    
    int64_t *centers_ = (int64_t *)centers;
    int64_t center0,center1,center2;

    
    for (unsigned int i=0;i<SIZE_DATAPOINT; i++){
        int64_t *points_ = (int64_t *)points;
        points_ = points +i*NUM_POINTS; //times 8 or not is an element automatically assumed as 8bytes long?
        int64_t *clusters_ = (int64_t *)clusters;

        center0=*centers_;
        center1=*(centers_+1);
        center2=*(centers_+2);

        asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));

        for (; avl > 0; avl -= vl) {
            asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));
            asm volatile("vle64.v v20, (%0)" ::"r"(points_)); // Load vector cooridnate x
            asm volatile("vle64.v v16, (%0)" ::"r"(clusters_)); // Load clusters 

            //compute the variance within cluster0
            asm volatile("vmseq.vi v0, v16, 0"); // Set mask for elements equal to 0 in v16
            asm volatile("vsub.vx v20, v20, %0, v0.t":: "r"(center0));
            asm volatile("vmul.vv v20, v20, v20, v0.t");
            asm volatile("vredsum.vs v4, v20, v20, v0.t"); //accumulate

            //compute the variance within cluster1
            asm volatile("vmseq.vi v0, v16, 1"); // Set mask for elements equal to 1 in v16
            asm volatile("vsub.vx v20, v20, %0 ,v0.t":: "r"(center1));
            asm volatile("vmul.vv v20, v20, v20, v0.t");
            asm volatile("vredsum.vs v8, v20, v8, v0.t");

            //compute the variance within cluster2
            asm volatile("vmseq.vi v0, v16, 2"); // Set mask for elements equal to 2 in v16
            asm volatile("vsub.vx v20, v20, %0, v0.t":: "r"(center2));
            asm volatile("vmul.vv v20, v20, v20, v0.t");
            asm volatile("vredsum.vs v12, v20, v12, v0.t");
            points_+=vl;
            clusters_+=vl;
            }
        centers_= centers_+i*NUM_CLUSTERS;
    }
    int64_t variance0, variance1, variance2;

    asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(1));
    
    asm volatile("vse64.v v4, (%0)" :: "r"(&variance0)); // Store v8 to scalar_value
    asm volatile("vse64.v v8, (%0)" :: "r"(&variance1)); // Store v8 to scalar_value
    asm volatile("vse64.v v12, (%0)" :: "r"(&variance2)); // Store v8 to scalar_value
    printf("Variance of cluster 0 is : %ld, of cluster 1 is : %ld , of cluster 2 is : %ld \n", variance0, variance1, variance2 );
    
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


kmeans_result kmeans( const int64_t *points,  int64_t *centers,  int64_t *clusters,int64_t *clusters_last){
	int iterations = 0;
    int max_iteration=20;
    
    size_t clusters_sz = NUM_POINTS * sizeof(int64_t);
	
	while (1)
	{


        printf("Max iterations %d", max_iteration);
        printf("iteration number %d",iterations);
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
            printf("KMEANS succeeded");
			return KMEANS_OK;
		}

		if (iterations++ > max_iteration)
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

