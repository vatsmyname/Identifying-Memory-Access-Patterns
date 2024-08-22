# LLVM Analysis Pass for Identifying Memory Access Patterns

The files in this repository are used to write an LLVM analysis pass to identify memory access patterns in a C or C++ program. This was built on LLVM 20 which was installed and built using the source code from the LLVM documentations.

## Implementation:
1. Get the source code by cloning it from https://github.com/llvm/llvm-project.git
2. Change the directory into the cloned repository and create a build directory
3. Create the MemoryAccessPatterns.cpp in the following location of the cloned repository: llvm/lib/Transforms/Utils/MemoryAccessPatterns.cpp
4. Add MemoryAccessPatterns.cpp to the CMakeLists.txt in the same directory: llvm/lib/Transforms/Utils/CMakeLists.txt
5. Create the header file called MemoryAccessPatterns.h in the following location: llvm/include/llvm/Transforms/Utils/MemoryAccessPatterns.h
6. Use the PassBuilder.cpp by replacing the existing file in the location: llvm/lib/Passes/PassBuilder.cpp
7. Use the PassRegistry.def by replacing the existing file in the location: llvm/lib/Passes/PassRegistry.def
8. Finally build the system in the build directory created earlier.

## Usage:
1. The LLVM Intermediate Representation (LLVM IR) code is obtained for the sample program given by using the following clang command: \
clang -S -emit-llvm -o sample.ll sample.c
2. This creates a file consisting of the LLVM Intermediate Representation (LLVM IR) code in sample.ll
3. The pass is then performed on the sample.ll using the following command to give the final output: \
  ./build/bin/opt -passes=memoryaccesspatterns sample.ll -disable-output --analyze-memory-access-patterns

