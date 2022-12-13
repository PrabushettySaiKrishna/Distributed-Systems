# Distributed-Systems
A distributed prime number finder. The algorithm is implemented using MPI Programming. 

## Development
- Please set your editor to use the Google C++ format.
> For VSCode, go to preferences and in the workspace tab, check format on save. Then, set clang-format's fallback style to "Google".

- Useful make targets:
```
make primetime
make debug
make clean
```

- Command to run:
```
mpirun -np <num-procs> -use-hwthread-cpus -oversubscribe ./primetime <base> <offset>
``` 

here, `<num-procs>` is the number of threads are need to be used.
`<base>` is the first element.
`<offset>` is the last element of the loop.

- Output
It is the time taken by the algoritm to find the prime numbers.

## Code Details

### Include Folder

It contains header files. All the files contains library headers, funtions definition and definition of funtions.

### Makefile

Make file is a program that's used to build other programs or documents. Make is suitable for any process that has intermediate steps that may or may not be complete. Make doesn't rebuild things that are already up to date.

### main.cpp

The file first check if the input is valid or not using the funtions:

```
bool is_valid_num(int64_t num) {
  if (num < 0) return false;

  return true;
}

bool is_valid_nth(int64_t nth) {
  if (nth <= 0) return false;

  return true;
}
```
Initialise the main funtion of MPI, `MPI_Init`. Then, Synchronize all processes for time keeping using.

```
  MPI_Barrier(MPI_COMM_WORLD);
```

After checking the errors with the manager and the input. Start with the basic search. 

1. If it is the manager, then begin the loop for search. Broadcast the iteration number till the element is not found.
```
MPI_Bcast(&iteration, 1, MPI_INT, 0, MPI_COMM_WORLD);
```

After receiving the result from each process, check for the prime number.

```
MPI_Recv(&recv_arr, batch_size, MPI_INT64_T, i, 0, MPI_COMM_WORLD, MPI_SUCCESS);

if (current_prime_count == nth) 
{
    // Return the answer
    cout << x << "," << (iteration-1) * (num_procs-1)* batch_size + i * batch_size << "," << endl;
    found = true;
}
```
2. If follower, then run the loop forever and send the primes between base and offset.

```
for (auto i = min_val; i < max_val; i++) {
    if (is_prime(i)) primes[primecount++] = i;
}

MPI_Send(&primes, batch_size, MPI_INT64_T, 0, 0, MPI_COMM_WORLD);
```

After that, synchronize all processes for time keeping and return the max time of all processes and shutdown the MPI.

```
MPI_Finalize();
```

### primality_test.cpp

First, the basic code of modular binary exponentiation is explained. The power is generally evaluated under the modulo of a large number. Below is the fundamental modular property that is used for efficiently computing power under modular arithmetic. 
```
base %= mod;
while (exponent) {
    if (exponent & 1) result = (__int128_t)result * base % mod;
    base = (__int128_t)base * base % mod;
    exponent >>= 1;
}
```
Then, the `check_composite` function is checking the Miller-Robin primality condition for a particular witness.

```
for (int r = 1; r < s; ++r) {
    x = (__int128_t)x * x % n;
    if (x == n - 1) {
      return false;
    }
}
```

The above code checks if the number x has any of the prime factors other than 1 or x, then it is not prime and return `false`, else `true`.

The `deterministic_miller_rabin_64` function is Miller-Rabin primality test. Checks only as many prime witnesses as needed for 64 bit integers. It returns `true` if n is prime, `false` otherwise.

```
for (int a : {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37}) {
    if (n == a) {
      return true;
    }
}

```
It is given the some prime numbers and directly checks if it belongs to them or not. Return `true` if belongs to them, else
```
check_composite(n, a, d, r)
```
and return true or false based on that.

### Other files

Other files, utils.cpp and worker.cpp provide basi struture for master and follower nodes.
