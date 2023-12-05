// Assuming a predefined maximum number of vertices
#define MAX_VERTICES 1000
#define INFINITY 1000000
#define DELTA 10  // Example delta value

// Tentative distances array
int tent[MAX_VERTICES];

// 2D array to store edge weights
int C[MAX_VERTICES][MAX_VERTICES];

// Bucket structure - a simple array of lists
// Each list node represents a vertex
typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

Node* B[MAX_VERTICES / DELTA];  // Array of pointers to bucket lists
Node* R = NULL; // Set R to store vertices after processing light edges

void relax(int v, int new_dist) {
    if (new_dist < tent[v]) {
        int old_bucket_index = floor(tent[v] / DELTA);
        int new_bucket_index = floor(new_dist / DELTA);

        if (isInBucket(old_bucket_index, v)) {
            removeFromBucket(old_bucket_index, v);
        }

        addToBucket(new_bucket_index, v);
        tent[v] = new_dist;
    }
}

// Function to check if a vertex is in a bucket
int isInBucket(int bucketIndex, int vertex) {
    Node* current = B[bucketIndex];
    while (current != NULL) {
        if (current->vertex == vertex) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

// Function to remove a vertex from a bucket
void removeFromBucket(int bucketIndex, int vertex) {
    Node* current = B[bucketIndex];
    Node* prev = NULL;
    while (current != NULL) {
        if (current->vertex == vertex) {
            if (prev == NULL) {
                B[bucketIndex] = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

// Function to add a vertex to a bucket
void addToBucket(int bucketIndex, int vertex) {
    Node* newNode = createNode(vertex);
    newNode->next = B[bucketIndex];
    B[bucketIndex] = newNode;
}

// Function to create a new list node
Node* createNode(int v) {
    Node* newNode = (Node*) malloc(sizeof(Node));
    if (!newNode) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    newNode->vertex = v;
    newNode->next = NULL;
    return newNode;
}

void addToR(int vertex) {
    Node* newNode = (Node*) malloc(sizeof(Node));
    newNode->vertex = vertex;
    newNode->next = R;
    R = newNode;
}

####################################################### MAINLOOOP####
int findSmallestNonEmptyBucket() {
    for (int i = 0; i < MAX_VERTICES / DELTA; ++i) {
        if (B[i] != NULL) {  // Check if the bucket is not empty
            return i;
        }
    }
    return -1;  // Return -1 if all buckets are empty
}
// Function to process a bucket
void processBucket(int bucketIndex) {
    Node* current = B[bucketIndex];
    while (current != NULL) {
        int vertex = current->vertex;


       //check for outgoing edges that have a weight smaller than delta and relax 
        for (int i = 0; i < MAX_VERTICES; i++) {
            if (C[vertex][i] != INFINITY && C[vertex][i] < DELTA) { // Check if the edge is light
                int new_dist = tent[vertex] + C[vertex][i];
                relax(i, new_dist);
            }
        }

        addToR(vertex);
        temp = current;
        current = current->next;
        free(temp); // Free the node after moving vertex to R
    }

    B[bucketIndex] = NULL; // Empty the bucket
}

int sssp(){

	while (1) {
        int bucketIndex = findSmallestNonEmptyBucket();
        if (bucketIndex == -1) {
            // All buckets are empty, algorithm is finished
            break;
        }

        processBucket(bucketIndex);
    }

}