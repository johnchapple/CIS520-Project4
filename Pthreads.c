#include <pthread.h>//p4 addition
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>

//p4 start
//#define STRING_SIZE 2001 // no lines larger than 2000 chars
int STRING_SIZE = 2001;
//#define CPU_NUM 8
int cpu_num = 0;
//#define ARRAY_SIZE 1000000//was 1000
int ARRAY_SIZE = 0;
int NUM_THREADS = 0;
//int ARRAY_SIZE = 1000;//was 10.31pm

pthread_mutex_t mutexsum;

//char line_array[ARRAY_SIZE][STRING_SIZE];
//float line_avg[ARRAY_SIZE];			// count of individual characters
char *line_array;
float *line_avg;

//p4 stop

float find_avg(char* line, int nchars) {
   int i, j;
   float sum = 0.0;

   for ( i = 0; i < nchars; i++ ) {
      sum += ((int) line[i]);
   }

   if (nchars > 0)
        return sum / (float) nchars;
   else
        return 0.0;
}

//p4 start
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

void *count_array(void *myID)
{
  char theChar;
  int i, j, charLoc;
  float local_line_avg[ARRAY_SIZE];

  int startPos = ((int) myID) * (ARRAY_SIZE / NUM_THREADS);
  int endPos = startPos + (ARRAY_SIZE / NUM_THREADS);

  printf("myID = %d startPos = %d endPos = %d \n", (int) myID, startPos, endPos);

					// init local count array
  for ( i = 0; i < ARRAY_SIZE; i++ ) {
  	local_line_avg[i] = 0.0;
  }
					// count up our section of the global array
  for ( i = startPos; i < endPos; i++) {
     local_line_avg[i]=find_avg(line_array[i], strlen(line_array[i]));
  }
					// sum up the partial counts into the global arrays
  pthread_mutex_lock (&mutexsum);
   for ( i = 0; i < ARRAY_SIZE; i++ ) {
      line_avg[i] += local_line_avg[i];
   }
  pthread_mutex_unlock (&mutexsum);

  pthread_exit(NULL);
}

void print_results(float the_line_avg[])
{
  int i = 0;

  					// then print out the totals
  for ( i = 0; i < ARRAY_SIZE; i++ ) {
	printf("%d: %.1f\n", i, the_line_avg[i]);
  }
}
//p4 stop 

main(int argc, char* argv[]) 
{
   //printf("####\n");
   //printf("%s\n", (char *)argv[1]);		//2
   //printf("%d\n", strtol(argv[1],NULL, 10));	//2
   cpu_num = strtol(argv[1], NULL, 10);
   NUM_THREADS = strtol(argv[2], NULL, 10);//was a static 4
   ARRAY_SIZE = strtol(argv[3], NULL, 10);//1000000
   printf("###cpu_num: %d, num_threads: %d, array_size: %d ###\n",cpu_num, NUM_THREADS, ARRAY_SIZE);

   line_array = (char *)malloc(ARRAY_SIZE * STRING_SIZE * sizeof(char));
   line_avg = malloc(sizeof(float)*ARRAY_SIZE);

   //printf("####\n");

   clock_t begin = clock();//p4

   int a;
   for (a = 0; a < sizeof(argv); a++) {
      printf("%s", argv[a]);
   }
   printf("%d", argc);

   //int nlines = 0, maxlines = 1000000;
   int i, j, err, rc;
   //p4 start
	pthread_t threads[NUM_THREADS];
	pthread_attr_t attr;

   struct timeval t1, t2;
   double elapsedTime;
   gettimeofday(&t1, NULL);

   pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   init_arrays();


   //p4 test start
   for (i = 0; i < NUM_THREADS; i++ ) {
      rc = pthread_create(&threads[i], &attr, count_array, (void *)i);
      if (rc) {
         printf("ERROR; return code from pthread_create() is %d\n", rc);
		   exit(-1);
      }
	}

   for (j = 0; j < NUM_THREADS; j++)
       pthread_join(threads[j], NULL);
   
   print_results(line_avg);

   clock_t end = clock();

	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

   gettimeofday(&t2, NULL);
   elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; //sec to ms
   elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms
   printf("DATA, %s, %f\n", getenv("SLURM_NTASKS"),  elapsedTime);

   //cpu efficiency=cpu_time / (run_time x number_of_cpus)
   printf("CPU efficiency: %f\n", elapsedTime / (elapsedTime * cpu_num));

   /*
    int help;
    char command[20] = "top";//"top -p ";
    //printf("%lun",getppid());
    //strcat(command, (char*)getppid());
    //printf(command);
    help = system(command);
    */
   /*
   struct rusage usage;

   printf(getrusage(RUSAGE_SELF, &usage));
	printf("Main: program completed. Time spent = %d. Exiting.\n", time_spent);
   */
  /*
   FILE* status = fopen( "/proc/self/status", "r" );
   char filename[100], c;
   c = fgetc(status);
   int x = 0;
   int help;
   int track = 7;
   char command[20] = "top -p ";
   while (c != EOF)
   {
      if(x > 70 && x < 75)//was 70-75
      {
         //printf ("%c", c);
         
         //printf("%lun",getppid());
         //strcat(helper[track],c);
         //track ++;
         //printf("%d",track);
         command[track] = c;
         track++;
         //printf(helper);
         //strcat(command, (char)c);
         //strcat(command, (char*)getppid());
         
      }
      c = fgetc(status);
      x++;
   }
   //strcat(command, helper);
   //printf("###");
   //printf(command);
   help = system(command);
  
    fclose(status);
    */
   //p4 end

   //fclose( fd );//p4

}
