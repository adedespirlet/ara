def read_and_process_file(filename):
    with open(filename, 'r') as file:
        lines = file.readlines()

    all_arrays = []
    current_array = []
    processing_array = False
    line_buffer = ''  # Buffer for lines split across multiple lines

    for line in lines:
        if '---' in line:
            if processing_array and line_buffer:
                process_line(line_buffer, current_array)
                all_arrays.append(current_array)
                current_array = []
                line_buffer = ''
            processing_array = not processing_array
        elif processing_array:
            line = line.strip()
            line_buffer += line.rstrip('$')
            if not line.endswith('$'):
                process_line(line_buffer, current_array)
                line_buffer = ''

    data_array = reshape_2d_array(all_arrays[0], 3) if all_arrays else []
    cluster_arrays = all_arrays[1:] if len(all_arrays) > 1 else []

    return data_array, cluster_arrays

def process_line(line, current_array):
    if not line:
        return
    line_items = line.split(',')
    try:
        current_array.extend([int(item.strip()) for item in line_items if item.strip()])
    except ValueError as e:
        print(f"Error processing line: {line} - {e}")

def reshape_2d_array(array, rows):
    return [array[i:i + len(array) // rows] for i in range(0, len(array), len(array) // rows)]

def main():
    filename = 'data.out'
    data_array, cluster_arrays = read_and_process_file(filename)

    print("2D Data Array:")
    if not data_array:
        print("No data found in 2D Data Array")
    for row in data_array:
        print(row)

    print("\nCluster Arrays:")
    if not cluster_arrays:
        print("No cluster arrays found")
    for idx, array in enumerate(cluster_arrays):
        print(f"1D Cluster Array {idx + 1}:")
        print(array)

if __name__ == "__main__":
    main()

