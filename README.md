# NTHU_bike
Datastructurc final project: 

This project simulates a rental analysis system for NTHU-Bike to optimize workflow management and revenue based on given test cases. Two versions of the simulation are available: a basic version and an advanced version. The simulation imports data, calculates the shortest paths using the Floyd-Warshall algorithm, handles bikes and users with a priority queue, and prints the results.

Key Features
1. User Input: Process rental requests: User_Id, Accept_Bike_Type, Start_Time, End_Time, Start_Point, End_Point.  Calculate rental fees based on distance and rental time.
2. Fee Calculation: Depreciation reduces the rental price after each use. Bikes are retired after reaching a rental count limit.
3. Free Bike Transfer Service:Allows bike transfer to designated stations without increasing the rental count.
4. Speed Assumptions: Biking and transfer speed: one unit distance per minute.


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

