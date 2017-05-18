#include <fcntl.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef __APPLE__
  #include <OpenCL/opencl.h>
#else
  #include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)

int main(int argc, char** argv)
{
  uint32_t ROWS = 10; // TODO get this from the user
  uint32_t COLS = 3; // TODO get this from the user
  float initValue = 1e10f;
  float diffConstant = 0.02f;
  size_t noOfIterations = 10;

  char arg[10];
  if (argc > 6)
    printf("Too many arguments\n");

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      memset(arg, 0, sizeof(arg));
      memcpy(arg, &argv[i][2], strlen(argv[i]) - 2);
      if (argv[i][1] == 'i') {
        initValue = (float) strtof(arg, NULL);
      } else if (argv[i][1] == 'd') {
        diffConstant = (float) strtof(arg, NULL);
      } else if (argv[i][1] == 'n') {
        noOfIterations = (size_t) strtol(arg, NULL, 10);
      } else {
        printf("Unknown argument %s\n", argv[i]);
      }
    } else {
      // Digit => degrees
      if (i == 1)
        COLS = (int) strtol(argv[i], NULL, 10);
      else if (i == 2)
        ROWS = (int) strtol(argv[i], NULL, 10);
      else
        printf("Too many integer arguments\n");
    }
  }

  // printf("Arguments\nWidth: %d, Height: %d\ninitValue: %f, diffConstant: %f, noOfIterations: %lu\n", COLS, ROWS, initValue, diffConstant, noOfIterations);

  uint32_t PADDED_ROWS = (ROWS + 2);
  uint32_t PADDED_COLS = (COLS + 2);
  uint64_t GRID_SIZE = (ROWS * COLS);
  uint64_t GRID_SIZE_PADDED = (PADDED_ROWS * PADDED_COLS);

  int err; // error code

  float *data = (float *) calloc(sizeof(float), GRID_SIZE_PADDED * 2);

  size_t global; // global domain size
  size_t local; // local domain size
  size_t transformedId;
  float mean = 0.0f;
  float std = 0.0f;

  cl_device_id device_id; // compute device id
  cl_context context; // compute context
  cl_command_queue commands; // compute command queue
  cl_program program; // compute program
  cl_kernel heat_diff_kernel, sum_kernel, std_kernel; // compute kernels
  cl_mem input; // device memory used for the input array

  int innerIndex = ( GRID_SIZE % 2 == 0 ) ? GRID_SIZE / 2 - COLS / 2 - 1 : GRID_SIZE / 2;
  int transformedInnerIndex = innerIndex + (PADDED_COLS + 1) + 2 * (innerIndex / (PADDED_COLS - 2));

  data[transformedInnerIndex] = initValue;



  /*for (size_t i=0;i<GRID_SIZE_PADDED;i++){
    if(i % PADDED_COLS == 0)
      printf("\n");
    printf("%f ", data[i]);
  }*/

  printf("\n");

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
  heat_diff_kernel = clCreateKernel(program, "heat_diff", &err);
  sum_kernel = clCreateKernel(program, "sum", &err);
  std_kernel = clCreateKernel(program, "std", &err);
  if (!heat_diff_kernel || !sum_kernel || !std_kernel || err != CL_SUCCESS)
  {
    printf("Error: Failed to create compute kernels!\n");
    exit(1);
  }

  // Create the input and output arrays in device memory for our calculation
  input = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * GRID_SIZE_PADDED * 2, NULL, NULL);

  if (!input)
  {
    printf("Error: Failed to allocate device memory!\n");
    exit(1);
  }

  // Write our data set into the input array in device memory
  err = clEnqueueWriteBuffer(commands, input, CL_TRUE, 0, sizeof(float) * GRID_SIZE_PADDED * 2, data, 0, NULL, NULL);
  if (err != CL_SUCCESS)
  {
    printf("Error: Failed to write to source array!\n");
    exit(1);
  }

  // Set the arguments to our compute kernel
  err = 0;
  err = clSetKernelArg(heat_diff_kernel, 0, sizeof(cl_mem), &input);
  err |= clSetKernelArg(heat_diff_kernel, 1, sizeof(uint32_t), &PADDED_ROWS);
  err |= clSetKernelArg(heat_diff_kernel, 2, sizeof(uint32_t), &PADDED_COLS);
  err |= clSetKernelArg(heat_diff_kernel, 3, sizeof(float), &diffConstant);

  if (err != CL_SUCCESS)
  {
    printf("Error: Failed to set kernel arguments! %d\n", err);
    exit(1);
  }

  // Get the maximum work group size for executing the kernel on the device
  err = clGetKernelWorkGroupInfo(heat_diff_kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
  if (err != CL_SUCCESS)
  {
    printf("Error: Failed to retrieve kernel work group info! %d\n", err);
    exit(1);
  }

  // Execute the kernel over the entire range of our 1d input data set
  // using the maximum number of work group items for this device
  global = GRID_SIZE;

  for (size_t iter = 0; iter < noOfIterations; iter++){
    err = clSetKernelArg(heat_diff_kernel, 4, sizeof(unsigned int), &iter);
    err |= clEnqueueNDRangeKernel(commands, heat_diff_kernel, 1, NULL, &global, NULL, 0, NULL, NULL); // TODO Optimize local (5th arg)

    if (err)
    {
      printf("Error: Failed to execute kernel!\n");
      return EXIT_FAILURE;
    }
  }
  while (global % local !=0){
    local = local >> 1;
  }
  // Time For mean calculation
  size_t n_partial_sums = global / local;
  if (global % local != 0){
    printf("%s\n", "Watch out partial sums not feeling good.");
    exit(1);
  }
  float * partial_sums = (float *) malloc(sizeof(float)*n_partial_sums);
  float * partial_stds = (float *) malloc(sizeof(float)*n_partial_sums);

  cl_mem cl_partial_sums = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*n_partial_sums, NULL, &err);
  cl_mem cl_partial_stds = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*n_partial_sums, NULL, &err);

  err = 0;
  err = clSetKernelArg(sum_kernel, 0, sizeof(cl_mem), &input);
  err |= clSetKernelArg(sum_kernel, 1, sizeof(cl_mem), &cl_partial_sums);
  err |= clSetKernelArg(sum_kernel, 2, sizeof(float) * local, NULL);
  err |= clSetKernelArg(sum_kernel, 3, sizeof(uint32_t), &PADDED_COLS);

  if (err != CL_SUCCESS)
  {
    printf("Error: Failed to set mean kernel arguments! %d\n", err);
    exit(1);
  }

  err |= clEnqueueNDRangeKernel(commands, sum_kernel, 1, NULL, &global, &local, 0, NULL, NULL); // TODO Optimize local (5th arg)
  clFinish(commands);

  err = clEnqueueReadBuffer(commands, cl_partial_sums, CL_TRUE, 0, sizeof(float)*n_partial_sums, partial_sums, 0, NULL, NULL);
  err |= clEnqueueReadBuffer(commands, input, CL_TRUE, 0, sizeof(float) * GRID_SIZE_PADDED * 2, data, 0, NULL, NULL );

  if (err != CL_SUCCESS)
  {
    printf("Error: Failed to read output array! %d\n", err);
    exit(1);
  }

  mean = 0.0f;
  for(size_t i = 0; i < n_partial_sums; i++) {
    mean += partial_sums[i];
  }
  mean /= n_partial_sums;
  printf("%s %.5e\n","average:", mean);

  /*
  mean = 0.0f;
  for (size_t i=0; i < GRID_SIZE;i++){
    transformedId = i + (PADDED_COLS + 1) + 2 * (i / (COLS));
    mean += data[transformedId] / GRID_SIZE;
  }
  //mean /= GRID_SIZE;
  printf("%s %f\n", "mean C layer",mean);
  */

  // Read back the results from the device to verify the output
  // CALCULATE STDS

  err = 0;
  err = clSetKernelArg(std_kernel, 0, sizeof(cl_mem), &input);
  err |= clSetKernelArg(std_kernel, 1, sizeof(cl_mem), &cl_partial_stds);
  err |= clSetKernelArg(std_kernel, 2, sizeof(float) * local, NULL);
  err |= clSetKernelArg(std_kernel, 3, sizeof(uint32_t), &PADDED_COLS);
  err |= clSetKernelArg(std_kernel, 4, sizeof(float), &mean);

  if (err != CL_SUCCESS)
  {
    printf("Error: Failed to set mean kernel arguments! %d\n", err);
    exit(1);
  }

  err |= clEnqueueNDRangeKernel(commands, std_kernel, 1, NULL, &global, &local, 0, NULL, NULL); // TODO Optimize local (5th arg)
  clFinish(commands);

  err = clEnqueueReadBuffer(commands, cl_partial_stds, CL_TRUE, 0, sizeof(float)*n_partial_sums, partial_stds, 0, NULL, NULL);

  // Read back the results from the device to verify the output
  if (err != CL_SUCCESS)
  {
    printf("Error: Failed to read output array! %d\n", err);
    exit(1);
  }

  for(size_t i = 0; i < n_partial_sums; i++) {
    std += partial_stds[i];
  }
  std /= n_partial_sums;

  printf("average absolute difference: %.5e\n", std);

/*
  for (size_t i=0; i < GRID_SIZE;i++){
    transformedId = i + (PADDED_COLS + 1) + 2 * (i / (COLS));

    if(i%COLS==0)
      printf("\n");
    printf("%f ",data[transformedId]);
  } */

  // Cleaning up
  clReleaseMemObject(input);
  clReleaseProgram(program);
  clReleaseKernel(heat_diff_kernel);
  clReleaseCommandQueue(commands);
  clReleaseContext(context);

  free(source_str);
  free(partial_sums);
  free(data);

  return 0;
}
