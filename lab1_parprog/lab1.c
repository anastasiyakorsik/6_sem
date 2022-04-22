#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Right part function
float f(float x, float t) {
    return 2*x*x + t;
}
// Functions to use in initial conditions
float fi(float x) {
    return x * x;
}

float psi(float t) {
    return 2*t;
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv); // Initialize MPI work

    float X = 10, T = 10;
    // grid maximums
    const int K = 1000, M = 1000;
    // grid axes
    float t = 0, x = 0;
    // grid steps
    float tau = T / (K - 1);
    float h = X / (M - 1);

    // grid matrix
    float **u = (float**) malloc(K * sizeof(float*));
    for (int i = 0; i < K; ++i) {
        u[i] = (float*) malloc(M * sizeof(float));
    }

    // matrix size
    int matrix_size = K * M;

    FILE *fp;
    char name[] = "solution.txt";
    fp = fopen(name, "w");

    //filling in angular values of grid using initial conditions
    for (int k = 0; k < K; ++k) {
        t = k * tau;
        u[k][0] = psi(t);
    }

    for (int m = 0; m < M; ++m) {
        x = m * h;
        u[0][m] = fi(x);
    }


    int rank = 0, size = 0;
    double start_time = 0, end_time = 0;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get the process number on which the program is running
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Get the number of processes
    int prev_process_num = 0, next_process_num = 0;
    int message_id = 0;

    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();

    if (rank != 0) {
        prev_process_num = rank - 1;
    } else prev_process_num = size - 1;

    if (rank != size - 1) {
        next_process_num = rank + 1;
    } else next_process_num = 0;

    for (int k = 1 + rank; k < K; k += size) {
        for (int m = 1; m < M; ++m) {
            //accept data from previous process
            if (k != 1 && size > 1)
            {
                MPI_Recv(&(u[k - 1][m]), 1, MPI_FLOAT, prev_process_num, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }

            //calculate the grid note
            x = (m + 1 / 2) * h;
            t = (k + 1 / 2) * tau;
            u[k][m] = (4 * h * tau * f(x, t) - 2 * h * (u[k][m - 1] - u[k - 1][m - 1] - u[k - 1][m]) -
                       2 * tau * (u[k - 1][m] - u[k][m - 1] - u[k - 1][m - 1])) / (2 * tau + 2 * h);
            //send calculated note to the next process
            if (k != K - 1 && size > 1)
            {
                MPI_Send(&(u[k][m]), 1, MPI_FLOAT, next_process_num, message_id, MPI_COMM_WORLD);
            }
        }
    }

    // 0 process receives from other threads
    if (rank == 0) {
        for (int k = 2; k < K; ++k)
            if ((k - 1) % size != 0 && (k % size != 0 || k == K - 1))
                MPI_Recv(&u[k][1], M - 1, MPI_FLOAT, (k - 1) % size, message_id, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    else {
        for (int k = 1 + rank; k < K; k += size)
            if (rank != size - 1 || k == K - 1)
                MPI_Send(&u[k][1], M - 1, MPI_FLOAT, 0, message_id, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD); // measure time when all processes reach this line
    end_time = MPI_Wtime();

    if (rank == 0) {
        printf("time delay (ms) = %lf, number of processes: %d\n", end_time - start_time, size);
        //put solutions into file
        for (int k = 0; k < K; ++k) {
            for (int m = 0; m < M - 1; ++m) {
                fprintf(fp,"%lf",u[k][m]);
                fprintf(fp,"%c",' ');
            }
            fprintf(fp, "\n");
        }
    }

    for (int i = 0; i < K; ++i) {
        free(u[i]);
    }
    free(u);
    fclose(fp);

    MPI_Finalize();

    return 0;
}