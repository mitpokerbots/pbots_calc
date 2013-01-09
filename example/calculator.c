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

#include <stdio.h>
#include "pbots_calc.h"

/*
 * Simple command line tool for equity calculations. Demonstrates how to use the
 * pbots_calc library.
 */

int main(int argc, char **argv) {
  Results* res;
  char* board = "";
  char* dead = "";
  if (argc < 2) {
    printf("usage: %s hand1:hand2:hand... [board [dead]]\n", argv[0]);
    return 0;
  }
  if (argc >= 3) {
    board = argv[2];
    if (argc >= 4) {
      dead = argv[3];
    }
  }
  res = alloc_results();
  calc(argv[1], board, dead, 1000000, res);
  print_results(res);
  free_results(res);
  return 1;
}
