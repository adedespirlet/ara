
import random as rand
import numpy as np
import sys
from scipy.io import mmread
from scipy.sparse import csr_matrix
import ctypes


dtype = np.int64

num_nodes= 35
# Read the .mtx file to get a sparse matrix
weighted_graph = mmread('football.mtx')
print(weighted_graph)

# Ensure the matrix is in CSR format
csr_weighted_graph = csr_matrix(weighted_graph)

data_array = csr_weighted_graph.data.astype(dtype)
col_array = csr_weighted_graph.indices.astype(dtype)
row_ptr = csr_weighted_graph.indptr.astype(dtype)

##created tentaive distance array for each vertex
distances = np.zeros([1,num_nodes], dtype=dtype) # contains the assigned cluster to each data point

##set delta, to be tuned, good estimate would be the avarge or mean of theedges weight
DELTA=2

##allocate memory for the buckets , max amount of buckets is (max_edge_weight x number_of_vertices )/delta
max_edge_weight= np.max(data_array)
number_buckets= ((max_edge_weight*num_nodes)/DELTA).astype(dtype)


####Allocate memory for the buckets ####
# Define the Node structure in Python
class Node(ctypes.Structure):
    pass

# Finish defining Node to include a pointer to Node (creating a linked list)
Node._fields_ = [("vertex", ctypes.c_int64),
                 ("next", ctypes.POINTER(Node))]

# Create an array of pointers for the buckets
# Initially, all pointers (heads of the linked lists) are None (NULL in C)
BucketArrayType = ctypes.POINTER(Node) * number_buckets
buckets = BucketArrayType()


# Allocate memory for all the nodes
nodes_array = (Node * num_nodes)()


print(buckets)
print(nodes_array)

#tranform adjancy matrix to 