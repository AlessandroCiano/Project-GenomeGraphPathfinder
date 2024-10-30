# Graph Pathfinding and Cycle Detection in Genome Assemblies

## Overview
This project implements a graph-based algorithm for pathfinding and cycle detection in genome assembly graphs. It reads nodes and edges from a `.gfa` file and provides functions to identify and remove cycles, determine source and destination nodes, and print specified paths. The project supports complex graph structures with node orientations, adjacency lists, and back-edge management.

## Key Features
- **Graph Construction**: Parses `.gfa` files to build node sequences and directed edges.
- **Cycle Detection**: Detects and removes cycles for a directed acyclic graph (DAG) structure.
- **Pathfinding**: Finds paths from source to destination nodes, including reverse complements for DNA sequences.

## Usage
1. Load node and edge data using `readNodes()` and `readEdges()`.
2. Use `cycle_d()` to detect cycles, followed by `remove_back_edges()` to make the graph acyclic.
3. Define source and destination nodes and print paths using `printxPaths()`.

## Requirements
- C++11 or newer

