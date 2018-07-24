#!/bin/bash

build_dir="./build"
project="io-cct"
resources="./res"

function build_failed
{
	echo "Make returned errors, aborting..."
	exit
}

if test -d $build_dir; then
	echo "Build directory exists..."
else
	echo "Creating build directory..."
	mkdir ./$build_dir
fi

echo "Building $project..."

echo "Running make..."
make || build_failed

echo "Moving resources and binary to $build_dir"
cp $resources/* $build_dir
mv $project $build_dir
echo "Build finished"
