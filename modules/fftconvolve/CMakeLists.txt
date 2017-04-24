CMAKE_MINIMUM_REQUIRED(VERSION 3.2)
project(grimpFFTConvolve)


message(";; source directory: ${PROJECT_SOURCE_DIR}")
message(";; binary directory: ${PROJECT_BINARY_DIR}")



# Get a list of all of the source files in the directory:
file(GLOB SOURCE
    "*.h"
    "*.cpp" 
)

find_package(CUDA REQUIRED)  

find_package(OpenMP REQUIRED)
if(OPENMP_FOUND)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

INCLUDE_DIRECTORIES(/usr/local/cuda/include)

# nvcc flags                                                                                                                                                                                               
#set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS};-gencode arch=compute_20,code=sm_20)

LIST(APPEND CUDA_NVCC_FLAGS --compiler-options -fno-strict-aliasing -lineinfo -use_fast_math -Xptxas -dlcm=cg -Wno-deprecated-gpu-targets)

cuda_compile(grimpFFT_O fftmain.cu OPTIONS -G)


cuda_add_library(grimpFFTConvolve ${grimpFFT_O})
cuda_add_cufft_to_target(grimpFFTConvolve)

#target_link_libraries(grimpFFTConvolve grimpFFTConvolve_lib)



