#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "poker_defs.h"
#include "enumdefs.h"

//#include "inlines\eval.h"

// store distribution of pocket cards in ll
typedef struct {
  StdDeck_CardMask cards;
  void* next;
  void* prev;
} ll_card;

typedef struct {
  ll_card* hand_dist;
  int dist_n;
} Hand;

typedef enum {
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
  ll_card* new = (ll_card*)malloc(sizeof(ll_card));
  new->cards = cards;
  if (hand->dist_n < 1) {
    // first entry, needs to be self-referencing
    new->next = new;
    new->prev = new;
    hand->hand_dist = new;
  } else {
    ll_card* cur = hand->hand_dist;
    // ensure no duplicate entries
    for (int i=0; i<hand->dist_n; i++) {
      if (StdDeck_CardMask_EQUAL(cards, cur->cards)) {
        return;
      }
      cur = (ll_card*)cur->next;
    }
    // set my refs
    new->next = hand->hand_dist;
    new->prev = hand->hand_dist->prev;
    // update other refs
    ((ll_card*)new->prev)->next = new;
    hand->hand_dist->prev = new;
  }
  // incr counter
  hand->dist_n++;
}

void free_hand(Hand* hand) {
  ll_card* next;
  ll_card* cur = hand->hand_dist;
  for (int i=0; i<hand->dist_n; i++) {
    next = (ll_card*)cur->next;
    //printf("freeing %d\n", next->cards);
    free(cur);
    cur = next;
  }
  free(hand);
}

pocket_type get_pocket_type(const char* pocket) {
  if (strlen(pocket) == 4) {
    if (strchr("shdc", pocket[1]) != NULL) {
      if (strchr("shdc", pocket[3]) != NULL) {
        if (strchr("23456789TJQKA", pocket[0]) != NULL) {
          if (strchr("23456789TJQKA", pocket[2]) != NULL) {
            return SINGULAR;
          }
        }
      }
    }
  }
  if (strcmp(pocket, "XxXx") == 0)
    return RAND;
  if (strlen(pocket) >= 2 && pocket[0] == pocket[1])
    return PAIR;
  if (strlen(pocket) >= 3) {
    if (pocket[2] == 's')
      return SUITED;
    if (pocket[2] == 'o')
      return OFFSUIT;
  }
  return NONE;
}

int extract_cards_singular(char* cards, Hand* hand, StdDeck_CardMask dead) {
  StdDeck_CardMask pocket;
  StdDeck_CardMask_RESET(pocket);
  int card;

  if (DstringToCard(StdDeck, cards, &card) == 0)
    return 0;
  StdDeck_CardMask_SET(pocket, card);
  if (DstringToCard(StdDeck, cards+2, &card) == 0)
    return 0;
  StdDeck_CardMask_SET(pocket, card);
  if (StdDeck_CardMask_ANY_SET(dead, pocket)) /* cards already seen */
    return 1;
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
    floor = char2rank(*(index+1));
  } else if (strchr(cards, '+') != NULL) {
    ceil = StdDeck_Rank_ACE;
  }
  if (floor < 0 || ceil < 0) {
    return 0;
  }

  StdDeck_CardMask pocket;
  StdDeck_CardMask_RESET(pocket);
  // enumerate all cards in range
  for (int rank=floor; rank <= ceil; rank++) {
    for(int suit1 = StdDeck_Suit_FIRST; suit1 <= StdDeck_Suit_LAST; suit1++) {
      for (int suit2 = suit1 + 1; suit2 <= StdDeck_Suit_LAST; suit2++) {
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

int parse_pocket(char* hand_text, Hand* hand, StdDeck_CardMask dead) {
  char *c = strtok(hand_text, ",");
  while (c != NULL) {
    pocket_type p = get_pocket_type(c);
    if (p == SINGULAR) {
      if (extract_cards_singular(c, hand, dead) == 0) {
        return 0;
      }
    } else if (p == RAND) {
      extract_cards_random(hand, dead);
    } else if (p == PAIR) {
      extract_cards_pair(c, hand, dead);
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
  ll_card* c = hand->hand_dist;
  for (int i=0; i<3; i++) {
    DprintMask(StdDeck, c->cards);
    printf("\n");
    c = (ll_card*)c->next;
  }
  free_hand(hand);
  return 0;
}
