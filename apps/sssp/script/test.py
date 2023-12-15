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
# Read the matrix from the file
weighted_graph = mmread('football.mtx')
multiplication_factor=1


dtype=np.int64
##compute delta



# Ensure the matrix is in CSR format
csr_weighted_graph = csr_matrix(weighted_graph)

data_array = csr_weighted_graph.data
print(data_array)
col_array = csr_weighted_graph.indices.astype(dtype)
row_ptr = csr_weighted_graph.indptr.astype(dtype)
data_array*=multiplication_factor
data_array=data_array.astype(dtype)

average_weight= math.floor(np.mean(data_array))

print(average_weight)
degrees = np.diff(row_ptr)

# Find the maximum degree
max_degree = np.max(degrees)
print("maxdeg")
print(max_degree)


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