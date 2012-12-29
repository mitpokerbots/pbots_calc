#ifndef __CALC_INT_H__
#define __CALC_INT_H__

typedef struct {
  double* ev;
  char** hands;
  int iters;
  int size;
  int MC; // did we run Monte Carlo simulation?
} Results;

extern Results* alloc_results(void);
extern void print_results(Results*);
extern void free_results(Results*);
extern int calc(const char*, char*, char*, int, Results*);

#endif
