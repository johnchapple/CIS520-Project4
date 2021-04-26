#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

int main()
{
   int nlines = 0, maxlines = 1000000;
   int i, err;
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

   fclose( fd );

}