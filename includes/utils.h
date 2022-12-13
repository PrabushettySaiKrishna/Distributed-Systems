#ifndef __UTILS_H
#define __UTILS_H

// DEBUG MACROS
#ifdef DEBUG_BUILD

#define DEBUG(x)                                            \
  do {                                                      \
    std::cerr << "DEBUG: " << #x << ": " << x << std::endl; \
    sleep(1);                                               \
  } while (0)

#define DEBUGR(rank, x)                                              \
  do {                                                               \
    std::cerr << "DEBUG ON PROC " << rank << ": " << #x << ": " << x \
              << std::endl;                                          \
    sleep(1);                                                        \
  } while (0)

#define DEBUGM(x) DEBUGR(mpi_rank, x)

#else

#define DEBUGR(rank, x)
#define DEBUGM(x)
#define DEBUG(x)

#endif  // #ifdef DEBUG_BUILD

#endif  // #ifndef __UTILS_H