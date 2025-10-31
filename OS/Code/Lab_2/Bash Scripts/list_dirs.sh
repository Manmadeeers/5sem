#!/bin/bash

convert_size() {
    local size_in_bytes=$1
    if (( size_in_bytes < 1024 )); then
        echo "${size_in_bytes} B"
    elif (( size_in_bytes < 1048576 )); then
        echo "$(( size_in_bytes / 1024 )) KB"
    elif (( size_in_bytes < 1073741824 )); then
        echo "$(( size_in_bytes / 1048576 )) MB"
    else
        echo "$(( size_in_bytes / 1073741824 )) GB"
    fi
}

list_directory() {
    local dir=$1
    echo "Contents of directory: $dir"

    for item in "$dir"/*; do
        if [ -d "$item" ]; then
            local file_count=$(find "$item" -type f | wc -l)
            echo "Directory: $(basename "$item") - Files: $file_count"
            list_directory "$item"
        elif [ -f "$item" ]; then
            local size=$(stat -c%s "$item")
            local readable_size=$(convert_size "$size")
            echo "File: $(basename "$item") - Size: $readable_size"
        fi
    done
}

if [ -z "$1" ]; then
    echo "Please specify a directory to analyze."
    exit 1
fi

list_directory "$1"