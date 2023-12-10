
#include "sssp.h"
#include <stdio.h>
#include <math.h>


void addToBucket(Node *List, Node **B, int64_t vertex, int64_t bucketid, uint64_t num_nodes) {
    printf("addToBucket function\n");
        // Find the next available Node in List
        uint64_t i = 0;
        while (i < num_nodes && List[i].vertex != -1) {
            i++;
        }
        if (i == num_nodes) {
            // List is full, handle error
            printf( "Error: List is full.\n");
        }
        // Initialize the node
        List[i].vertex = vertex;
        List[i].next = B[bucketid]; // New node points to the current head of the bucket

        // Update the bucket to point to the new node
        B[bucketid] = &List[i];

        for (uint64_t i = 0; i < num_nodes; i++) {
        printf("%ld \t %ld \t ", List[i].vertex, List[i].next);
        printf("\n");
    }
}

int findSmallestNonEmptyBucket(Node **B, uint64_t num_nodes,int64_t delta) {
    printf("findSmallestNonEmptyBucket function\n");
    for (uint64_t i = 0; i < num_nodes / delta; ++i) {
        if (B[i] != NULL) {  // Check if the bucket is not empty
            return i;
        }
    }
    return -1;  // Return -1 if all buckets are empty
}

// Function to process a bucket
void processBucket(int64_t *data_array,uint64_t *col_array,uint64_t *row_ptr,Node **B, int64_t bucketIndex, uint64_t num_nodes, int64_t delta, int64_t *distances, int64_t *Req_dl,int64_t *Req_dh, int64_t *Req_vl, int64_t *Req_vh,Node *List) {
    printf("processBucket function\n");
    Node* current = B[bucketIndex];
    printf("Busy with BUcket index: %d \n",bucketIndex );
    uint64_t avl,vl;
    uint64_t limit;
    uint64_t totalLightedges=0, n=0,totalHeavyedges=0;
    uint64_t numberHeavyEdge,numberLightEdge;
   
    int64_t *Req_dl_= Req_dl;
    int64_t *Req_vl_= Req_vl;
    int64_t *Req_dh_= Req_dh;
    int64_t *Req_vh_= Req_vh;
    int64_t *data_array_= data_array;
    uint64_t *col_array_=col_array;
   
    while (B[bucketIndex] != NULL){ //check if bucket is not empty
        printf("First while loop\n");
        current = B[bucketIndex];
        limit= num_nodes*(num_nodes -1)*2;
        //empty reqL
        for (uint64_t i=0; i<limit;i++){
            Req_dl[i]=0;
            Req_vl[i]=0;
        }
        
        totalLightedges=0;
        while (current != NULL) {
            printf("Second while loop\n");
            int vertex = current->vertex;
            // Check for outgoing light edges
            // Start and end positions in CSR arrays for the current vertex
            uint64_t start_edge = row_ptr[vertex];
            uint64_t end_edge = row_ptr[vertex + 1];

            avl = end_edge-start_edge ;
            printf("NUmber of edges (avl) : %ld", avl);
            asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));
            int64_t distance= distances[vertex];
            printf("distance for vertex %ld is : %ld \n", vertex, distance);

            asm volatile("vmv.v.i v4, 0");
            asm volatile("vmv.v.i v8, 0");
            asm volatile("vmv.v.i v12, 0");
            asm volatile("vmv.v.i v16, 0");
            asm volatile("vmv.v.i v20, 0");
            for (; avl > 0; avl -= vl) {
                printf("Avl value is: %ld, Vl value is : %ld \n",avl,vl);

                asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));
                data_array_=data_array+start_edge;
                col_array_=col_array+start_edge;
                asm volatile("vle64.v v4,  (%0)" ::"r"(data_array_)); //contains weight
                asm volatile("vle64.v v8,  (%0)" ::"r"(col_array_)); 

                asm volatile("vadd.vx v12 , v4, %0":: "r"(distance));  //contains new dist
                asm volatile("vmslt.vx v0, v4, %0"::"r"(delta));

                asm volatile("vcpop.m %0, v0":"=r"(numberLightEdge)) ;
                asm volatile("vcompress.vm v16, v12, v0"); //contains new dist values for weight smaller than delta
                asm volatile("vcompress.vm v20, v8, v0"); //contains vertex values for weight smaller than delta

                asm volatile("vsetvli x0, %0, e64, m4, ta, ma" :: "r"(numberLightEdge));
                asm volatile("vse64.v v16, (%0)"::"r"(Req_dl_));
                asm volatile("vse64.v v20, (%0)" ::"r"(Req_vl_));
                Req_dl_+=numberLightEdge;
                Req_vl_+=numberLightEdge;
                totalLightedges+=numberLightEdge;

                numberHeavyEdge=avl- numberLightEdge;
                asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));
                asm volatile("vmnot.m v0, v0 ");
                asm volatile("vcompress.vm v16, v12, v0");
                asm volatile("vcompress.vm v20, v8, v0");
                printf("Number heavy Edge %ld \n", numberHeavyEdge);
                asm volatile("vsetvli x0, %0, e64, m4, ta, ma" :: "r"(numberHeavyEdge));
                asm volatile("vse64.v v16, (%0)"::"r"(Req_dh_));
                asm volatile("vse64.v v20, (%0)" ::"r"(Req_vh_));

                Req_dh_+=numberHeavyEdge;
                Req_vh_+=numberHeavyEdge;
                totalHeavyedges+=numberHeavyEdge;

                data_array_+=vl;
                col_array_+=vl;

            }
            Node* temp = current;
            current = current->next;   
        }
        // Reset only the nodes in the current bucket's chain
        current = B[bucketIndex];
        while (current != NULL) {
            current->vertex = -1;  // Reset vertex
            Node* temp = current;
            current = current->next;
            temp->next = NULL;     // Disconnect the node from the list
        }
         // Finally, set the head of the current bucket to NULL
        B[bucketIndex] = NULL;

        //relax light edges 
        printf("l value is %d \n",totalLightedges );
        if (totalLightedges>0){
            relax(Req_vl,Req_dl,delta, distances,B,List,num_nodes,totalLightedges);
        }
       

    //check the while loop if there vertexes have been placed in the current bucket otherwise exit looop
    }
    //relax heavy edges
    printf("h value is %d\n",totalHeavyedges);
    printf("printing light edges array:\n");
     for (uint64_t i=0;i<10;i++){
        printf("Req_dl is: %ld, Req_vl is : %ld \n", Req_dl[i], Req_vl[i]);
        
    }
    printf("printing heavy edges array:\n");
    for (uint64_t i=0;i<10;i++){
        printf("Req_dh is: %ld, Req_vh is : %ld \n", Req_dh[i], Req_vh[i]);
    }
    
    if (totalHeavyedges>0){
            relax(Req_vh,Req_dh,delta, distances,B,List,num_nodes,totalHeavyedges);
    }
    
    
    //empty Reqh
    for (uint64_t i=0; i<totalHeavyedges;i++){
            Req_dh[i]=0;
            Req_vh[i]=0;
    }

}

void relax(int64_t *Req_v,int64_t *Req_d,  int64_t delta,  int64_t *distances, Node **B, Node *List, uint64_t num_nodes, uint64_t totaledge) {
    printf("relax function\n");
    uint64_t numberOfupdate=0;
    uint64_t avl= totaledge;
    printf("Total edge (avl) is : %ld \n", avl);
    uint64_t vl;
    asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));
    asm volatile("vmv.v.i v28, 0");
    asm volatile("vmv.v.i v4, 0");
    asm volatile("vmv.v.i v8, 0");
    asm volatile("vmv.v.i v12, 0");
    asm volatile("vmv.v.i v16, 0");
    asm volatile("vmv.v.i v20, 0");
    asm volatile("vmv.v.i v24, 0");
      
    int64_t *distances_=distances;
    int64_t *Req_d_= Req_d;
    int64_t *Req_v_= Req_v;
    int64_t *Req_ds_= Req_d; //address to store updated elements
    int64_t *Req_vs_= Req_v;
    uint64_t totalNumberofUpdate=0;

    for (; avl > 0; avl -= vl) {
        printf("Avl value is: %ld, Vl value is : %ld \n",avl,vl);
        asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));
        
        asm volatile("vle64.v v4,  (%0)" ::"r"(Req_d_)); //contains new computed distances
        asm volatile("vle64.v v8,  (%0)" ::"r"(Req_v_)); // containes associated vertexes
        //empty or set to zero space in array Req
        asm volatile("vse64.v v28,  (%0)" ::"r"(Req_d_)); //empty location 
        asm volatile("vse64.v v28,  (%0)" ::"r"(Req_v_));
        uint64_t factor=8;
        asm volatile ("vmul.vx v20,v8,%0"::"r"(factor));
        //scatter gather to fetch current distance associated to loaded vertexes
        asm volatile("vloxei64.v v12,(%0),v20"::"r"(distances_)); //v12 contains current distance associated to each node
        
        int64_t number=-1;
        asm volatile("vmseq.vx  v0, v12, %0"::"r"(number));
        asm volatile("vmslt.vv v24, v4, v12"); // set mask if v4 < v12

        asm volatile("vor.vv v0, v0, v24"); //combine the masks

        //asm volatile("vse64.v v4, (%0), v0.t" ::"r"(distances_)); //update distance value if smaller 
        asm volatile("vsoxei64.v v4, (%0), v20,v0.t"::"r"(distances_)); //scatter gather store 
      

        //compress updates vertexs with distances in two vectors and store them in memory to update BUcket and list afterwards
        asm volatile("vcompress.vm v12, v4, v0"); //contains new distance
        asm volatile("vcompress.vm v16, v8, v0"); // contains vertex
        asm volatile ("vcpop.m %0, v0":"=r"(numberOfupdate)) ;


        asm volatile("vsetvli x0, %0, e64, m4, ta, ma" :: "r"(numberOfupdate)); //numberofupdate gives the number of elements to store back and put in Buckets
        asm volatile("vse64.v v12, (%0)"::"r"(Req_ds_));
        asm volatile("vse64.v v16, (%0)" ::"r"(Req_vs_));
        Req_ds_+= numberOfupdate;
        Req_vs_+= numberOfupdate;
        totalNumberofUpdate+=numberOfupdate; //remember total to know how many iteration in loop for updating buckets
        printf("total numver of updates: %ld \n",totalNumberofUpdate);
        Req_d_+=vl;
        Req_v_+=vl;
        distances_+=vl;
        
    }

    for (uint64_t i=0; i<totalNumberofUpdate; i++){
        int64_t new_bucket_index = floor(Req_d[i] / delta);
        printf("New bucket index is : %ld \n", new_bucket_index);
        printf("Vrtex value is %ld \n",Req_v[i]);
        addToBucket(List, B, Req_v[i],new_bucket_index,num_nodes);
    }
    printf("distances array:\n");
    for (uint64_t j = 0; j < num_nodes; ++j) {
        printf("%ld ", distances[j]);
    }
    printf("\n");
}

void sssp(int64_t *data_array,uint64_t *col_array,uint64_t *row_ptr,int64_t *distances,int64_t *B, int64_t *List, uint64_t num_nodes,int64_t delta, uint64_t source, int64_t *ReqdL, int64_t *ReqdH,int64_t *ReqvL, int64_t *ReqvH){
    printf("SSSP function\n");
    Node *list = (Node *)List;
    Node **buckets = (Node **)B;
    int64_t bucketIndex;

    //initiliaze distance matrix
    for (uint64_t i = 0; i < num_nodes; i++) {
        distances[i]=-1;
    }
    distances[source]=0;
    printf("distances array:\n");

    for (uint64_t j = 0; j < num_nodes; ++j) {
        printf("%ld ", distances[j]);
    }
    printf("\n");
    
    // Initialize buckets to NULL meaning they're empty
    for (uint64_t i = 0; i < num_nodes; i++) {
        buckets[i] = NULL;
    }

    //initialize List to empty list
    for (uint64_t i = 0; i < num_nodes; i++) {
        list[i].vertex = -1;  // Set vertex to -1
        list[i].next = NULL;  // Set next pointer to NULL
    }
    printf("List array\n");
    //initialize List to empty list
    for (uint64_t i = 0; i < num_nodes; i++) {
        printf("%ld \t %ld \t ", list[i].vertex, list[i].next);
    }
    printf("\n");
    
    //set source node into first bucket
    addToBucket(list, buckets, source,0,num_nodes);

    //start algortihm
    while (1) {
        bucketIndex = findSmallestNonEmptyBucket(B,num_nodes,delta);
        if (bucketIndex == -1) {
            // All buckets are empty, algorithm is finished
            break;
        }
        processBucket(data_array,col_array,row_ptr, buckets,  bucketIndex,  num_nodes,  delta, distances, ReqdL,ReqdH, ReqvL,ReqvH, list);
    }

}
