import numpy as np
import sys
from scipy.io import mmread
from scipy.sparse import csr_matrix, coo_matrix
# Probability of an edge existing between two nodes
p = 0.4

DAMPING = 0.85
CONVERGENCE =0.001

dtype = np.float64
# Read the sparse matrix


sparse_matrix= mmread('jazz.mtx')

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
print(sparse_matrix.row)
print(sparse_matrix.data)
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

# Compute and display PageRank scores using CSR format
result_csr = calculate_page_rank_csr(data_array, col_array, row_ptr, PR_, mean_column_)
result_csr2 = calculate_page_rank_csr(data_array2, col_array2, row_ptr2, PR_2, mean_column_2)

# Calculate the average number of nonzeros per row
def average_nonzeros_per_row(matrix):
    nonzero_counts = np.diff(matrix.indptr)
    return np.mean(nonzero_counts)


# Compute the average number of nonzeros per row
avg_nonzeros = average_nonzeros_per_row(csr_A)
print("Average nonzeros per row:", avg_nonzeros)

# Assuming csr_A is your CSR matrix
total_nonzeros = len(data_array)
print("Total number of nonzeros in the CSR matrix:", total_nonzeros)

# Print the CSR arrays
print("Data Array (Non-zero elements):")
print(data_array)

print("\nColumn Indices Array:")
print(col_array)

# print("\nRow Pointer Array:")
# print(row_ptr)

print("Size of data_array:", data_array.size)
print("Size of col_array:", col_array.size)
print("Size of row_ptr:", row_ptr.size)



print(result_csr)
