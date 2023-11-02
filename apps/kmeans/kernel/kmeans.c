#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h> // Include the header for DBL_MAX
#include <time.h>
#include "kmeans.h"
#include <string.h>


void assignPointsToClusters(const int64_t *points, const int64_t *centers, int64_t *clusters){

    size_t avl=NUM_POINTS;
    size_t vl;
    //stripmine
    asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));
    int64_t counter=0;
    int64_t *points_ = (int64_t *)points ;
    int64_t r1;

// Temporary variables
  int64_t *t0, *t1, *t2, *t3;

    ///// MAKING THREE CLUSTERS
    for (; avl > 0; avl -= vl) {
        counter++;
        double minDist = DBL_MAX;
        //set distance for each datapoint to each cluster to zero v0 used as mask
        asm volatile("vmv.v.i v4, 0");
       
        asm volatile("vmv.v.i v8, 0");
        asm volatile("vmv.v.i  v12, 0");
        //initialize centers vector to zero
        asm volatile("vmv.v.i  v12, 0");
        //initialize ,cluster number to 0
        asm volatile("vmv.v.i v16, 0");
        int64_t *clusters_ = (int64_t *)clusters +(counter-1)*vl*8;
        asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));
        
            //////////////// REPEATING THIS FOR EACH CLUSTER CENTER POINT //////////////// n*0
        for (unsigned int i=0;i<SIZE_DATAPOINT; i++){
           

            points_ = points_ +i*NUM_POINTS*8;// for each coordinate go to the next row which is number of datapoint times bytes per point
            int64_t *centers_ = (int64_t *)centers +i*NUM_CLUSTERS*8;

            //LOAD first coordinate
            asm volatile("vle64.v v20,  (%0)" ::"r"(points_ )); //load datapoints to v20
            t0= centers_;
            //Subtract the scalar value from all elements of the vector
            asm volatile("vsub.vx v20, v20, %0":: "r"(t0));
            asm volatile("vmul.vv v20, v20, v20");
            asm volatile("vadd.vv v4 , v4, v20");  //accumulate v4 with first coordinate     
        }
            //take the sqrt of the accumulation vector 
        asm volatile("vfsqrt.v v4, v4");
       
        
        //////////////// REPEATING THIS FOR EACH CLUSTER CENTER POINT //////////////// n*1
        for (unsigned int i=0;i<SIZE_DATAPOINT; i++){
            int64_t *centers_ = (int64_t *)centers +i*NUM_CLUSTERS*8+1*8;
            points_ = points_ +i*NUM_POINTS*8;// for each coordinate go to the next row which is number of datapoint times bytes per point
            
            
            //LOAD first coordinate
            asm volatile("vle64.v v20,  (%0)" ::"r"(points_ )); //load datapoints to v20
            t0=centers_;
            //Subtract the scalar value from all elements of the vector
            asm volatile("vsub.vx v20, v20, %0":: "r"(t0));
            asm volatile("vmul.vv v20, v20, v20");
            asm volatile("vadd.vv v8 , v8, v20");  //accumulate v0 with first coordinate    

        }

        //take the sqrt of the accumulation vector 
        asm volatile ("vfsqrt.v v8, v8");
        asm volatile("vmslt.vv v0, v8, v4");    //mask vector set if elements in v8 are smaller than v4
        //asm volatile("vmv.v.i v16, 1"); //set cluster number to 1 if mask is set
        asm volatile("vmerge.vim v16,v16,1,v0");//set cluster number to 1 if mask is set
        asm volatile("vmerge.vvm v4, v8, v4, v0") ;//replace elements in v4 by v8 if mask is set



        //////////////// REPEATING THIS FOR EACH CLUSTER CENTER POINT //////////////// n*2
        for (unsigned int i=0;i<SIZE_DATAPOINT; i++){
            int64_t *centers_ = (int64_t *)centers +i*NUM_CLUSTERS*8+2*8;
            points_ = points_ +i*NUM_POINTS*8;// for each coordinate go to the next row which is number of datapoint times bytes per point
            
            //LOAD first coordinate
            asm volatile("vle64.v v20,  (%0)" ::"r"(points_ ));
            t0=centers_;
        
            //Subtract the scalar value from all elements of the vector
            asm volatile("vsub.vx v20, v20, %0":: "r"(t0));
            
            asm volatile("vmul.vv v20, v20, v20");
            asm volatile("vadd.vv v12 , v12, v20");  //accumulate v0 with first coordinate   
        } 

        //take the sqrt of the accumulation vector 
        asm volatile ("vfsqrt.v v12, v12");

        asm volatile("vmslt.vv v0, v12, v4");    //mask vector set if elements in v12 are smaller than v4
        asm volatile("vmerge.vim v16,v16,2,v0");//set cluster number to 2 if mask is set
        //Store clusters back 
        asm volatile("vse64.v   v16, (%0)"::"r"(clusters_));  

        }
        //fetch next group
        points_+=vl;
    
    }





void updateClusterCenters(const int64_t *points, int64_t *centers, int64_t *clusters){

    size_t avl=NUM_POINTS;
    asm volatile("vmv.v.i v22, 0"); // Initialize group0 to zero
    asm volatile("vmv.v.i v26, 0"); // Initialize group1 to zero
    asm volatile("vmv.v.i v30, 0"); // Initialize group2 to zero
    int64_t *points_ = (int64_t *)points;
    int64_t *clusters_ = (int64_t *)clusters;
    int64_t *centers_ = (int64_t *)centers;
    size_t vl;
    //stripmine
    asm volatile("vsetvli %0, %1, e64, m2, ta, ma" : "=r"(vl) : "r"(avl));

    // vector registers for accumulation: v28 and v30 v24 v26
    
    for (unsigned int i=0;i<SIZE_DATAPOINT; i++){
        points_ = points_ +i*NUM_POINTS*8;
        int64_t vectorCount=0, vectorCount0=0, vectorCount1=0, vectorCount2=0;

        for (; avl > 0; avl -= vl) {
            asm volatile("vsetvli %0, %1, e64, m2, ta, ma" : "=r"(vl) : "r"(avl));
            
            asm volatile("vle64.v v8, (%0)" ::"r"(points_)); // Load vector cooridnate x
            asm volatile("vle64.v v10, (%0)" ::"r"(clusters_)); // Load clusters 

            //group0
            //asm volatile("vseq.x v12, v10, 0"); // Mask in v12 for elements equal to 0 in v10
            asm volatile("vmseq.vi v0, v10,0");  //vmseq(vd vs imm vm)
            asm volatile ("vpopc.m %0, v0"::"r"(vectorCount));
            
            asm volatile("vredsum.vs v30, v8, v30, v0.t"); //accumulate in v30
            vectorCount0+=vectorCount;
            //group1
            //asm volatile("vseq.x v12, v10, 1"); // Mask for elements equal to 1 in v20
            asm volatile("vmseq.vi v0,v10,1");  //vmseq(vd vs imm vm)
           
            asm volatile ("vpopc.m %0, v0"::"r"(vectorCount));
            vectorCount1+=vectorCount;
            asm volatile("vredsum.vs v26, v8, v26, v0.t"); //accumulate in v26

            //group2
            asm volatile("vmseq.vi v0,v10,2");  //vmseq(vd vs imm vm)
            asm volatile ("vpopc.m %0, v0"::"r"(vectorCount));
            asm volatile("vredsum.vs v22, v8, v22,v0.t"); //accumulate in v22
            vectorCount2+=vectorCount;
            points_+=vl;

        }
        //divide total sum by number of elements for each cluster
        asm volatile("vdivu.vx v30, v30, %0"::"r"(vectorCount0)); 
        asm volatile("vdivu.vx v26, v26, %0":: "r"(vectorCount1)); 
        asm volatile("vdivu.vx v22, v22, %0":: "r"(vectorCount1)); 
       
        int64_t *centers1_= centers_+8;
        int64_t *centers2_=centers_+16;
        // Store back each new computed cluster center
        asm volatile("vse64.v   v30, (%0)" :: "r"(centers_));  
        asm volatile("vse64.v   v26, (%0)" :: "r"(centers1_));  
        asm volatile("vse64.v   v22, (%0)" :: "r"(centers2_));

        centers_= centers_+i*NUM_CLUSTERS*8;

    }
}

void assessQualityCluster(const int64_t *points, int64_t *centers, int64_t *clusters)
{
    size_t avl=NUM_POINTS;
    asm volatile("vmv.v.i v2, 0"); // Initialize group0 to zero (accumulation group)
    asm volatile("vmv.v.i v4, 0"); // Initialize group1 to zero
    asm volatile("vmv.v.i v6, 0"); // Initialize group2 to zero
    //vector v8 contains scalar element

    int64_t *points_ = (int64_t *)points;
    int64_t *clusters_ = (int64_t *)clusters;
    int64_t *centers_ = (int64_t *)centers;
    size_t vl;
    int64_t t0,t1,t2;

    asm volatile("vsetvli %0, %1, e64, m2, ta, ma" : "=r"(vl) : "r"(avl));
    for (unsigned int i=0;i<SIZE_DATAPOINT; i++){
        points_ = points_ +i*NUM_POINTS*8;
        int64_t * centers1_= centers_+8;
        int64_t *centers2_= centers_+16; 

        t0=*centers_;
        t1=*centers1_;
        t2=*centers2_;

        for (; avl > 0; avl -= vl) {
            asm volatile("vsetvli %0, %1, e64, m2, ta, ma" : "=r"(vl) : "r"(avl));
            asm volatile("vle64.v v14, (%0)" ::"r"(points_)); // Load vector cooridnate x
            asm volatile("vle64.v v16, (%0)" ::"r"(clusters_)); // Load clusters 

            //group0
            asm volatile("vmseq.vi v0, v16, 0"); // Mask in v12 for elements equal to 0 in v16
           // asm volatile("vsub.vx v14, v14, %0, v18");

            asm volatile("vsub.vx v14, v14, %0, v0.t":: "r"(t0));
            asm volatile("vmul.vv v14, v14, v14, v0.t");
            asm volatile("vredsum.vs v2, v14, v14, v0.t");

            //group1
            asm volatile("vmseq.vi v0, v16, 1"); // Mask in v12 for elements equal to 0 in v10
            //asm volatile("vsub.vm v14, v14, v10, v18");
            asm volatile("vsub.vx v14, v14, %0 ,v0.t":: "r"(t1));
            asm volatile("vmul.vv v14, v14, v14, v0.t");
            asm volatile("vredsum.vs v4, v14, v4, v0.t");

            //group2
            asm volatile("vmseq.vi v0, v16, 2"); // Mask in v12 for elements equal to 0 in v10
            //asm volatile("vsub.vm v14, v14, v12, v18");
            asm volatile("vsub.vx v14, v14, %0, v0.t":: "r"(t2));
            asm volatile("vmul.vv v14, v14, v14, v0.t");
            asm volatile("vredsum.vs v6, v14, v6, v0.t");
            points_+=vl;
            clusters_+=vl;
            }
        centers_= centers_+i*NUM_CLUSTERS*8;
    }
    int64_t variance0, variance1, variance2;
    
    asm volatile("vse64.v v2, (%0)" :: "r"(&variance0)); // Store v8 to scalar_value
    asm volatile("vse64.v v4, (%0)" :: "r"(&variance1)); // Store v8 to scalar_value
    asm volatile("vse64.v v6, (%0)" :: "r"(&variance2)); // Store v8 to scalar_value

    printf("Variance of cluster 0 is : %ld, of cluster 1 is : %ld , of cluster 2 is : %ld \n", variance0, variance1, variance2 );
    
}

void custom_memcpy(int64_t*dest, int64_t *src, size_t size){
    char *d = (char *)dest;
    const char *s = (const char *)src;
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

