# RTOSBench

RTOSBench is a benchmark to quickly characterize the performances and cost of real time operating system services. By using an thin abstraction, it can be easily ported to any operating system.

# Porting RTOSBench to an operating system

To port the benchmark to an operating system, you must provide two files.

- config.h
- $(os)_porting_layer.c

## config.h

The config file defines the type mappings between the types used in the test scenarios and the types used in the tested operating system. This file is include by the "porting_layer.h" file, included in every test scenario.

Use the linux config file as a basis to know which types must be mapped.

## $(os)_porting_layer.c

The porting layer file provides the OS specific implementation for the function used by the test scenarios. You must provide the implementation of all the functions declared in the file "porting_layer.h".

When compiling the benchmark, you must make sure to link the OS specific porting layer file with the test scenario.

# Compiling

At the root of the project, you will find a Makefile that simplifies compiling the benchmark for different target. This is a quality of life addon to the benchmark, but any of the test scenarios could replace the "main" file in an RTOS compilation pipeline, as long as it is linked to the porting layer file during the build.

To build the linux example, type `make $(TEST_SCENARIO) EXAMPLE=linux`. The root makefile simply calls the make command at the root of the example/linux folder and injects some variable definitions. Since the compilation process varies quite a bit between RTOS, an OS specific makefile must be created. Check the branch `dev` for examples with FreeRTOS. This example is not included in the master branch, since the build process used requires additional setup for a toolchain.
