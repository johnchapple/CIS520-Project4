run : MPI
	source /etc/profile.d/modules.sh
	module purge
	module load foss
	mpicc MPI.c -o MPI
	mpirun -np 1 MPI 1