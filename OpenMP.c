#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define NUM_THREADS 1

#define ARRAY_SIZE 1000 //Numeber of lines
#define STRING_SIZE 2001 //size of lines
//#define CPU_NUM 8//number of cpus
int cpu_num = 0;

char line_array[ARRAY_SIZE][STRING_SIZE];
float line_avg[ARRAY_SIZE];			// count of individual characters


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

void *count_array(int myID)
{
	char theChar;
	int i, j, charLoc;
	float local_line_avg[ARRAY_SIZE];
	int startPos, endPos;

	#pragma omp private(myID,theChar,charLoc,local_char_count,startPos,endPos,i,j)
	{
		startPos = myID * (ARRAY_SIZE / NUM_THREADS);
		endPos = startPos + (ARRAY_SIZE / NUM_THREADS);

		printf("myID = %d startPos = %d endPos = %d \n", myID, startPos, endPos);

						// init local count array
		for ( i = 0; i < ARRAY_SIZE; i++ ) {
			local_line_avg[i] = 0.0;
		}
						// count up our section of the global array
		for ( i = startPos; i < endPos; i++) {
			local_line_avg[i]=find_avg(line_array[i], strlen(line_array[i]));
		}

						// sum up the partial counts into the global arrays
		#pragma omp critical
		{
			for ( i = 0; i < ARRAY_SIZE; i++ ) {
		 		line_avg[i] += local_line_avg[i];
			}
		}
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

main(int argc, char* argv[]) {

	//clock_t begin = clock();
	cpu_num = strtol(argv[1], NULL, 10);

	struct timeval t1, t2;
    double elapsedTime;
    gettimeofday(&t1, NULL);

	omp_set_num_threads(NUM_THREADS);

	init_arrays();

	#pragma omp parallel 
	{
		count_array(omp_get_thread_num());
	}

	print_results(line_avg);

	//clock_t end = clock();
	//double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	gettimeofday(&t2, NULL);
	elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; //sec to ms
	elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms
	printf("DATA, %s, %f\n", getenv("SLURM_NTASKS"),  elapsedTime);

	//cpu efficiency=cpu_time / (run_time x number_of_cpus)
    printf("CPU efficiency: %f\n", elapsedTime / (elapsedTime * CPU_NUM));

	//printf("Main: program completed. Exiting.\n");
}

