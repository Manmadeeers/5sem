#!/bin/bash

executable_count=0
non_executable_count=0

IFS=':' read -ra dirs <<< "$PATH"

for dir in "${dirs[@]}"; do
    if [[ -d "$dir" ]]; then
        for command in "$dir"/*; do
            if [[ -x "$command" ]]; then
                ((executable_count++))
            else
                ((non_executable_count++))
            fi
        done
    fi
done

echo "Executable commands: $executable_count"
echo "Non-executable commands: $non_executable_count"