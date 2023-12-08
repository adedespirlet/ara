
import random as rand
import numpy as np
import sys
from scipy.io import mmread
from scipy.sparse import csr_matrix
import ctypes
import networkx as nx
import pandas as pd

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


print(len(data_array))

######GOLDEN MODEL##############


# Load the data from the football.mtx file
file_path = 'football.mtx'  # Adjusted file path for your dataset
data = pd.read_csv(file_path, delim_whitespace=True, header=None, names=['source', 'destination', 'weight'], comment='%')

# Create a directed graph
G_directed = nx.DiGraph()

# Add edges to the graph
for index, row in data.iterrows():
    G_directed.add_edge(row['source'], row['destination'], weight=row['weight'])

# Compute the shortest path distances from vertex 1
shortest_paths_from_1 = nx.single_source_dijkstra_path_length(G_directed, 1)

# Display the results
for node, distance in shortest_paths_from_1.items():
    print(f"Vertex 1 to Vertex {node}: {distance}")

shortest_paths_2d_array = [[vertex, distance] for vertex, distance in shortest_paths_from_1.items()]
print(shortest_paths_2d_array)

# Get the largest node number to define array size

max_node = max(G_directed.nodes())

# Initialize the array with -1 (indicating no path)
distances_array = np.full(max_node + 1, -1)  # Adding 1 because nodes are 1-indexed

# Update the array with distances where paths exis
for node, distance in shortest_paths_from_1.items():
    distances_array[node] = distance

# Setting the distance from vertex 1 to itself as 0
distances_array[1] = 0
distances_array=distances_array[1:]  # Exclude the 0th index as it's not used in this graph

print(distances_array)