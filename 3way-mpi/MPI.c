#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

//#define NUM_THREADS 4
int NUM_THREADS;

#define ARRAY_SIZE 1000000
#define STRING_SIZE 2001
//#define CPU_NUM 8
int cpu_num = 0;
int set = 0;//helps to initialize the cpu_num

char line_array[ARRAY_SIZE][STRING_SIZE];
float line_avg[ARRAY_SIZE];			// count of individual characters
float local_line_avg[ARRAY_SIZE];

void init_arrays()
{
  int i, j, err;
  FILE *fd;

   fd = fopen( "/homes/dan/625/wiki_dump.txt", "r" );
   for ( i = 0; i < ARRAY_SIZE; i++ )  {
      err = fscanf( fd, "%[^\n]\n", line_array[i]);
      if( err == EOF ) break;
   }

  for ( i = 0; i < ARRAY_SIZE; i++ ) {
  	line_avg[i] = 0.0;
  }
}

float find_avg(char* line, int nchars) {
   int i, j;
   float sum = 0;

   for ( i = 0; i < nchars; i++ ) {
      sum += ((int) line[i]);
   }

   if (nchars > 0)
        return sum / (float) nchars;
   else
        return 0.0;
}

void *count_array(void *rank)
{
  char theChar;
  int i, j, charLoc;
  int myID =  *((int*) rank);

  int startPos = ((long) myID) * (ARRAY_SIZE / NUM_THREADS);
  int endPos = startPos + (ARRAY_SIZE / NUM_THREADS);

  printf("myID = %d startPos = %d endPos = %d \n", myID, startPos, endPos); fflush(stdout);

    // init local count array
  for ( i = 0; i < ARRAY_SIZE; i++ ) {
  	local_line_avg[i] = 0.0;
  }

    // count up our section of the global array
  for ( i = startPos; i < endPos; i++) {
     local_line_avg[i]=find_avg(line_array[i], strlen(line_array[i]));
  }
}

void print_results(float the_line_avg[])
{
  int i,j, total = 0;

  // then print out the totals
  for ( i = 0; i < ARRAY_SIZE; i++ ) {
	printf("%d: %.1f\n", i, the_line_avg[i]);
  }
}

main(int argc, char* argv[]) 
{

	int i, rc;
	int numtasks, rank;
    //mpirun -np 2 MPI 5           
    //printf("###%d###\n", strtol(argv[0],NULL, 10));
    //printf("###%d###\n", strtol(argv[1],NULL, 10));//5
    //printf("###%d###\n", strtol(argv[3],NULL, 10));
    //printf("###%d###\n", strtol(argv[4],NULL, 10));
    cpu_num = strtol(argv[1], NULL, 10);

	MPI_Status Status;

    struct timeval t1, t2;
    double elapsedTime;
    gettimeofday(&t1, NULL);

	rc = MPI_Init(&argc,&argv);
	if (rc != MPI_SUCCESS) 
    {
	    printf ("Error starting MPI program. Terminating.\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	NUM_THREADS = numtasks;
	printf("size = %d rank = %d\n", numtasks, rank);
	fflush(stdout);

	if ( rank == 0 ) {
		init_arrays();
	}
	MPI_Bcast(line_array, ARRAY_SIZE * STRING_SIZE, MPI_CHAR, 0, MPI_COMM_WORLD);
		
	count_array(&rank);

  //MPI_Reduce(local_char_count, char_counts, ALPHABET_SIZE, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(local_line_avg, line_avg, ARRAY_SIZE, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    

	if ( rank == 0 ) {
		print_results(line_avg);
    
    gettimeofday(&t2, NULL);
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; //sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms
    printf("DATA, %s, %f\n", getenv("SLURM_NTASKS"),  elapsedTime);

    //cpu efficiency=cpu_time / (run_time x number_of_cpus)
    printf("CPU efficiency: %f\n", elapsedTime / (elapsedTime * cpu_num));

    //printf("Main: program completed. Exiting.\n");

    /*
    int help;
    char command[20] = "./Pthreads";
    help = system(command);
    */
	}

  

	MPI_Finalize();
	return 0;
}

