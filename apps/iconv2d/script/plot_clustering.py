import pandas as pd
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import os

# Set the total number of iterations you have
total_iterations = 10  # Update this with your actual number of iterations

# Assign colors based on cluster number
colors = {0: 'r', 1: 'g', 2: 'b', 3: 'y'}

for iteration in range(total_iterations):
    filename = f'cluster_data_iteration_{iteration}.csv'

    # Check if the file exists
    if not os.path.exists(filename):
        print(f"File {filename} not found, skipping.")
        continue

    # Read the CSV file into a DataFrame
    df = pd.read_csv(filename)

    # Initialize a 3D plot
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    # Plot each data point with its corresponding color
    for cluster_number in df['cluster'].unique():
        cluster_data = df[df['cluster'] == cluster_number]
        ax.scatter(cluster_data['x'], cluster_data['y'], cluster_data['z'], 
                   color=colors.get(cluster_number, 'k'), label=f'Cluster {cluster_number}')

    # Add labels and title
    ax.set_xlabel('X Coordinate')
    ax.set_ylabel('Y Coordinate')
    ax.set_zlabel('Z Coordinate')
    ax.set_title(f'3D Scatter Plot of Clusters - Iteration {iteration}')

    # Add legend
    ax.legend()

    # Show plot
    plt.show()

    # Uncomment the following line if you want to pause between iterations
    # input("Press Enter to continue to the next iteration...")
