#ifndef __PBOTS_CALC_H__
#define __PBOTS_CALC_H__

typedef struct {
  double* ev;
  char** hands;
  int iters;
  int size;
  int MC; // did we run Monte Carlo simulation?
} Results;

Results* alloc_results(void);
void print_results(Results*);
void free_results(Results*);
int calc(const char*, char*, char*, int, Results*);

#endif
