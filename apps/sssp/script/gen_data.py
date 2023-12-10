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

dtype = np.uint64
source_node= 4
num_nodes= 35
# Read the .mtx file to get a sparse matrix
weighted_graph = mmread('football.mtx')

# Ensure the matrix is in CSR format
csr_weighted_graph = csr_matrix(weighted_graph)

data_array = csr_weighted_graph.data.astype(dtype)
col_array = csr_weighted_graph.indices.astype(dtype)
row_ptr = csr_weighted_graph.indptr.astype(dtype)

##created tentaive distance array for each vertex
distances = np.zeros([1,num_nodes], dtype=dtype) # contains the assigned cluster to each data point

##set delta, to be tuned, good estimate would be the avarge or mean of theedges weight
DELTA=3

##allocate memory for the buckets , max amount of buckets is (max_edge_weight x number_of_vertices )/delta
max_edge_weight= np.max(data_array)
number_buckets= ((max_edge_weight*num_nodes)/DELTA).astype(dtype)

#allocate memory for an array B that contains pointers to begin of their linked list
B = np.zeros([1,number_buckets], dtype=dtype)
#allocate mmeory for the linked lists, one structure takes 2x64bits (for the int vertex and for the pointer to the next vertex)
#we have 35 nodes so since each node requires 2x64bits we need an array of 70entries of 64bits
List= np.zeros([1,70], dtype=dtype)
max_edges = num_nodes * (num_nodes - 1) #Worst-case number of light edges
ReqdL= np.zeros([1,max_edges], dtype=dtype) #allocate space for heavy requests 
ReqdH=np.zeros([1,max_edges], dtype=dtype) #allocate space for light requests
ReqvL= np.zeros([1,max_edges], dtype=dtype) #allocate space for heavy requests 
ReqvH=np.zeros([1,max_edges], dtype=dtype) #allocate space for light requests
#########################GOLDEN MODEL#########################
# Load the data from the football.mtx file
file_path = 'football.mtx'  # Adjusted file path for your dataset
data = pd.read_csv(file_path, delim_whitespace=True, header=None, names=['source', 'destination', 'weight'], comment='%')

# Create a directed graph
G_directed = nx.DiGraph()

# Add edges to the graph
for index, row in data.iterrows():
    G_directed.add_edge(row['source'], row['destination'], weight=row['weight'])

# Compute the shortest path distances from vertex 1
shortest_paths_from_1 = nx.single_source_dijkstra_path_length(G_directed, source_node+1)  ##add 1 to source node because python code counts vertexes from 1 

# Get the largest node number to define array size

max_node = max(G_directed.nodes())

# Initialize the array with -1 (indicating no path)
distances_array = np.full(max_node + 1, -1)  # Adding 1 because nodes are 1-indexed

# Update the array with distances where paths exis
for node, distance in shortest_paths_from_1.items():
    distances_array[node] = distance

# Setting the distance from vertex 1 to itself as 0
distances_array[source_node+1] = 0
result=distances_array[1:]  # Exclude the 0th index as it's not used in this graph

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


