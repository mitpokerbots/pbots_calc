/*
 * Copyright (C) 2012-2013 Owen Derby (ocderby@gmail.com)
 *
 * This file is part of pbots_calc.
 *
 * pbots_calc is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * pbots_calc is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * pbots_calc in a file in teh toplevel directory called "GPLv3".  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef __PBOTS_CALC_H__
#define __PBOTS_CALC_H__

//#define VERBOSE

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
  #define EXPORT_DLL_FLAG __declspec(dllexport)
  #define STRTOK strtok_s
#else
  #define EXPORT_DLL_FLAG
  #define STRTOK strtok_r
#endif

#ifdef __cplusplus    // If used by C++ code,
extern "C" {          // we need to export the C interface
#endif

typedef struct {
  double* ev;
  char** hands;
  int iters;
  int size;
  int MC; // did we run Monte Carlo simulation?
} Results;

// Allocate, print, and free the results data structure
EXPORT_DLL_FLAG Results* alloc_results(void);
EXPORT_DLL_FLAG void print_results(Results*);
EXPORT_DLL_FLAG void free_results(Results*);
// Main library function - call equity calculator!
EXPORT_DLL_FLAG int calc(const char*, char*, char*, int, Results*);

#ifdef __cplusplus
}
#endif

#endif
