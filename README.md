# NTHU_bike
Datastructurc final project
This project simulates a bike sharing system based on given test cases. Two versions of the simulation are available: a basic version and an advanced version. The simulation will import data, find the shortest paths, handle bikes and users, and print the results.

## Requirements
1. A compiler that supports C++ (e.g., g++).
1. The nthu_bike.h header file.

## Usage
You can run the simulation using the following command:

1. 	g++ -g -std=c++11 -o ./bin/main ./src/main.cpp
1. ./bin/main  [testcase] [version]
   
1. testcase: The name of the test case to be simulated.
1. version: Which version of the simulation to run (basic or advanced).
For example:
./bin/main  case1 basic

## Verification
After running the simulation, you can verify the output using the provided verifier:

