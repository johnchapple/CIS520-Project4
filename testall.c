#include <stdio.h>
#include <string.h>

int main()
{
    /*
     
    */
    int help;
    char command[100] = 
        "sbatch sb.MPI-1; sbatch sb.MPI-2; sbatch sb.MPI-3;";// sbatch sb.MPI-4;";
    help = system(command);
    char g2[100] = 
        "sbatch sb.OpenMP-1; sbatch sb.OpenMP-2; sbatch sb.OpenMP-3; sbatch sb.OpenMP-4;";
    //help = system(g2);
    char g3[100] = 
        "sbatch sb.Pthreads-1; sbatch sb.Pthreads-2; sbatch sb.Pthreads-3; sbatch sb.Pthreads-4;";
    //help = system(g3);
    return 0;
}

