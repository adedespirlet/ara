import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# Function to convert string data to list of integers
def str_to_int_list(str_data):
    return [int(item) for item in str_data.split(',') if item]

# Data points (x, y, z coordinates)
x_coordinates = str_to_int_list("38,28,14,42,7,20,38,18,22,10,10,23,35,39,23,2,21,1,23,43,29,37,1,20,32,11,21,43,24,48,26,41,27,15,14,46,43,2,36,6,20,8,38,17,3,24,13,49,8,25,1,19,27,46,6,43,7,46,34,13,16,35,49,39,3,1,5,41,3,28,17,25,43,33,9,35,13,30,47,14,7,13,22,39,20,15,44,17,46,23,25,24,44,40,28,14,44,0,24,6")  # Replace with your data
y_coordinates = str_to_int_list("42,7,20,38,18,22,10,10,23,35,39,23,2,21,1,23,43,29,37,1,20,32,11,21,43,24,48,26,41,27,15,14,46,43,2,36,6,20,8,38,17,3,24,13,49,8,25,1,19,27,46,6,43,7,46,34,13,16,35,49,39,3,1,5,41,3,28,17,25,43,33,9,35,13,30,47,14,7,13,22,39,20,15,44,17,46,23,25,24,44,40,28,14,44,0,24,6,8,23,0")   # Replace with your data
z_coordinates = str_to_int_list("38,18,22,10,10,23,35,39,23,2,21,1,23,43,29,37,1,20,32,11,21,43,24,48,26,41,27,15,14,46,43,2,36,6,20,8,38,17,3,24,13,49,8,25,1,19,27,46,6,43,7,46,34,13,16,35,49,39,3,1,5,41,3,28,17,25,43,33,9,35,13,30,47,14,7,13,22,39,20,15,44,17,46,23,25,24,44,40,28,14,44,0,24,6,8,23,0,43,7,23") # Replace with your data

# Cluster assignments and centroid positions for each iteration
# Replace with your data
cluster_iterations = [
    str_to_int_list("0,1,1,2,1,2,2,1,1,1,1,2,1,2,1,2,1,0,1,1,1,2,1,1,1,1,2,1,2,1,1,1,1,0,2,1,1,1,0,0,0,2,1,2,1,0,1,1,1,1,1,1,1,1,1,0,1,1,2,2,2,2,1,2,1,0,1,1,2,1,1,2,2,1,1,1,1,2,2,1,0,2,1,1,2,1,1,2,2,0,2,0,0,1,1,2,1,1,2,1"),
    str_to_int_list("0,2,0,2,2,2,2,0,2,1,1,2,0,2,1,2,1,0,0,0,2,2,2,1,1,1,2,1,2,1,2,1,1,0,2,1,1,1,0,0,0,2,1,2,1,0,0,1,1,1,1,1,2,1,1,0,2,1,2,2,2,2,1,2,1,0,1,0,2,1,0,2,2,0,1,1,2,2,2,1,0,2,1,1,2,0,1,2,2,0,2,0,0,1,0,2,1,1,2,1"),  # Iteration 1
    str_to_int_list("0,2,0,1,0,2,2,0,0,1,1,2,0,2,1,2,1,0,0,0,2,2,2,1,1,1,2,1,2,1,2,1,1,0,2,1,1,2,0,0,0,2,0,2,1,0,0,0,1,1,2,1,1,1,1,0,2,1,2,2,2,2,1,2,1,0,2,0,2,1,0,2,2,0,1,1,2,2,2,1,0,2,1,1,2,0,1,2,2,0,2,0,0,1,0,2,1,1,2,1"),          # Iteration 2
    str_to_int_list("0,2,0,1,2,2,2,0,0,1,1,2,0,2,1,2,1,0,0,0,0,2,2,1,1,1,2,1,2,1,2,1,1,0,2,1,1,2,0,0,0,2,0,2,1,0,0,0,1,1,2,1,1,1,1,0,2,1,2,2,2,2,1,2,1,0,2,0,2,1,0,2,2,0,1,1,2,2,2,1,0,2,1,1,2,0,1,2,2,0,2,0,0,1,0,2,1,1,2,1,"),
    str_to_int_list("0,2,0,1,2,2,2,0,0,1,1,2,0,2,1,2,1,0,0,0,0,2,2,1,1,1,2,1,2,1,2,1,1,0,2,1,1,2,0,0,0,2,0,2,1,0,0,0,1,1,2,1,1,1,1,0,2,1,2,2,2,2,1,2,1,0,2,0,2,1,0,2,2,0,1,1,2,2,2,1,0,2,1,1,2,0,1,2,2,0,2,0,0,1,0,2,1,1,2,1")
]

# Replace with your centroid data for each iteration
centroid_iterations = [
    [(21, 5, 10), (24, 24, 17), (25, 32, 40)],  # Centroids for Iteration 1
    [(23, 5, 17), (25, 30, 12), (23, 30, 38)],  # Centroids for Iteration 2
    [(24, 6, 19),(26, 33, 10),(21, 30, 38)],
    [(25, 6, 19),(26, 33, 10),(21, 29, 38)],
    [(25, 6, 19),(26, 33, 10),(21, 29, 38)]]




# Plotting function
def plot_kmeans_iteration(x, y, z, clusters, centroids, iteration):
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    
    # Plot data points
    scatter = ax.scatter(x, y, z, c=clusters, cmap='viridis', marker='o')

    # Plot centroids
    for cx, cy, cz in centroids:
        ax.scatter(cx, cy, cz, c='red', marker='x', s=100)

    ax.set_title(f'K-means Iteration {iteration}')
    ax.set_xlabel('X Coordinate')
    ax.set_ylabel('Y Coordinate')
    ax.set_zlabel('Z Coordinate')

    plt.show()

# Plot each iteration
for i, (clusters, centroids) in enumerate(zip(cluster_iterations, centroid_iterations), start=1):
    plot_kmeans_iteration(x_coordinates, y_coordinates, z_coordinates, clusters, centroids, i)
