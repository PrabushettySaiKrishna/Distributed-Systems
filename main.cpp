#include "includes/main.h"

#include <mpi.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>

#include "includes/primality_test.h"
#include "includes/utils.h"
#include "includes/worker.h"

using namespace std;

bool is_valid_num(int64_t num) {
  if (num < 0) return false;

  return true;
}

bool is_valid_nth(int64_t nth) {
  if (nth <= 0) return false;

  return true;
}

int main(int argc, char **argv) {
  // Initialize.
  MPI_Init(&argc, &argv);

  int root = 0;
  int rank, num_procs;
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if ((argc < 3) and (rank == 0)) {
    cerr << "Invalid input." << endl;
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  // Synchronize all processes for time keeping.
  MPI_Barrier(MPI_COMM_WORLD);
  double start_time = MPI_Wtime();

  // Do stuff.

  int64_t num = (int64_t)stoll(argv[1]);
  int64_t nth = (int64_t)stoll(argv[2]);
  int batch_size = 1000;  // Have it possibly calculated or input by the user

  bool found = false;

  // Error checking with the manager
  if (rank == 0) {
    if (!is_valid_num(num)) {
      cerr << "Specify n greater than 0" << endl;
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    if (!is_valid_nth(nth)) {
      cerr << "Specify nth greater than 0" << endl;
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
  }

  // If manager
  if (rank == 0) {
    // Begin the loop for search
    int64_t current_prime_count = 0;
    int64_t iteration = 1;

    while (!found) {
      // Broadcast the iteration number to each process
      MPI_Bcast(&iteration, 1, MPI_INT, 0, MPI_COMM_WORLD);

      // Receive the result from each of the processes
      for (int i = 1; i < num_procs; i++) {
        int64_t recv_arr[batch_size];

        MPI_Recv(&recv_arr, batch_size, MPI_INT64_T, i, 0, MPI_COMM_WORLD,
                 MPI_SUCCESS);

        // Check for nth prime
        if (!found) {
          for (auto x : recv_arr) {
            if (x != 0) {
              current_prime_count += 1;

              if (current_prime_count == nth) {
                // Return the answer
                cout << x << "," << (iteration-1) * (num_procs-1) * batch_size + i * batch_size << "," << endl;
                found = true;
              }
            }
          }
        }
      }

      // Increment iteration
      ++iteration;
    }

    // Ask other nodes to break
    iteration = -1;
    MPI_Bcast(&iteration, 1, MPI_INT, 0, MPI_COMM_WORLD);
  }
  // For the followers
  else {
    // Run forever
    while (true) {
      int iteration;

      MPI_Bcast(&iteration, 1, MPI_INT, 0, MPI_COMM_WORLD);

      if (iteration == -1) break;

      int64_t min_val = num + (iteration - 1) * batch_size * (num_procs - 1) +
                        batch_size * (rank - 1);
      int64_t max_val = min_val + batch_size;

      int64_t primes[batch_size] = {0};
      int primecount = 0;
      for (auto i = min_val; i < max_val; i++) {
        if (is_prime(i)) primes[primecount++] = i;
      }

      MPI_Send(&primes, batch_size, MPI_INT64_T, 0, 0, MPI_COMM_WORLD);
    }
  }

  // Synchronize all processes for time keeping.
  MPI_Barrier(MPI_COMM_WORLD);
  double end_time = MPI_Wtime();
  double elapsed_time = end_time - start_time;

  // Get max time across all processes.
  double max_time;
  MPI_Reduce(&elapsed_time, &max_time,
             /*count=*/1, MPI_DOUBLE, MPI_MAX, root, MPI_COMM_WORLD);

  if (rank == root) {
    cout << max_time << endl;
  }

  // Shutdown.
  MPI_Finalize();
  return 0;
}