cmake_minimum_required (VERSION 3.2) 

set (CUDA_PATH /usr/local/cuda/)

add_subdirectory(dfm)
add_subdirectory(fftconvolve)
add_subdirectory(marchingCubes)
