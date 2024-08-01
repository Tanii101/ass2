#include <stdio.h>
#include <stdlib.h>

extern double get_final_temperatures(int N, int maxIter, double radTemp, int fracX, int fracY);
extern int read_dims(char *filename);
extern double* read_array(char *filename, int numOfValues);
extern void *write_to_output_file(char *filename, double *output, int numOfValues);

int main(int argc, char *argv[]) {
    if (argc != 6) {
        fprintf(stderr, "Usage: %s <N> <maxIter> <input filename> <frac filename> <output filename>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    int maxIter = atoi(argv[2]);
    char *inputFileName = argv[3];
    char *fracFileName = argv[4];
    char *outputFileName = argv[5];

    int numOfTemps = read_dims(inputFileName);
    double *temps = read_array(inputFileName, numOfTemps);
    double *fracs = read_array(fracFileName, numOfTemps * 2);

    double *results = (double *)malloc(numOfTemps * sizeof(double));
    if (results == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return 1;
    }

    for (int i = 0; i < numOfTemps; i++) {
        results[i] = get_final_temperatures(N, maxIter, temps[i], fracs[i * 2], fracs[i * 2 + 1]);
    }

    write_to_output_file(outputFileName, results, numOfTemps);

    free(temps);
    free(fracs);
    free(results);

    return 0;
}
