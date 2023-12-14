
#include "sssp.h"
#include <stdio.h>
#include <math.h>


void addToBucket(Node *List, Node **B, int64_t vertex, int64_t bucketid, uint64_t num_nodes) {
    //printf("addToBucket function\n");
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
    //printf("findSmallestNonEmptyBucket function\n");
    for (uint64_t i = 0; i < num_nodes / delta; ++i) {
        if (B[i] != NULL) {  // Check if the bucket is not empty
            return i;
        }
    }
    return -1;  // Return -1 if all buckets are empty
}

// Function to process a bucket
void processBucket(int64_t *data_array,uint64_t *col_array,uint64_t *row_ptr,Node **B, int64_t bucketIndex, uint64_t num_nodes, int64_t delta, int64_t *distances, int64_t *Req_l, int64_t *Req_h,Node *List) {
    
    Node* current = B[bucketIndex];
    int64_t new_dist;
    uint64_t limit;
    uint64_t j;

    uint64_t l=0, n=0,h=0;
    //printf("Busy with BUcket index: %d \n",bucketIndex );
 
    while (B[bucketIndex] != NULL){ //check if bucket is not empty
        //printf("First while loop\n");
        current = B[bucketIndex];
        
        limit= num_nodes*(num_nodes -1)*2;
        //empty reqL
        for (uint64_t i; i<limit;i++){
            Req_l[i]=0;
        }
        l=0;
        while (current != NULL) {
            //printf("Second while loop\n");
            int vertex = current->vertex;
            // Check for outgoing light edges
             // Start and end positions in CSR arrays for the current vertex
            uint64_t start_edge = row_ptr[vertex];
            uint64_t end_edge = row_ptr[vertex + 1];

            // Check for outgoing edges
            for (uint64_t i = start_edge; i < end_edge; i++) {
                int64_t target_vertex = col_array[i];
                int64_t edge_weight = data_array[i];

                if (edge_weight < delta) { // Check if the edge is light
                    new_dist = distances[vertex] + edge_weight;
                    Req_l[l*2] = target_vertex;  // Store vertex
                    Req_l[l*2+1] = new_dist;  // Store new distance
                    l++;
                    //printf("L value is %d\n",l );
                }
                else if (edge_weight >= delta) {
                    new_dist = distances[vertex] + edge_weight;
                    Req_h[h*2] = target_vertex;  // Store vertex
                    Req_h[h*2 +1] = new_dist;  // Store new distance
                    h++;
                    //printf("relaxing light edge\n");
                }
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
        //printf("l value is %d \n",l );
        //printf("relaxing light edge\n");
        for (int64_t k = 0; k < l; k++) {
            //printf("relaxing light edge\n");
            relax(Req_l[k*2], Req_l[k*2 + 1],delta, distances,B,List,num_nodes);
        }
    //check the while loop if there vertexes have been placed in the current bucket otherwise exit looop
    }

    //relax heavy edges
   // printf("h value is %d\n",h);
   // printf("relaxing heavy edge\n");
    for (int64_t k = 0; k < h; k++) {
        relax(Req_h[k*2], Req_h[k*2 + 1],delta, distances,B,List,num_nodes);
    }
    //empty Reqh
     for (uint64_t i; i<n;i++){
            Req_h[i]=0;
    }

}

void relax(int64_t v, int64_t new_dist,  int64_t delta,  int64_t *distances,Node **B, Node *List, uint64_t num_nodes) {
    //printf("relax function\n");
    if (distances[v]==-1){
        int64_t new_bucket_index = floor(new_dist / delta);
        addToBucket(List, B, v,new_bucket_index,num_nodes);
        distances[v] = new_dist;
    }
    else if (new_dist < distances[v]) {
        int64_t old_bucket_index = floor(distances[v] / delta);
        int64_t new_bucket_index = floor(new_dist / delta);
        addToBucket(List, B, v,new_bucket_index,num_nodes);
        distances[v] = new_dist;
    }
}

void sssp(int64_t *data_array,uint64_t *col_array,uint64_t *row_ptr,int64_t *distances,int64_t *B, int64_t *List, uint64_t num_nodes,int64_t delta, uint64_t source, int64_t *ReqL, int64_t *ReqH){
    //printf("SSSP function\n");
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
   
    //set source node into first bucket
    addToBucket(list, buckets, source,0,num_nodes);

    //start algortihm
    while (1) {
        bucketIndex = findSmallestNonEmptyBucket(B,num_nodes,delta);
        if (bucketIndex == -1) {
            // All buckets are empty, algorithm is finished
            break;
        }
        processBucket(data_array,col_array,row_ptr, buckets,  bucketIndex,  num_nodes,  delta, distances, ReqL, ReqH,list);
    }

}
