#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

extern double get_final_temperatures(int N, int maxIter, double radTemp, int fracX, int fracY);
extern int read_dims(char *filename);
extern double* read_array(char *filename, int numOfValues);
extern void *write_to_output_file(char *filename, double *output, int numOfValues);

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    if (argc != 6) {
        fprintf(stderr, "Usage: %s <N> <maxIter> <input filename> <frac filename> <output filename>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    int N = atoi(argv[1]);
    int maxIter = atoi(argv[2]);
    char *inputFileName = argv[3];
    char *fracFileName = argv[4];
    char *outputFileName = argv[5];

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int numOfTemps = 0;
    double *temps = NULL;
    double *fracs = NULL;

    if (rank == 0) {
        numOfTemps = read_dims(inputFileName);
        temps = read_array(inputFileName, numOfTemps);
        fracs = read_array(fracFileName, numOfTemps * 2);
    }

    MPI_Bcast(&numOfTemps, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank != 0) {
        temps = (double *)malloc(numOfTemps * sizeof(double));
        fracs = (double *)malloc(numOfTemps * 2 * sizeof(double));
    }

    MPI_Bcast(temps, numOfTemps, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(fracs, numOfTemps * 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    int local_numOfTemps = numOfTemps / size;
    int remainder = numOfTemps % size;

    if (rank < remainder) {
        local_numOfTemps++;
    }

    double *local_results = (double *)malloc(local_numOfTemps * sizeof(double));
    int local_start = rank * (numOfTemps / size) + (rank < remainder ? rank : remainder);
    int local_end = local_start + local_numOfTemps;

    for (int i = local_start; i < local_end; i++) {
        local_results[i - local_start] = get_final_temperatures(N, maxIter, temps[i], fracs[i * 2], fracs[i * 2 + 1]);
    }

    double *results = NULL;
    if (rank == 0) {
        results = (double *)malloc(numOfTemps * sizeof(double));
    }

    MPI_Gather(local_results, local_numOfTemps, MPI_DOUBLE, results, local_numOfTemps, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        write_to_output_file(outputFileName, results, numOfTemps);
        free(results);
    }

    free(local_results);
    free(temps);
    free(fracs);

    MPI_Finalize();
    return 0;
}
