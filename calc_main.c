#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "poker_defs.h"
#include "enumdefs.h"

//#include "inlines\eval.h"
typedef struct h_dist h_dist;
// store distribution of pocket cards in ll
typedef struct h_dist {
  StdDeck_CardMask cards;
  h_dist* next;
  h_dist* prev;
} Hand_Dist;

typedef struct {
  Hand_Dist* hand_dist;
  int dist_n;
} Hand;

typedef enum {
  ERROR,
  SINGULAR,
  RAND,
  PAIR,
  SUITED,
  OFFSUIT,
  NONE
} pocket_type;

static int char2rank(char c) {
  switch(toupper(c)) {
    case '2': return StdDeck_Rank_2;
    case '3': return StdDeck_Rank_3;
    case '4': return StdDeck_Rank_4;
    case '5': return StdDeck_Rank_5;
    case '6': return StdDeck_Rank_6;
    case '7': return StdDeck_Rank_7;
    case '8': return StdDeck_Rank_8;
    case '9': return StdDeck_Rank_9;
    case 'T': return StdDeck_Rank_TEN;
    case 'J': return StdDeck_Rank_JACK;
    case 'Q': return StdDeck_Rank_QUEEN;
    case 'K': return StdDeck_Rank_KING;
    case 'A': return StdDeck_Rank_ACE;
    default: return -1;
  }
}

static int char2suit(char c) {
  switch(toupper(c)) {
    case 'h': return StdDeck_Suit_HEARTS;
    case 'd': return StdDeck_Suit_DIAMONDS;
    case 'c': return StdDeck_Suit_CLUBS;
    case 's': return StdDeck_Suit_SPADES;
    default: return -1;
  }
}

void insert(StdDeck_CardMask cards, Hand* hand) {
  Hand_Dist* new = (Hand_Dist*)malloc(sizeof(Hand_Dist));
  new->cards = cards;
  if (hand->dist_n < 1) {
    // first entry, needs to be self-referencing
    new->next = new;
    new->prev = new;
    hand->hand_dist = new;
  } else {
    Hand_Dist* cur = hand->hand_dist;
    // ensure no duplicate entries
    for (int i=0; i<hand->dist_n; i++) {
      if (StdDeck_CardMask_EQUAL(cards, cur->cards)) {
        return;
      }
      cur = cur->next;
    }
    // set my refs
    new->next = hand->hand_dist;
    new->prev = hand->hand_dist->prev;
    // update other refs
    new->prev->next = new;
    hand->hand_dist->prev = new;
  }
  // incr counter
  hand->dist_n++;
}

void free_hand(Hand* hand) {
  Hand_Dist* next;
  Hand_Dist* cur = hand->hand_dist;
  for (int i=0; i<hand->dist_n; i++) {
    next = cur->next;
    //printf("freeing %d\n", next->cards);
    free(cur);
    cur = next;
  }
  free(hand);
}

pocket_type get_pocket_type(const char* pocket) {
  printf("strlen: %d\n", (int)strlen(pocket));
  if (strchr("x", tolower(pocket[0])) != NULL)
    return RAND;
  size_t n_p = strlen(pocket);
  // filter out blatant errors in input
  for (int i=0; i<n_p; i++) {
    if (strchr("23456789TJQKA+-SHDCO", toupper(pocket[i])) == NULL)
      return ERROR;
  }
  if (n_p < 2)
    return ERROR;

  if (n_p == 4) {
    if (strchr("shdc", tolower(pocket[1])) != NULL &&
        strchr("shdc", tolower(pocket[3])) != NULL &&
        strchr("23456789TJQKA", toupper(pocket[2])) != NULL) {
          return SINGULAR;
    }
  }
  if (n_p >= 2 && pocket[0] == pocket[1])
    return PAIR;
  if (n_p >= 3) {
    if (strchr("23456789TJQKA", toupper(pocket[1])) == NULL)
      return ERROR;
    if (n_p == 3 ||
        (n_p == 4 && pocket[3] == '+') ||
        (n_p == 7 && pocket[3] == '-' &&
         strchr("23456789TJQKA", toupper(pocket[4])) != NULL &&
         strchr("23456789TJQKA", toupper(pocket[5])) != NULL &&
         pocket[2] == pocket[6])) {
      if (pocket[2] == 's')
        return SUITED;
      if (pocket[2] == 'o')
        return OFFSUIT;
      return ERROR;
    }
  }
  if (n_p == 2)
    return NONE;

  return ERROR;
}

int extract_cards_singular(char* cards, Hand* hand, StdDeck_CardMask dead) {
  StdDeck_CardMask pocket;
  StdDeck_CardMask_RESET(pocket);
  int card;

  if (DstringToCard(StdDeck, cards, &card) == 0) {
    printf("R1: parsing card1 failed\n");
    return 0;
  }
  StdDeck_CardMask_SET(pocket, card);
  if (DstringToCard(StdDeck, cards+2, &card) == 0) {
    printf("R1: parsing card2 failed\n");
    return 0;
  }
  StdDeck_CardMask_SET(pocket, card);
  if (StdDeck_CardMask_ANY_SET(dead, pocket)) {
    printf("R1: in dead cards\n");
    return 1;
  }
  insert(pocket, hand);
  return 1;
}

void extract_cards_random(Hand* hand, StdDeck_CardMask dead) {
  StdDeck_CardMask curHand;
  DECK_ENUMERATE_2_CARDS_D(StdDeck, curHand, dead, insert(curHand, hand););
}

int extract_cards_pair(char* cards, Hand* hand, StdDeck_CardMask dead) {
  // extract limits to possible ranges
  int ceil = char2rank(cards[0]);
  int floor = char2rank(cards[0]);
  if (strchr(cards, '-') != NULL) {
    const char* index = strchr(cards, '-');
    if ((floor = char2rank(*(index+1))) < 0) {
      return 0;
    }
  } else if (strchr(cards, '+') != NULL) {
    ceil = StdDeck_Rank_ACE;
  }
  // if inverse range specified, just flip it
  if (floor > ceil) {
    int temp = floor;
    floor = ceil;
    ceil = temp;
  }

  StdDeck_CardMask pocket;
  // enumerate all cards in range
  for (int rank=floor; rank <= ceil; rank++) {
    for(int suit1 = StdDeck_Suit_FIRST; suit1 <= StdDeck_Suit_LAST; suit1++) {
      for (int suit2 = suit1 + 1; suit2 <= StdDeck_Suit_LAST; suit2++) {
        StdDeck_CardMask_RESET(pocket);
        StdDeck_CardMask_SET(pocket, StdDeck_MAKE_CARD(rank, suit1) );
        StdDeck_CardMask_SET(pocket, StdDeck_MAKE_CARD(rank, suit2) );
        if (!StdDeck_CardMask_ANY_SET(dead, pocket)) {
          insert(pocket, hand);
        }
      }
    }
  }
  return 1;
}

int extract_cards_suited(char* cards, Hand* hand, StdDeck_CardMask dead) {
  // extract limits to possible ranges
  int ceil = char2rank(cards[0]);
  int floor = char2rank(cards[0]);
  if (strchr(cards, '-') != NULL) {
    const char* index = strchr(cards, '-');
    floor = char2rank(*(index+1));
  } else if (strchr(cards, '+') != NULL) {
    ceil = StdDeck_Rank_ACE;
  }
  if (floor < 0 || ceil < 0) {
    return 0;
  }
  return 1;
}

int extract_cards_offsuit(char* cards, Hand* hand, StdDeck_CardMask dead) {
  return 1;
}

int parse_pocket(char* hand_text, Hand* hand, StdDeck_CardMask dead) {
  char *c = strtok(hand_text, ",");
  while (c != NULL) {
    printf("here2: %s\n", c);
    pocket_type p = get_pocket_type(c);
    printf("here3 %d\n", p);
    if (p == SINGULAR) {
      if (extract_cards_singular(c, hand, dead) == 0) {
        return 0;
      }
    } else if (p == RAND) {
      extract_cards_random(hand, dead);
    } else if (p == PAIR) {
      if (extract_cards_pair(c, hand, dead) == 0) {
        return 0;
      }
    } else if (p == SUITED) {
      extract_cards_suited(c, hand, dead);
    } else if (p == OFFSUIT) {
      extract_cards_offsuit(c, hand, dead);
    } else if (p == NONE) {
      extract_cards_suited(c, hand, dead);
      extract_cards_offsuit(c, hand, dead);
    } else {
      return 0;
    }
    c = strtok(NULL,",");
  }
  if (hand->dist_n == 0) {
    return 0;
  }
  return 1;
}

int main(int argc, char **argv) {
  Hand* hand = (Hand*)malloc(sizeof(Hand));
  hand->dist_n = 0;
  insert(StdDeck_MASK(0), hand);
  insert(StdDeck_MASK(5), hand);
  insert(StdDeck_MASK(6), hand);
  Hand_Dist* c = hand->hand_dist;
  for (int i=0; i<3; i++) {
    DprintMask(StdDeck, c->cards);
    printf("\n");
    c = c->next;
  }
  free_hand(hand);
  hand = (Hand*)malloc(sizeof(Hand));
  hand->dist_n = 0;
  StdDeck_CardMask dead;
  StdDeck_CardMask_RESET(dead);
  if (parse_pocket(*(argv+1), hand, dead) == 1) {
    printf("hhar: %d\n",hand->dist_n);
    c = hand->hand_dist;
    for (int i=0; i<hand->dist_n; i++) {
      DprintMask(StdDeck, c->cards);
      printf("\n");
      c = c->next;
    }
  }
  return 0;
}
