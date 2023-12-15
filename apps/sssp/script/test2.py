import random as rand
import numpy as np
import sys
from scipy.io import mmread
from scipy.sparse import csr_matrix
import scipy.sparse.csgraph as csgraph
import math
import ctypes
import networkx as nx
import pandas as pd

dtype = np.int64
source_node= 32
multiplication_factor=1
# Read the .mtx file to get a sparse matrix
weighted_graph = mmread('HB.mtx')

##set delta, to be tuned, good estimate would be the avarge or mean of theedges weight
DELTA=3


#compute number of nodes
num_nodes= weighted_graph.shape[0]


# Ensure the matrix is in CSR format
csr_weighted_graph = csr_matrix(weighted_graph)

data_array = csr_weighted_graph.data
data_array*=multiplication_factor
data_array=data_array.astype(dtype)

##compute delta
average_weight= math.floor(np.mean(data_array))
#average_weight=average_weight.astype(np.uint64)

col_array = csr_weighted_graph.indices.astype(dtype)
row_ptr = csr_weighted_graph.indptr.astype(dtype)

##created tentaive distance array for each vertex
distances = np.zeros([1,num_nodes], dtype=dtype) # contains the assigned cluster to each data point


##allocate memory for the buckets , max amount of buckets is (max_edge_weight x number_of_vertices )/delta
max_edge_weight= np.max(data_array)
number_buckets= ((max_edge_weight*num_nodes)/DELTA).astype(dtype)
B = np.zeros([1,number_buckets], dtype=np.int64)
print("number_buckets:")
print(number_buckets)
#allocate mmeory for the linked lists, one structure takes 2x64bits (for the int vertex and for the pointer to the next vertex)
#we have 35 nodes so since each node requires 2x64bits we need an array of 70entries of 64bits


degrees = np.diff(row_ptr)

# Find the maximum degree
max_degree = np.max(degrees)
max_size= num_nodes*max_degree*2
List= np.zeros([1,max_size], dtype=dtype)

max_edges = num_nodes * (num_nodes - 1) #Worst-case number of light edges

print("max-edges")
print(max_edges)

# Convert the matrix to a Compressed Sparse Row (CSR) format for efficient computation
csr_graph = weighted_graph.tocsr()
# Multiply the weights by the specified factor
csr_graph.data *= multiplication_factor


# Choose a source node. For simplicity, we'll start with the first node (index 0)
source_node = 0

# Compute the single source shortest paths using Dijkstra's algorithm
distances, predecessors = csgraph.dijkstra(csr_graph, return_predecessors=True, indices=source_node)
result = np.where(np.isinf(distances), -1, distances).astype(dtype)


print(result)