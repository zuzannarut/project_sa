#!/bin/bash

# Check if the correct number of arguments are provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <path_to_executable> <path_to_folder>"
    exit 1
fi

# Assign arguments to variables
executable="$1"
folder="$2"

# Check if the executable file exists
if [ ! -x "$executable" ]; then
    echo "Error: Executable file not found or is not executable."
    exit 1
fi

# Check if the folder exists
if [ ! -d "$folder" ]; then
    echo "Error: Folder not found."
    exit 1
fi

# Create an output file for CPU times
output_file="cpu_times.txt"
echo "" > "$output_file"

# Loop through each file in the folder
for file in "$folder"/*; do
    # Check if the file is a regular file
    if [ -f "$file" ]; then
        # Execute the C++ executable on the file and measure CPU time
        echo "Running $executable on $file..."
        { time "$executable" "$file"; } 2>&1 | grep real | cut -d ' ' -f 2 >> "$output_file"
        echo "Finished running $executable on $file."
        sleep 15
    fi
done

echo "CPU times written to $output_file"
