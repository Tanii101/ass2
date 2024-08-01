#include <omp.h>
#include <math.h>
#include <stdlib.h>

double get_final_temperatures(int N, int maxIter, double radTemp, int fracX, int fracY) {
    int i, j, iter;
    double (*prev_t)[N] = malloc(N * N * sizeof(double));
    double (*curr_t)[N] = malloc(N * N * sizeof(double));

    // Initialize the temperature arrays
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (j == N-1 && i >= floor((N-1)*0.3) && i <= ceil((N-1)*0.7)) {
                prev_t[i][j] = radTemp;
            } else {
                prev_t[i][j] = 10.0;
            }
        }
    }

    // Iterative computation
    for (iter = 0; iter < maxIter; iter++) {
        #pragma omp parallel for private(i, j)
        for (i = 1; i < N-1; i++) {
            for (j = 1; j < N-1; j++) {
                curr_t[i][j] = 0.25 * (prev_t[i+1][j] + prev_t[i-1][j] + prev_t[i][j+1] + prev_t[i][j-1]);
            }
        }

        // Swap the arrays
        double (*temp)[N] = prev_t;
        prev_t = curr_t;
        curr_t = temp;
    }

    int point_x = floor((N-1) * fracX);
    int point_y = floor((N-1) * fracY);
    double result = prev_t[point_x][point_y];

    // Free the allocated memory
    free(prev_t);
    free(curr_t);

    return result;
}
