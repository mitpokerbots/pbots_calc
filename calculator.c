#include "pbots_calc.h"
#include <stdio.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("usage: %s hand1:hand2:hand... [board [dead]]\n", argv[0]);
    return 0;
  }
  char* board = "";
  char* dead = "";
  if (argc >= 3) {
    board = argv[2];
    if (argc >= 4) {
      dead = argv[3];
    }
  }
  Results* res = alloc_results();
  calc(argv[1], board, dead, 1000000, res);
  print_results(res);
  free_results(res);
  /*
  Hands* hands;
  StdDeck_CardMask dead;
  StdDeck_CardMask_RESET(dead);
  if ((hands = get_hands(argv[1], &dead)) != NULL) {
    print_hands(hands);
  }
  */
  return 1;
}
