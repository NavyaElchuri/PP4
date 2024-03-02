#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

#define infinity 9999

int main(int argc, char **argv) {
    int n = 4;
    int pid, np;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    int m = n / sqrt(np);
    int a[n][n];
    int b[n][n];

    if (pid == 0) {
        // Input matrix D0
        a[0][0] = 0;
        a[0][1] = 3;
        a[0][2] = 6;
        a[0][3] = 15;
        a[1][0] = 2;
        a[1][1] = 0;
        a[1][2] = 8;
        a[1][3] = infinity;
        a[2][0] = 6;
        a[2][1] = 8;
        a[2][2] = 0;
        a[2][3] = 9;
        a[3][0] = 15;
        a[3][1] = infinity;
        a[3][2] = 9;
        a[3][3] = 0;
    }

    MPI_Bcast(a, n * n, MPI_INT, 0, MPI_COMM_WORLD);

    // Perform parallel computation
    for (int k = 0; k < n; k++) {
        for (int i = pid * m; i < (pid + 1) * m; i++) {
            for (int j = 0; j < n; j++) {
                if (a[i][j] > a[i][k] + a[k][j]) {
                    a[i][j] = a[i][k] + a[k][j];
                }
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Allgather(a, n * m, MPI_INT, b, n * m, MPI_INT, MPI_COMM_WORLD);
        for (int i = pid * m; i < (pid + 1) * m; i++) {
            for (int j = 0; j < n; j++) {
                a[i][j] = b[i][j];
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    // Output the matrix D
    if (pid == 0) {
        printf("Final D matrix:\n");
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                printf("%d ", a[i][j]);
            }
            printf("\n");
        }
    }

    MPI_Finalize();

    return 0;
}