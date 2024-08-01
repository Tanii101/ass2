# Define compiler and flags
CC = gcc
MPICC = mpicc
CFLAGS = -fopenmp
ICCCFLAGS = -qopenmp
LDFLAGS = -lm

# Define the targets
all: gccserial iccserial gcccomplete icccomplete

# Serial compilation with GNU compiler
gccserial: main-serial.c heat.c file-reader.c
	$(CC) $(CFLAGS) main-serial.c heat.c file-reader.c -o heat-omp-gcc $(LDFLAGS)

# Serial compilation with Intel compiler
iccserial: main-serial.c heat.c file-reader.c
	@command -v icc >/dev/null 2>&1 && icc $(ICCCFLAGS) main-serial.c heat.c file-reader.c -o heat-omp-icc $(LDFLAGS) || echo "icc compiler not found, skipping iccserial target"

# MPI compilation with GNU compiler
gcccomplete: main-mpi.c heat.c file-reader.c
	$(MPICC) $(CFLAGS) main-mpi.c heat.c file-reader.c -o heat-complete-gcc $(LDFLAGS)

# MPI compilation with Intel compiler
icccomplete: main-mpi.c heat.c file-reader.c
	@command -v mpiicc >/dev/null 2>&1 && mpiicc $(ICCCFLAGS) main-mpi.c heat.c file-reader.c -o heat-complete-icc $(LDFLAGS) || echo "mpiicc compiler not found, skipping icccomplete target"

# Clean up build files
clean:
	rm -f heat-omp-gcc heat-omp-icc heat-complete-gcc heat-complete-icc
