#!/bin/bash

# Output CSV file
output_file="results.csv"

# Header for the CSV file
echo "Input_Size,C_File,Run_Number,Time_Taken,Average_Time,Tags" > "$output_file"

# Array of C files
files=(
    "mXv_mpi_naive.c"
    "mXv_mpi_tiling.c"
    "mXv_openmp_naive.c"
    "mXv_openmp_tiling.c"
    "mXv_sequential.c"
)

# Loop over input sizes
for size in {64..16384..64}; do
    echo "Input size: $size"

    # Loop over each C file
    for file in "${files[@]}"; do
        echo "Running $file"

        # Compile the C file
        if [[ $file == *"openmp"* ]]; then
            gcc -fopenmp "$file" -o "${file%.c}"
            tags="OpenMP"
        elif [[ $file == *"mpi"* ]]; then
            mpicc "$file" -o "${file%.c}"
            tags="MPI"
        else
            gcc "$file" -o "${file%.c}"
            tags="None"
        fi

        # Run the program 10 times and calculate average time
        total_time=0
        for ((i = 1; i <= 10; i++)); do
            echo "Run $i"
            if [[ $tags == *"MPI"* ]]; then
                time_taken=$(mpirun -np 1 "./${file%.c}" $size | grep "Time taken:" | cut -d ':' -f 2)
            else
                time_taken=$(./"${file%.c}" $size | grep "Time taken:" | cut -d ':' -f 2)
            fi
            echo "Time taken: $time_taken"
            total_time=$(echo "$total_time + $time_taken" | bc)
            # Write results to CSV
            echo "$size,$file,$i,$time_taken,"$tags"" >> "$output_file"
        done
        average_time=$(echo "scale=3; $total_time / 10" | bc)
        echo "Average time: $average_time"
        echo "-------------------"
    done
done
