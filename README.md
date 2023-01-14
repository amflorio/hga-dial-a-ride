# 2-Layer Hierarchical Grouping Algorithm (2-HGA) for the Multi-Vehicle Dial-a-Ride Problem

## Description
This repository contains the source code and datasets to allow the replication of the results from the paper:

[1] **Luo, K., Florio, A.M., Das, S., Guo, X. (2023). A Hierarchical Grouping Algorithm for the Multi-Vehicle Dial-a-Ride Problem. Proceedings of the VLDB Endowment.**

## Building and Running
For reference, we provide two `Makefile`'s: one for MacOS (`Makefile.macos`) and one for Linux (`Makefile.linux`). These should be adapted to match the specific host requirements. For licensing reasons, some dependencies cannot be included in this repository (see [Dependencies](#deps) below). The code compiles into a single executable `main`. The app allows several command line options:

```
$ ./main
usage: ./main <network> <# reqs> <# drivers> <Lambda>
where <network> is:
	'ny' for New York
	'sf' for San Francisco
	'sy-u' for synthetic data, uniformly distributed
	'sy-g' for synthetic data, mixed Gaussian model
```

For example, to run 2-HGA on the New York instance with 50,000 requests, 200 drivers and a van capacity of 8:

```
$ ./main ny 50000 200 8 >> hga2.out
```

The app outputs progress, as the different steps and iterations of the algorithm are executed. Since the app is output intensive, we recommend redirecting `stdout` to a given file, as indicated above. Note: especially in larger instances (50,000 requests and more), the algorithm may take several minutes for finalizing.

The results are printed in comma-separated lines, as follows:

```
HGA,NY,10000,200,8,1.26831e+07,87,3.60978e+06,4.62051e+07,87.979
pruneGDP,NY,10000,200,8,1.48831e+07,102,508027,1.07037e+08,18.227
FESI,NY,10000,200,8,2.29595e+07,200,145465,1.07609e+08,8.5792
```

In order, each field contains the following information: algorithm (e.g., 'HGA'), instance code (e.g., 'NY'), # of requests, # of drivers, van capacity, total travel time, # of routes in the solution, makespan, total latency, time elapsed (in seconds).

Some options are hardcoded and can be defined/tuned in the header file `Config.h`. For example, in this file one can toggle heuristic mode by setting flags `EXACT_W1` and `EXACT_MATCHING` to `false` (see the paper [1] for the meaning of those flags).

## <a name="deps"></a>Dependencies
The code is currently setup to use the minimum cost perfect matching algorithm by Vladimir Kolmogorov:

Kolmogorov, V. (2009). Blossom V: a new implementation of a minimum cost perfect matching algorithm. Mathematical Programming Computation, 1(1), 43-67.

For licensing reasons, the perfect matching algorithm implementation is *not* included in this repository. The implementation is freely available at [https://pub.ist.ac.at/~vnk/software.html](https://pub.ist.ac.at/~vnk/software.html).

## Credits
The implementation of the FESI benchmark algorithm (files `FESI.h` and `FESI.cpp`) was obtained from [github.com/BUAA-BDA/ridesharing-LMD](https://github.com/BUAA-BDA/ridesharing-LMD).

