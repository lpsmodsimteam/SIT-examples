# Examples using SST Interoperability Toolkit

## Monte Carlo Integration

A simple simulation of Monte Carlo integration to estimate the value of π.

To install and run the simulation:

- Navigate to the root directory and create a build directory to store all the binaries: `mkdir .build && .build`
- Generate the boilerplate code layer using SIT: `python ../generate_boilerplate.py`
- Run CMake to build and install the library: `cmake .. && make -j`
- Run the simulation using SST: `sst ../run.py`
