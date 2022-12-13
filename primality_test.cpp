#include "includes/primality_test.h"

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

#include "includes/utils.h"

using namespace std;

// Deterministic Miller Rabin guaranteed to be correct for 64 bit integers.

// Computes base^exponent modulo mod using binary exponentiation.
int64_t binary_power(int64_t base, int64_t exponent, int64_t mod) {
  int64_t result = 1;
  base %= mod;
  while (exponent) {
    if (exponent & 1) result = (__int128_t)result * base % mod;
    base = (__int128_t)base * base % mod;
    exponent >>= 1;
  }
  return result;
}

// Checks the Miller-Rabin primality condition for a particular witness.
bool check_composite(int64_t n, int64_t a, int64_t d, int s) {
  int64_t x = binary_power(a, d, n);

  if (x == 1 || x == n - 1) {
    return false;
  }

  for (int r = 1; r < s; ++r) {
    x = (__int128_t)x * x % n;
    if (x == n - 1) {
      return false;
    }
  }

  return true;
}

// Miller-Rabin primality test. Checks only as many prime witnesses as needed
// for 64 bit integers.
// Returns true if n is prime, false otherwise.
bool deterministic_miller_rabin_64(int64_t n) {
  if (n < 2) {
    return false;
  }

  int r = 0;
  int64_t d = n - 1;
  while ((d & 1) == 0) {
    d >>= 1;
    r++;
  }

  for (int a : {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37}) {
    if (n == a) {
      return true;
    }

    if (check_composite(n, a, d, r)) return false;
  }

  return true;
}

// Exported function for checking primality.
bool is_prime(__int64_t n) { return deterministic_miller_rabin_64(n); }