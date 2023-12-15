
#include "sssp.h"
#include <stdio.h>
#include <math.h>


void addToBucket(Node *List, Node **B, int64_t vertex, int64_t bucketid, uint64_t num_nodes) {
   
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

}

int findSmallestNonEmptyBucket(Node **B, uint64_t num_nodes,int64_t delta) {
  
    for (uint64_t i = 0; i < num_nodes / delta; ++i) {
        if (B[i] != NULL) {  // Check if the bucket is not empty
            return i;
        }
    }
    return -1;  // Return -1 if all buckets are empty
}

// Function to process a bucket
void processBucket(int64_t *data_array,uint64_t *col_array,uint64_t *row_ptr,Node **B, int64_t bucketIndex, uint64_t num_nodes, int64_t delta, int64_t *distances, int64_t *Req_dl,int64_t *Req_dh, int64_t *Req_vl, int64_t *Req_vh,Node *List) {
   
    Node* current = B[bucketIndex];
 
    uint64_t avl,vl;
    uint64_t limit;
    uint64_t totalLightedges=0, n=0,totalHeavyedges=0,totalvl=0;
    uint64_t numberHeavyEdge,numberLightEdge;
   
    int64_t *Req_dl_= Req_dl;
    int64_t *Req_vl_= Req_vl;
    int64_t *Req_dh_= Req_dh;
    int64_t *Req_vh_= Req_vh;
    int64_t *data_array_= data_array;
    uint64_t *col_array_=col_array;
   
    while (B[bucketIndex] != NULL){ //check if bucket is not empty
       
        current = B[bucketIndex];
        limit= num_nodes*(num_nodes -1);
        //empty reqL
        for (uint64_t i=0; i<limit;i++){
            Req_dl[i]=-1;
            Req_vl[i]=-1;
        }
       
        Req_dl_=Req_dl;
        Req_vl_=Req_vl;
        
        totalLightedges=0;
        while (current != NULL) {
            
            int vertex = current->vertex;

            // Check for outgoing light edges
            // Start and end positions in CSR arrays for the current vertex
            uint64_t start_edge = row_ptr[vertex];
            uint64_t end_edge = row_ptr[vertex + 1];

            avl = end_edge-start_edge ;
            asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));
            int64_t distance= distances[vertex];
            asm volatile("vmv.v.i v4, 0");
            asm volatile("vmv.v.i v8, 0");
            asm volatile("vmv.v.i v12, 0");
            asm volatile("vmv.v.i v16, 0");
            asm volatile("vmv.v.i v20, 0");
            asm volatile("vmv.v.i v24, -1");

            for (; avl > 0; avl -= vl) {
                

                asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));
                data_array_=data_array+start_edge;
                col_array_=col_array+start_edge;
                asm volatile("vle64.v v4,  (%0)" ::"r"(data_array_)); //contains weight
                asm volatile("vle64.v v8,  (%0)" ::"r"(col_array_)); 

                asm volatile("vadd.vx v12 , v4, %0":: "r"(distance));  //contains new dist
                asm volatile("vmslt.vx v0, v4, %0"::"r"(delta));

                asm volatile("vcpop.m %0, v0":"=r"(numberLightEdge)) ;
                // asm volatile("vcompress.vm v16, v12, v0"); //contains new dist values for weight smaller than delta
                // asm volatile("vcompress.vm v20, v8, v0"); //contains vertex values for weight smaller than delta

                // asm volatile("vsetvli x0, %0, e64, m4, ta, ma" :: "r"(numberLightEdge));
                // asm volatile("vse64.v v16, (%0)"::"r"(Req_dl_));
                // asm volatile("vse64.v v20, (%0)" ::"r"(Req_vl_));

                // Req_dl_+=numberLightEdge;
                // Req_vl_+=numberLightEdge;

                asm volatile("vse64.v v12, (%0),v0.t"::"r"(Req_dl_));
                asm volatile("vse64.v v8, (%0),v0.t" ::"r"(Req_vl_));

                Req_dl_+=vl;
                Req_vl_+=vl;  
                totalLightedges+=numberLightEdge;

                numberHeavyEdge=avl- numberLightEdge;
                //asm volatile("vmnot.m v0, v0 ");
                asm volatile("vmxor.mm v0, v0, v24");

                

                // asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));
                // asm volatile("vcompress.vm v16, v12, v0");
                // asm volatile("vcompress.vm v20, v8, v0");
                // printf("Number heavy Edge %ld \n", numberHeavyEdge);
                // asm volatile("vsetvli x0, %0, e64, m4, ta, ma" :: "r"(numberHeavyEdge));
                // asm volatile("vse64.v v16, (%0)"::"r"(Req_dh_));
                // asm volatile("vse64.v v20, (%0)" ::"r"(Req_vh_));
                // Req_dh_+=numberHeavyEdge;
                // Req_vh_+=numberHeavyEdge;

                asm volatile("vse64.v v12, (%0),v0.t"::"r"(Req_dh_));
                asm volatile("vse64.v v8, (%0),v0.t" ::"r"(Req_vh_));

                totalHeavyedges+=numberHeavyEdge;
                totalvl+=vl;

                Req_dh_+=vl;
                Req_vh_+=vl;
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
        // printf("printing light edges array:\n");
        // for (uint64_t i=0;i<10;i++){
        //     printf("Req_dl is: %ld, Req_vl is : %ld \n", Req_dl[i], Req_vl[i]);
            
        // }
        printf("l value is %d \n",totalLightedges );
        if (totalLightedges>0){
            rearrangeArray(Req_vl,num_nodes);
            rearrangeArray(Req_dl,num_nodes);

            // //totalLightedges= sorting(Req_vl,Req_dl,totalLightedges);  //only if testing vector relax
            // for (uint64_t i=0;i<10;i++){
            //     printf("Req_dl is: %ld, Req_vl is : %ld \n", Req_dl[i], Req_vl[i]);
            // }

            relax_scalar(Req_vl,Req_dl,delta, distances,B,List,num_nodes,totalLightedges);
            //relax_vector(Req_vl,Req_dl,delta, distances,B,List,num_nodes,totalLightedges);
        }
       

    //check the while loop if there vertexes have been placed in the current bucket otherwise exit looop
    }
    //relax heavy edges
    
    
    // printf("printing heavy edges array:\n");
    // for (uint64_t i=0;i<10;i++){
    //     printf("Req_dh is: %ld, Req_vh is : %ld \n", Req_dh[i], Req_vh[i]);
    // }
    
    if (totalHeavyedges>0){
        rearrangeArray(Req_vh,num_nodes);
        rearrangeArray(Req_dh,num_nodes);
        //totalHeavyedges= sorting(Req_vh,Req_dh,totalHeavyedges);
        relax_scalar(Req_vh,Req_dh,delta, distances,B,List,num_nodes,totalHeavyedges);
        //relax_vector(Req_vh,Req_dh,delta, distances,B,List,num_nodes,totalHeavyedges);
    }
    
    //empty Reqh
    for (uint64_t i=0; i<totalvl;i++){
            Req_dh[i]=-1;
            Req_vh[i]=-1;
    }
}

void rearrangeArray(int64_t *arr, uint64_t num_nodes) {
    //this function makes sure elementa are next to each other in array with no -1 values in between;
    int64_t i = 0, j = 0;
    int64_t temp;
    int64_t limit = (int64_t) num_nodes * (num_nodes - 1);

    while (i < limit) {
        // Find the next non-negative value for j, starting from the current i
        j = (j < i) ? i : j;
        while (j < limit && arr[j] < 0) {
            j++;
        }

        // If i is at a negative value and j is at a non-negative value, swap them
        if (arr[i] < 0 && j < limit) {
            temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }

        i++;
    }
}


void relax_vector(int64_t *Req_v,int64_t *Req_d,  int64_t delta,  int64_t *distances, Node **B, Node *List, uint64_t num_nodes, uint64_t totaledge) {
    //receives vertexes and new potential distance to be updated if smaller than current distance and to be added to bucket accordingly

    uint64_t numberOfupdate=0;
    uint64_t avl= totaledge;

    uint64_t vl;
    asm volatile("vsetvli %0, %1, e64, m4, ta, ma" : "=r"(vl) : "r"(avl));
    asm volatile("vmv.v.i v28, -1");
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
        asm volatile ("vcpop.m %0, v0":"=r"(numberOfupdate));

        //compress updates vertexs with distances in two vectors and store them in memory to update BUcket and list afterwards
        // asm volatile("vcompress.vm v12, v4, v0"); //contains new distance
        // asm volatile("vcompress.vm v16, v8, v0"); // contains vertex
        // asm volatile("vsetvli x0, %0, e64, m4, ta, ma" :: "r"(numberOfupdate)); //numberofupdate gives the number of elements to store back and put in Buckets
        // asm volatile("vse64.v v12, (%0)"::"r"(Req_ds_));
        // asm volatile("vse64.v v16, (%0)" ::"r"(Req_vs_));
        // Req_ds_+= numberOfupdate;
        // Req_vs_+= numberOfupdate;

        asm volatile("vse64.v v4, (%0),v0.t"::"r"(Req_d_));
        asm volatile("vse64.v v8, (%0),v0.t" ::"r"(Req_v_));


        totalNumberofUpdate+=numberOfupdate; //remember total to know how many iteration in loop for updating buckets
       
        Req_d_+=vl;
        Req_v_+=vl;
        distances_+=vl;
        
    }
    rearrangeArray(Req_v,num_nodes);
    rearrangeArray(Req_d,num_nodes);
 
    for (uint64_t i=0; i<totalNumberofUpdate; i++){
        int64_t new_bucket_index = floor(Req_d[i] / delta);
        // printf("nEW BUCKET INDEX %ld\n", new_bucket_index);
        // printf("Vertex value is %ld \n",Req_v[i]);
        addToBucket(List, B, Req_v[i],new_bucket_index,num_nodes);
    }
}


void relax_scalar(int64_t *Req_v,int64_t *Req_d,  int64_t delta,  int64_t *distances, Node **B, Node *List, uint64_t num_nodes, uint64_t totaledge){
    for (int64_t i = 0; i < totaledge; i++) {
        int64_t v=Req_v[i];
        int64_t new_dist=Req_d[i];
        if (distances[v]==-1){
            int64_t new_bucket_index = floor(new_dist / delta);
            addToBucket(List, B, v,new_bucket_index,num_nodes);
            distances[v] = new_dist;
        }
        else if (new_dist < distances[v]) {
            int64_t new_bucket_index = floor(new_dist / delta);
            addToBucket(List, B, v,new_bucket_index,num_nodes);
            distances[v] = new_dist;
        }
    }
}

uint64_t sorting(int64_t *array1, int64_t *array2, uint64_t size) {
 // Bubble sort for simplicity - not efficient for large arrays
    for (size_t i = 0; i < size; i++) {
        for (size_t j = i + 1; j < size; j++) {
            if ((array1[i] > array1[j]) || (array1[i] == array1[j] && array2[i] > array2[j])) {
                // Swap array1[i] and array1[j]
                int64_t temp = array1[i];
                array1[i] = array1[j];
                array1[j] = temp;

                // Swap array2[i] and array2[j]
                temp = array2[i];
                array2[i] = array2[j];
                array2[j] = temp;
            }
        }
    }

    // Filter out duplicates
    uint64_t writeIndex = 0;
    for (size_t i = 0; i < size; i++) {
        if (array1[i] == -1) continue; // Ignore -1 values

        if (i == 0 || array1[i] != array1[writeIndex - 1]) {
            array1[writeIndex] = array1[i];
            array2[writeIndex] = array2[i];
            writeIndex++;
        }
    }

    // Mark the rest as -1
    for (size_t i = writeIndex; i < size; i++) {
        array1[i] = -1;
        array2[i] = -1;
    }

    return writeIndex;
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
    
    // Initialize buckets to NULL meaning they're empty
    for (uint64_t i = 0; i < num_nodes; i++) {
        buckets[i] = NULL;
    }

    //initialize List to empty list
    for (uint64_t i = 0; i < num_nodes; i++) {
        list[i].vertex = -1;  // Set vertex to -1
        list[i].next = NULL;  // Set next pointer to NULL
    }
    //initiliaze Req arrays
    uint64_t limit= num_nodes*(num_nodes -1);
    //empty reqL
    for (uint64_t i=0; i<limit;i++){
        ReqdL[i]=-1;
        ReqvL[i]=-1;
        ReqdH[i]=-1;
        ReqvH[i]=-1;
    }
    
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
