#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <CL/cl.h>

#define GRID_SIZE (3)
#define GRID_SIZE_SQ (GRID_SIZE*GRID_SIZE)
#define MAX_SOURCE_SIZE (0x100000)
// Simple compute kernel

int main(int argc, char** argv)
{

 int err; // error code
 float data[GRID_SIZE_SQ] = {0.0f}; // original input data set to
 float data2[GRID_SIZE_SQ] = {0.0f}; // original input data set to device
 unsigned int correct; // number of correct results returned

 size_t global; // global domain size
 size_t local; // local domain size

 cl_device_id device_id; // compute device id
 cl_context context; // compute context
 cl_command_queue commands; // compute command queue
 cl_program program; // compute program
 cl_kernel kernel; // compute kernel
 cl_mem input; // device memory used for the input array
 cl_mem output; // device memory used for the output array

 //data[GRID_SIZE_SQ/2-GRID_SIZE/2-1] = 1e6f;
 //data2[GRID_SIZE_SQ/2-GRID_SIZE/2-1] = 1e6f;
 data[4] = 1e6f;
 data2[4] = 1e6f;

 for (size_t i=0;i<GRID_SIZE_SQ;i++){
    if(i%GRID_SIZE==0)
        printf("\n");
    printf("%f ",data[i]);
 }
 for (size_t i=0;i<GRID_SIZE_SQ;i++){
    if(i%GRID_SIZE==0)
        printf("\n");
    printf("%f ",data2[i]);
 }

FILE *fp;
char fileName[] = "./testcl.cl";
char *source_str;
size_t source_size;

/* Load the source code containing the kernel*/
fp = fopen(fileName, "r");
if (!fp) {
fprintf(stderr, "Failed to load kernel.\n");
exit(1);
}
source_str = (char*)malloc(MAX_SOURCE_SIZE);
source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
fclose(fp);


 // Connect to a compute device
 // If want to run your kernel on CPU then replace the parameter CL_DEVICE_TYPE_GPU
 // with CL_DEVICE_TYPE_CPU

 err = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);

 if (err != CL_SUCCESS)
 {
     printf("Error: Failed to create a device group!\n");
     return EXIT_FAILURE;
 }


 // Create a compute context
 //Contexts are responsible for managing objects such as command-queues, memory, program and kernel objects and for executing kernels on one or more devices specified in the context.

 context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);

 if (!context)
 {
     printf("Error: Failed to create a compute context!\n");
     return EXIT_FAILURE;
 }

 // Create a command commands
 commands = clCreateCommandQueue(context, device_id, 0, &err);
 if (!commands)
 {
     printf("Error: Failed to create a command commands!\n");
     return EXIT_FAILURE;
 }

 // Create the compute program from the source buffer
 program = clCreateProgramWithSource(context, 1, (const char **) &source_str, (const size_t *)&source_size, &err);
 if (!program)
 {
     printf("Error: Failed to create compute program!\n");
     return EXIT_FAILURE;
 }

 // Build the program executable
 err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
 if (err != CL_SUCCESS)
 {
    size_t len;
    char buffer[2048];
    printf("Error: Failed to build program executable!\n");
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
    printf("%s\n", buffer);
    exit(1);
 }

 // Create the compute kernel in the program we wish to run
 kernel = clCreateKernel(program, "square", &err);
 if (!kernel || err != CL_SUCCESS)
 {
    printf("Error: Failed to create compute kernel!\n");
    exit(1);
 }

 // Create the input and output arrays in device memory for our calculation
 input = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * GRID_SIZE_SQ, NULL, NULL);
 output = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * GRID_SIZE_SQ, NULL, NULL);

 if (!input || !output)
 {
    printf("Error: Failed to allocate device memory!\n");
    exit(1);
 }

 // Write our data set into the input array in device memory
 err = clEnqueueWriteBuffer(commands, input, CL_TRUE, 0, sizeof(float) * GRID_SIZE_SQ, data, 0, NULL, NULL);
 if (err != CL_SUCCESS)
 {
    printf("Error: Failed to write to source array!\n");
    exit(1);
 }

 // Set the arguments to our compute kernel
 err = 0;
 size_t grid_size= GRID_SIZE;
 err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input);
 err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &output);
 err |= clSetKernelArg(kernel, 2, sizeof(unsigned int), &grid_size);

 if (err != CL_SUCCESS)
 {
    printf("Error: Failed to set kernel arguments! %d\n", err);
    exit(1);
 }

 // Get the maximum work group size for executing the kernel on the device
 err = clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
 if (err != CL_SUCCESS)
 {
    printf("Error: Failed to retrieve kernel work group info! %d\n", err);
    exit(1);
 }

 // Execute the kernel over the entire range of our 1d input data set
 // using the maximum number of work group items for this device
 global = GRID_SIZE_SQ;
 local = global; // TODO: FIX nice local work group size
 printf("local %lu\n",local);
 size_t iter_max = 2;
 for (size_t iter = 0; iter < iter_max; iter++){
    err = clSetKernelArg(kernel, 3, sizeof(unsigned int), &iter);
    err |= clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, &local, 0, NULL, NULL);
    if (err)
    {
       printf("Error: Failed to execute kernel!\n");
       return EXIT_FAILURE;
    }
 }

 // Wait for the command commands to get serviced before reading back results
 clFinish(commands);

 // Read back the results from the device to verify the output
 err = clEnqueueReadBuffer( commands, output, CL_TRUE, 0, sizeof(float) * GRID_SIZE_SQ, data2, 0, NULL, NULL );

 err |= clEnqueueReadBuffer( commands, input, CL_TRUE, 0, sizeof(float) * GRID_SIZE_SQ, data, 0, NULL, NULL );
 if (err != CL_SUCCESS)
 {
    printf("Error: Failed to read output array! %d\n", err);
    exit(1);
 }
 for (size_t i=0;i<GRID_SIZE_SQ;i++){
    if(i%GRID_SIZE==0)
        printf("\n");
    printf("%f ",data[i]);
 }
 printf("\n");
 for (size_t i=0;i<GRID_SIZE_SQ;i++){
    if(i%GRID_SIZE==0)
        printf("\n");
    printf("%f ",data2[i]);
 }

 // Cleaning up
 clReleaseMemObject(input);
 clReleaseMemObject(output);
 clReleaseProgram(program);
 clReleaseKernel(kernel);
 clReleaseCommandQueue(commands);
 clReleaseContext(context);

 free(source_str);

 return 0;
}
