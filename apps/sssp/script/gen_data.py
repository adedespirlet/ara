#!/usr/bin/env python3
# Copyright 2022 ETH Zurich and University of Bologna.
#
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
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
DELTA=20000


col_array = csr_weighted_graph.indices.astype(dtype)
row_ptr = csr_weighted_graph.indptr.astype(dtype)

##created tentaive distance array for each vertex
distances = np.zeros([1,num_nodes], dtype=dtype) # contains the assigned cluster to each data point


##allocate memory for the buckets , max amount of buckets is (max_edge_weight x number_of_vertices )/delta
max_edge_weight= np.max(data_array)
number_buckets= ((max_edge_weight*num_nodes)/DELTA).astype(dtype)
B = np.zeros([1,number_buckets], dtype=np.int64)

#allocate mmeory for the linked lists, one structure takes 2x64bits (for the int vertex and for the pointer to the next vertex)
#we have 35 nodes so since each node requires 2x64bits we need an array of 70entries of 64bits


degrees = np.diff(row_ptr)

# Find the maximum degree
max_degree = np.max(degrees)
max_size= num_nodes*max_degree*2
List= np.zeros([1,max_size], dtype=dtype)

max_edges = num_nodes * (num_nodes - 1) #Worst-case number of light edges
ReqdL= np.zeros([1,max_edges], dtype=dtype) #allocate space for heavy requests 
ReqdH=np.zeros([1,max_edges], dtype=dtype) #allocate space for light requests
ReqvL= np.zeros([1,max_edges], dtype=dtype) #allocate space for heavy requests 
ReqvH=np.zeros([1,max_edges], dtype=dtype) #allocate space for light requests

#########################GOLDEN MODEL#########################

# Convert the matrix to a Compressed Sparse Row (CSR) format for efficient computation
csr_graph = weighted_graph.tocsr()

# Multiply the weights by the specified factor
csr_graph.data *= multiplication_factor


# Compute the single source shortest paths using Dijkstra's algorithm
distances, predecessors = csgraph.dijkstra(csr_graph, return_predecessors=True, indices=source_node)

result = np.where(np.isinf(distances), -1, distances).astype(np.int64)


# Create the file
print(".section .data,\"aw\",@progbits")
emit("num_nodes", np.array(num_nodes, dtype=np.uint64))
emit("delta", np.array(DELTA, dtype=np.uint64))
emit("source", np.array(source_node, dtype=np.uint64))
emit("data_array", data_array, 'NR_LANES*4')
emit("col_array", col_array, 'NR_LANES*4')
emit("row_ptr", row_ptr, 'NR_LANES*4')
emit("distances", distances, 'NR_LANES*4')
emit("B", B, 'NR_LANES*4')
emit("List", List, 'NR_LANES*4')
emit("ReqdH", ReqdH, 'NR_LANES*4')
emit("ReqdL", ReqdL, 'NR_LANES*4')
emit("ReqvH", ReqvH, 'NR_LANES*4')
emit("ReqvL", ReqvL, 'NR_LANES*4')
emit("golden_o", result, 'NR_LANES*4')


