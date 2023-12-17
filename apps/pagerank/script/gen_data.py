
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

from scipy.sparse import coo_matrix, csr_matrix

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

DAMPING = 0.85
CONVERGENCE = 0.001
dtype = np.float64

sparse_matrix= mmread('Harvard500.mtx')

# Convert the sparse matrix to a dense format and create an adjacency matrix
NUM_NODES = sparse_matrix.shape[0]


# for x, y in zip(sparse_matrix1.row, sparse_matrix1.col):
#     if x != y:  # Assuming no self-links
#         adj_matrix[x, y] = 1

def initAdj(row, col, num_nodes):
    adj_matrix = np.zeros((num_nodes, num_nodes))
    for x, y in zip(row, col):
        if x != y:  # Assuming no self-links
            adj_matrix[x, y] = 1
    return adj_matrix

# Function to normalize columns of the matrix
def normalize_columns(matrix):
    n = len(matrix)
    column_sums = np.sum(matrix, axis=0)
    return np.array([
        [1/n if column_sums[i] == 0 else row[i]/column_sums[i] for i in range(len(row))] 
        for row in matrix
    ])



adj_matrix = initAdj(sparse_matrix.row, sparse_matrix.col, NUM_NODES)

# Normalize columns
link_matrix = normalize_columns(adj_matrix)


# Convert list to numpy array
A = np.array(link_matrix, dtype=np.float64)
# Convert the link matrix to Compressed Sparse Row (CSR) format

csr_A = csr_matrix(A)
data_array = csr_A.data.astype(dtype)
col_array = csr_A.indices.astype(np.uint64)
row_ptr = csr_A.indptr.astype(np.uint64)



# # PR= np.zeros([NUM_NODES,1], dtype=dtype) #init pagerank vector
# # PR_new=np.zeros([NUM_NODES,1], dtype=dtype) #init pagerank vector
# # M= np.zeros([NUM_NODES,1], dtype=dtype) #init mean vector

# #############################
# ############################

# Read the sparse matrix
sparse_matrix1 = mmread('MLgraph_Ecoli.mtx')

NUM_NODES2 = sparse_matrix1.shape[0]

############################IF WEIGHTS ARE INCLUDED AND YOU WANT TO GET RID OF IT######
# Convert the graph to COO format
graph_coo = coo_matrix(sparse_matrix1)


#################################### TURNING UNDIRECTED TO DIRECTED GRAPH#####################
# Convert the undirected graph to COO format
#graph_coo = coo_matrix(sparse_matrix)

# Duplicate each edge in both directions
rows = np.hstack([graph_coo.row, graph_coo.col])
cols = np.hstack([graph_coo.col, graph_coo.row])


adj_matrix2 = initAdj(rows, cols, NUM_NODES2)

# Normalize columns
link_matrix2 = normalize_columns(adj_matrix2)

# Convert list to numpy array
A2 = np.array(link_matrix2, dtype=np.float64)
# Convert the link matrix to Compressed Sparse Row (CSR) format

csr_A2 = csr_matrix(A2)
data_array2 = csr_A2.data.astype(dtype)
col_array2 = csr_A2.indices.astype(np.uint64)
row_ptr2 = csr_A2.indptr.astype(np.uint64)



#########################GOLDEN MODEL#########################
# Initialize score and mean column
PR_ = np.ones(NUM_NODES) / NUM_NODES
mean_column_ = np.ones(NUM_NODES) / NUM_NODES

PR_2 = np.ones(NUM_NODES2) / NUM_NODES2
mean_column_2 = np.ones(NUM_NODES2) / NUM_NODES2


def csr_matrix_vector_mult(data_array, col_array, row_ptr, vector):
    num_rows = len(row_ptr) - 1
    result = np.zeros(num_rows, dtype=vector.dtype)
    for row in range(num_rows):
        start_index = row_ptr[row]
        end_index = row_ptr[row + 1]
        for i in range(start_index, end_index):
            result[row] += data_array[i] * vector[col_array[i]]
    return result

def calculate_page_rank_csr(data_array, col_array, row_ptr, PR_, mean_column_, damping=DAMPING, convergence=CONVERGENCE):
    num_pages = len(PR_)
    while True:
        PR_new_ = csr_matrix_vector_mult(data_array, col_array, row_ptr, PR_)
        PR_new_ = damping * PR_new_ + (1 - damping) * mean_column_
        if np.sum(np.abs(PR_new_ - PR_)) < convergence:
            break
        PR_ = PR_new_
    return PR_new_

# Calculate the average number of nonzeros per row
def average_nonzeros_per_row(matrix):
    nonzero_counts = np.diff(matrix.indptr)
    return np.mean(nonzero_counts)


# Compute the average number of nonzeros per row
avg_nonzeros = average_nonzeros_per_row(csr_A)


# Compute and display PageRank scores using CSR format
result = calculate_page_rank_csr(data_array, col_array, row_ptr, PR_, mean_column_)
result2 = calculate_page_rank_csr(data_array2, col_array2, row_ptr2, PR_2, mean_column_2)

PR= np.zeros([NUM_NODES,1], dtype=dtype) #init pagerank vector
PR_new=np.zeros([NUM_NODES,1], dtype=dtype) #init pagerank vector
M= np.zeros([NUM_NODES,1], dtype=dtype) #init mean vector

PR2= np.zeros([NUM_NODES2,1], dtype=dtype) #init pagerank vector
PR_new2=np.zeros([NUM_NODES2,1], dtype=dtype) #init pagerank vector
M2= np.zeros([NUM_NODES2,1], dtype=dtype) #init mean vector

# Golden result matrix
#G = np.matmul(A, B).astype(dtype)

# # Create the file
# print(".section .data,\"aw\",@progbits")
# emit("num_pages", np.array(NUM_NODES, dtype=np.uint64))
# emit("data_array", data_array, 'NR_LANES*4')
# emit("col_array", col_array, 'NR_LANES*4')
# emit("row_ptr", row_ptr, 'NR_LANES*4')
# emit("pr", PR, 'NR_LANES*4')
# emit("pr_new", PR_new, 'NR_LANES*4')
# emit("m", M, 'NR_LANES*4')
# emit("golden_o", result, 'NR_LANES*4')

# Create the file
print(".section .data,\"aw\",@progbits")
emit("num_pages", np.array(NUM_NODES2, dtype=np.uint64))
emit("avg_nonzeros", np.array(avg_nonzeros, dtype=np.uint64))
emit("data_array", data_array2, 'NR_LANES*4')
emit("col_array", col_array2, 'NR_LANES*4')
emit("row_ptr", row_ptr2, 'NR_LANES*4')
emit("pr", PR2, 'NR_LANES*4')
emit("pr_new", PR_new2, 'NR_LANES*4')
emit("m", M2, 'NR_LANES*4')
emit("golden_o", result2, 'NR_LANES*4')

