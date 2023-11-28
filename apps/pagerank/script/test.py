import numpy as np
import sys
from scipy.io import mmread
from scipy.sparse import csr_matrix

DAMPING = 0.85
CONVERGENCE = 1e-6
dtype = np.float64

# Read the sparse matrix
sparse_matrix = mmread('Harvard500.mtx')

# Convert the sparse matrix to a dense format and create an adjacency matrix
NUM_NODES = 500  # Assuming 500 nodes
adj_matrix = np.zeros((NUM_NODES, NUM_NODES))

for x, y in zip(sparse_matrix.row, sparse_matrix.col):
    if x != y:  # Assuming no self-links
        adj_matrix[x, y] = 1

# Function to normalize columns of the matrix
def normalize_columns(matrix):
    n = len(matrix)
    column_sums = np.sum(matrix, axis=0)
    return np.array([
        [1/n if column_sums[i] == 0 else row[i]/column_sums[i] for i in range(len(row))] 
        for row in matrix
    ])
np.set_printoptions(threshold=sys.maxsize)
# Create the link matrix and normalize it
link_matrix = normalize_columns(adj_matrix)
# Convert list to numpy array
A = np.array(link_matrix, dtype=np.float64)
# Convert the link matrix to Compressed Sparse Row (CSR) format
csr_A = csr_matrix(A)
data_array = csr_A.data.astype(dtype)
col_array = csr_A.indices.astype(np.uint64)
row_ptr = csr_A.indptr.astype(np.uint64)


PR= np.zeros([NUM_NODES,1], dtype=dtype) #init pagerank vector
PR_new=np.zeros([NUM_NODES,1], dtype=dtype) #init pagerank vector
M= np.zeros([NUM_NODES,1], dtype=dtype) #init mean vector


#########################GOLDEN MODEL#########################
# Initialize score and mean column
PR_ = np.ones(NUM_NODES) / NUM_NODES
mean_column_ = np.ones(NUM_NODES) / NUM_NODES

# Calculate PageRank
def calculate_page_rank(A, PR_, mean_column_, damping=DAMPING, convergence=CONVERGENCE):
    num_pages = len(PR_)
    while True:
        PR_new_ = np.dot(A, PR_)
        PR_new_ = damping * PR_new_ + (1 - damping) * mean_column_
        if np.sum(np.abs(PR_new_ - PR_)) < convergence:
            break
        PR_ = PR_new_
    return PR_new_

# Compute and display PageRank scores
result = calculate_page_rank(A, PR_, mean_column_)

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


# Print the CSR arrays
#print("Data Array (Non-zero elements):")
#print(data_array)

print("\nColumn Indices Array:")
print(col_array)

# print("\nRow Pointer Array:")
# print(row_ptr)

print("Size of data_array:", data_array.size)
print("Size of col_array:", col_array.size)
print("Size of row_ptr:", row_ptr.size)

print(result)
print(result_csr)