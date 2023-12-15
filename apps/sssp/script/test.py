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

def emit(name, array, alignment='8'):
  print(".global %s" % name)
  print(".balign " + alignment)
  print("%s:" % name)
  bs = array.data.tobytes()
  for i in range(0, len(bs), 4):
    s = ""
    for n in range(4):
      s += "%02x" % bs[i+3-n]
    print("    .word 0x%s" % s)

############
## SCRIPT ##
############

dtype = np.int64
source_node= 0
multiplication_factor=100000
# Read the .mtx file to get a sparse matrix
weighted_graph = mmread('HB.mtx')

##set delta, to be tuned, good estimate would be the avarge or mean of theedges weight


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
DELTA=average_weight


col_array = csr_weighted_graph.indices.astype(dtype)
row_ptr = csr_weighted_graph.indptr.astype(dtype)

##created tentaive distance array for each vertex
distances = np.zeros([1,num_nodes], dtype=dtype) # contains the assigned cluster to each data point


##allocate memory for the buckets , max amount of buckets is (max_edge_weight x number_of_vertices )/delta
max_edge_weight= np.max(data_array)
print(max_edge_weight)
number_buckets= ((max_edge_weight*num_nodes)/DELTA).astype(dtype)
B = np.zeros([1,number_buckets], dtype=np.int64)

#allocate mmeory for the linked lists, one structure takes 2x64bits (for the int vertex and for the pointer to the next vertex)
#we have 35 nodes so since each node requires 2x64bits we need an array of 70entries of 64bits
