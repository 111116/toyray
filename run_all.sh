#!/usr/bin/env bash

# If project not ready, generate cmake file.
if [[ ! -d build ]]; then
    mkdir -p build
    cd build
    cmake ..
    cd ..
fi

# Build project.
cd build
make -j
cd ..
mkdir bin
cp build/toyray bin/PA1

# Run all testcases. 
mkdir -p output
mkdir -p hdroutput
for ((i=1; i<=7; ++i))
do
	>&2 echo -e "\033[0;32mTestcase $i running...\033[0m"
	if bin/PA1 testcases/$i.json --quiet
	then
		# >&2 echo -e "\033[0;32mTestcase $i successfully exited.\033[0m"
		mv $i.exr hdroutput/
		mv $i.bmp output/
	else
		>&2 echo -e "\033[1;33mTestcase $i failed.\033[0m"
	fi
	>&2 echo
done
