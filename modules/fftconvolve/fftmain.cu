#include <omp.h>
#include <stdio.h>      // stdio functions are used since C++ streams aren't necessarily thread safe
#include <cufft.h> 
 
#define NX 1024
#define NY 1024
#define NZ 1024
 
using namespace std;

typedef unsigned char byte;


int fftmain()
{
        int num_gpus = 0;       // number of CUDA GPUs
 
	printf(" using CUDA and OpenMP \n");
        /////////////////////////////////////////////////////////////////
        // determine the number of CUDA capable GPUs
        //
    	cudaGetDeviceCount(&num_gpus);
        if(num_gpus < 1)
        {
                printf("no CUDA capable devices were detected\n");
                return 1;
        }
 
        /////////////////////////////////////////////////////////////////
        // display CPU and GPU configuration
        //
    printf("number of host CPUs:\t%d\n", omp_get_num_procs());
    printf("number of CUDA devices:\t%d\n", num_gpus);
    for(int i = 0; i < num_gpus; i++)
    {
        cudaDeviceProp dprop;
        cudaGetDeviceProperties(&dprop, i);
                printf("   %d: %s\n", i, dprop.name);
    }
        printf("---------------------------\n");
 
 
     
 
    ////////////////////////////////////////////////////////////////
        // run as many CPU threads as there are CUDA devices
        //   each CPU thread controls a different device, processing its
        //   portion of the data.  It's possible to use more CPU threads
        //   than there are CUDA devices, in which case several CPU
        //   threads will be allocating resources and launching kernels
        //   on the same device.  For example, try omp_set_num_threads(2*num_gpus);
        //   Recall that all variables declared inside an "omp parallel" scope are
        //   local to each CPU thread
        //
        omp_set_num_threads(num_gpus);  // create as many CPU threads as there are CUDA devices
      //omp_set_num_threads(2*num_gpus);// create twice as many CPU threads as there are CUDA devices
#pragma omp parallel
    {
        unsigned int cpu_thread_id = omp_get_thread_num();
        unsigned int num_cpu_threads = omp_get_num_threads();
 
                // set and check the CUDA device for this CPU thread
                int gpu_id = -1;
                cudaSetDevice(cpu_thread_id % num_gpus);        // "% num_gpus" allows more CPU threads than GPU devices
                cudaGetDevice(&gpu_id);
 
                printf("CPU thread %d (of %d) uses CUDA device %d\n", cpu_thread_id, num_cpu_threads, gpu_id);

		// do the fft
		cufftHandle plan;
		cufftComplex *data1, *data2;
		cudaMalloc((void**)&data1, sizeof( cufftComplex)*NX*NY*NZ);
		cudaMalloc((void**)&data2, sizeof( cufftComplex)*NX*NY*NZ);
		/* Create a 3D FFT plan. */
		cufftPlan3d(&plan, NX, NY, NZ, CUFFT_C2C);

		/* Transform the first signal in place. */
		cufftExecC2C(plan, data1, data1, CUFFT_FORWARD);

		/* Transform the second signal using the same plan. */
		cufftExecC2C(plan, data2, data2, CUFFT_FORWARD);

		/* Destroy the cuFFT plan. */
		cufftDestroy(plan);
		cudaFree(data1); cudaFree(data2);
  		
    }
        printf("---------------------------\n");
 
        if(cudaSuccess != cudaGetLastError())
                printf("%s\n", cudaGetErrorString(cudaGetLastError()));
 
    cudaThreadExit();
 
    return 0;
}



