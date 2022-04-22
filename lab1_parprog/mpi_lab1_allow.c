#include "mpi.h"
#include <stdio.h>

int main(int argc, char **argv)
{

	int rank = 0, size = 0;
	int data_recv = 0, data_send = 0;
	double start_time = 0, end_time = 0;

	MPI_Status status;

	MPI_Init(&argc, &argv); // Initialize MPI work
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get the process number on which the program is running
	MPI_Comm_size(MPI_COMM_WORLD, &size); // Get the number of processes

	if(rank == 0) // if it is the first process
	{
		start_time = MPI_Wtime();
		MPI_Send(&data_send, 1, MPI_INT, 1, 5, MPI_COMM_WORLD);
	}

	if(rank == 1) // if it is the second process
	{
		MPI_Recv(&data_recv, 1, MPI_INT, 0, 5, MPI_COMM_WORLD, &status);
	}

	if(rank == 0) // if it is the first process
	{
		end_time = MPI_Wtime();
        printf("time delay (ms) = %lf", end_time - start_time);
    }

	MPI_Finalize(); 

}
