// Wesley Stutzman
// Parallel Processing matrix multiplication

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

// Edit number of threads to run on
// ~1 thread per core
#define NUMOFTHREADS 1

// Struct designed to pass values into functions
struct arg_struct
{
  // array information
  int x1;
  int y1;
  int** array1;
  int x2;
  int y2;
  int** array2;
  
  // Return Array
  int** returnArray;

  // Number of threads
  int threadNum;
};

// Functions
int** mallocArray(int x, int y);
int** fillArray(int x, int y);
void* matrixMultiply(void* args);

// Main
int main()
{
  srand(time(NULL));
  printf("Starting\n");

  // Initial Variables
  int dim = 1200;
  int x = dim;
  int y = dim;
  int i, m;


  // Fill the arrays with random numbers
  printf("Filling Array:\tx: %d\ty: %d\n", x, y);
  int** a1 = fillArray(x,y);
  int** a2 = fillArray(x,y);
  int** a3 = mallocArray(x,y);
  printf("Filling Array:\tFinished\n");

  // Allocate the needed arrays
  int** finalArray = mallocArray(x,y);
  int** returnArray = mallocArray(x,y);

  // Allocate other recources
  pthread_t threads[NUMOFTHREADS];
  time_t start = time(NULL);
  struct arg_struct args[NUMOFTHREADS];

  // Store information for each thread to process
  for(i = 0; i < NUMOFTHREADS; ++i)
  {
    args[i].x1 = x;
    args[i].y1 = y;
    args[i].array1 = a1;
    args[i].x2 = x;
    args[i].y2 = y;
    args[i].array2 = a2;
    args[i].returnArray = a3;
    args[i].threadNum = i;
    pthread_create(&threads[i], NULL, matrixMultiply, &args[i]);
  }

  // Hold the values bine returned from the threads
  struct arg_struct* returnValue[NUMOFTHREADS];

  // Gather all returned information from threads
  for(i = 0; i < NUMOFTHREADS; ++i)
  {
    pthread_join(threads[i], &returnValue[i]);
  }
 
  // Calculate the size of the chunks
  int chunkSize = x / NUMOFTHREADS;
  int count = 0;
  for(i = 0; i < NUMOFTHREADS; ++i)
  {
    for(m = 0; m < chunkSize; ++m)
    {
      // Final Value      = chunk returned -> rows inside a chunk
      finalArray[count++] = returnValue[i]->returnArray[m];
    }
  }
  
  // End the timer
  double result = ((double)time(NULL) - start);
  printf("Time Taken: %fsec\n", result);

  // Print out the results
  /*
  count = 0;
  for(i = 0; i < x; i++)
  {
    for(m = 0; m < y; m++)
    {
      printf("%d,%d\t%d\t",i,m,finalArray[i][m]);
      if(count++ % 5 == 0)
      {
        printf("\n");
      }
    }
  }
  */

  printf("\nFinished\n");
  return 0; 
}

// Mallocs a specifided array size for you
int** mallocArray(int x, int y)
{
  // Allocate Memory to hold lists
  int** returnArray = (int**)malloc(x * sizeof(int*));
  int i;
  // Allocate Memory to hold integers
  for(i = 0; i < x; ++i)
  {
    returnArray[i] = (int*)malloc(y * sizeof(int));
  }
  return returnArray;
}

// Fills the array with random numbers
int** fillArray(int x, int y)
{
  // Allocate an array to be filled with random numbers
  int** returnArray = mallocArray(x,y);
  int i, m;
  for(i = 0; i < x; ++i)
  {
    for(m = 0; m < y; ++m)
    {
      returnArray[i][m] = rand() % 10 + 1;
    }
  }
  return returnArray;
}

// Multiply two matrices together
void* matrixMultiply(void* inputArgs)
{
  // Initialize variables
  struct arg_struct* args = (struct arg_struct*)inputArgs;
  int i, m, p;
  int start, end;
  int chunkSize;
  
  // Calculate what needs to be processed
  chunkSize = args->x1 / NUMOFTHREADS;    // Find the size of processing chunk
  start = chunkSize * args->threadNum;    // Find the starting position
  end = start + chunkSize;                // Find the ending   position

  // Count Operations
  int operations = 0;  

  // Keep count of true count location
  int count = 0;

  // Multiply the matrix together
  // Loop from start to end of your designated chunk
  for(i = start; i < end; i++)
  {

    // Loop Through the second matices columns
    for(m = 0; m < args->y2; m++)
    {

      // Make sure values are default 0
      args->returnArray[count][m] = 0;

      // Loop Through the first matrices columns
      for(p = 0; p < args->y1; p++)
      {
        args->returnArray[count][m] += args->array1[i][p] * args->array2[p][m];
        //printf("returnArray: %d\n", args->returnArray[count][m]);
        operations++;
      }
    }
    count++;
  }

  // Output info when thread compleats task
  printf("Thread finished: %d\tBlock: %d:%d     \tChunkSize: %d\tOperations: %d\n", args->threadNum, start, end, chunkSize, operations);

  // Exit Thread and return value
  pthread_exit(args);
}
