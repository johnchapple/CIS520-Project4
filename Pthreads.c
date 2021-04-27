#include <pthread.h>//p4 addition
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//p4 start
#define NUM_THREADS 4
#define ARRAY_SIZE 2000000
#define STRING_SIZE 16
#define ALPHABET_SIZE 26
pthread_mutex_t mutexsum;
//p4 stop

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

//p4 start
void *count_array(void *myID)
{
  char theChar;
  int i, j, charLoc;
  int local_line_avg[ARRAY_SIZE];

  int startPos = ((int) myID) * (ARRAY_SIZE / NUM_THREADS);
  int endPos = startPos + (ARRAY_SIZE / NUM_THREADS);

  printf("myID = %d startPos = %d endPos = %d \n", (int) myID, startPos, endPos);

					// init local count array
  for ( i = 0; i < ARRAY_SIZE; i++ ) {
  	local_live_avg[i] = 0;
  }
					// count up our section of the global array
  for ( i = startPos; i < endPos; i++) {
     local_line_avg[i]=find_avg(line_array[i], strlen(line_array[i]));
  }
					// sum up the partial counts into the global arrays
  pthread_mutex_lock (&mutexsum);
  for ( i = 0; i < ALPHABET_SIZE; i++ ) {
     char_counts[i] += local_char_count[i];
  }
  pthread_mutex_unlock (&mutexsum);

  pthread_exit(NULL);
}

void print_results(float the_line_avg[])
{
  int i,j, total = 0;

  					// then print out the totals
  for ( i = 0; i < ARRAY_SIZE; i++ ) {
	printf("%d: %.1f\n", i, the_line_avg[i]);
  }
}
//p4 stop

int main()
{
   clock_t begin = clock();//p4

   int nlines = 0, maxlines = 1000000;
   int i, err;
   //p4 start
	pthread_t threads[NUM_THREADS];
	pthread_attr_t attr;

   pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
   //p4 end
   float  charsum=0.0;
   int nchars = 0;
   FILE *fd;
   char line = (char) malloc( 2001 ); // no lines larger than 2000 chars

   // Read in the lines from the data file

   fd = fopen( "/homes/dan/625/wiki_dump.txt", "r" );
   for ( i = 0; i < maxlines; i++ )  {
      err = fscanf( fd, "%[^\n]\n", line);
      if( err == EOF ) break;
      nchars = strlen( line );
      printf("%d: %.1f\n", nlines, find_avg(line, nchars));
      nlines++;
   }

   //p4 test start
   for (i = 0; i < NUM_THREADS; i++ ) {
      rc = pthread_create(&threads[i], &attr, count_array, (void *)i);
      if (rc) {
         printf("ERROR; return code from pthread_create() is %d\n", rc);
		   exit(-1);
      }
	}
   
   
   print_results(line_avg);

   clock_t end = clock();

	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	printf("Main: program completed. Time spent = %d. Exiting.\n", time_spent);
   //p4 end

   fclose( fd );

}