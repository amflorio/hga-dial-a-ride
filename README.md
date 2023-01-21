# 2-Layer Hierarchical Grouping Algorithm (2-HGA) for the Multi-Vehicle Dial-a-Ride Problem

## Description
This repository contains the source code and datasets to allow the replication of the results from the paper:

[1] **Luo, K., Florio, A.M., Das, S., Guo, X. (2023). A Hierarchical Grouping Algorithm for the Multi-Vehicle Dial-a-Ride Problem. Proceedings of the VLDB Endowment (PVLDB), 16(5): 1195-1207. doi:10.14778/3579075.3579091**

## Building and Running
For reference, we provide two `Makefile`'s: one for MacOS (`Makefile.macos`) and one for Linux (`Makefile.linux`). These should be adapted to match the specific host requirements. For licensing reasons, some dependencies cannot be included in this repository (see [Dependencies](#deps) below). The code compiles into a single executable file named `main`. The app allows several command line options:

```
$ ./main
usage: ./main <network> <# reqs> <# drivers> <Lambda>
where <network> is:
	'ny' for New York
	'sf' for San Francisco
	'sy-u' for synthetic data, uniformly distributed
	'sy-g' for synthetic data, mixed Gaussian model
```

For example, to run 2-HGA on the New York instance with 10,000 requests, 200 drivers and a van capacity of 8:

```
$ ./main ny 10000 200 8 >> ny_10000_200_8.out
```

The app outputs progress, as the different steps and iterations of the algorithm are executed. Since the app is output intensive, we recommend redirecting `stdout` to a given file, as indicated above. Note: especially in larger instances (10,000 requests or more), the algorithm may take a few to several minutes before finalizing. It is recommended to toggle heuristic mode on (see below) before running such large instances.

The final results of 2-HGA and benchmarks are printed in CSV-lines, as follows:

```
$ grep 'NY' ny_10000_200_8.out
HGA,NY,10000,200,8,1.24316e+07,81,2.47447e+06,4.58064e+07,447.53
pruneGDP,NY,10000,200,8,1.48831e+07,102,508027,1.07037e+08,18.085
FESI,NY,10000,200,8,2.28268e+07,200,147394,1.07597e+08,8.5832
```

In order, each field contains the following information: algorithm (e.g., 'HGA'), instance code (e.g., 'NY'), # of requests, # of drivers, van capacity, total travel time, # of routes in the solution, makespan, total latency, time elapsed (in seconds).

Some options are hardcoded and can be defined/tuned in the header file `Config.h`. For example, in this file one can toggle heuristic mode by setting flags `EXACT_W1` and `EXACT_MATCHING` to `false` (see the paper [1] for the meaning of those flags).

## <a name="deps"></a>Dependencies
The code is currently setup to use the minimum cost perfect matching algorithm by Vladimir Kolmogorov:

Kolmogorov, V. (2009). Blossom V: a new implementation of a minimum cost perfect matching algorithm. Mathematical Programming Computation, 1(1), 43-67.

For licensing reasons, the perfect matching algorithm implementation is *not* included in this repository. The implementation is freely available at [https://pub.ist.ac.at/~vnk/software.html](https://pub.ist.ac.at/~vnk/software.html).

## Credits
The implementation of the FESI benchmark algorithm (files `FESI.h` and `FESI.cpp`) was obtained from [github.com/BUAA-BDA/ridesharing-LMD](https://github.com/BUAA-BDA/ridesharing-LMD).

