#!/bin/bash

# Assign arguments to variables
input_file="$1"
output_file="$2"

# Read the input file line by line, starting from the second line
first_line=true
while IFS= read -r line; do
    if [ "$first_line" = true ]; then
        first_line=false
        continue
    fi
    echo -n "$line " >> "$output_file"
done < "$input_file"