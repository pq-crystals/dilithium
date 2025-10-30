#!/bin/bash
#use chmod +x cpu_used_test.sh then ./cpu_used_test.sh

total_cpu=0
count=0

for i in {1..1000}
do
	# Extract the numeric value (removes the % sign)
	cpu=$( /usr/bin/time -v test/test_dilithium2 2>&1 | grep "Percent of CPU this job got" | grep -o ' [0-9]\+' )

	if [[ -n "$cpu" ]]; then
		total_cpu=$((total_cpu + cpu))
		count=$((count + 1))
	fi
done

if [ "$count" -gt 0 ]; then
	average_cpu=$(echo "scale=2; $total_cpu / $count" | bc)
	echo "Average CPU percent used: $average_cpu"
else
	echo "Could not extract any CPU percent values!"
fi