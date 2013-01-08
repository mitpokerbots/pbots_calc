#ifndef __UTIL_H__
#define __UTIL_H__

#include <ctype.h>

#include <poker-eval/enumdefs.h>
#include <poker-eval/poker_defs.h>

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
  int randomized;
  double ev;
  char* text;
  // How many combinations of 2-card hands are contained in this hand? (either 1
  // or 3)
  int coms;
} Hand;

typedef struct {
  Hand_Dist* hand_dist;
  Hand_Dist* start;
  Hand* hand;
  // for 3-card hand ranges, we need to be sure to hit each of the 3 possible
  // combinations of 2 hole cards when enumerating
  int discard_ptr;
} Hand_Dist_Ptr;

typedef struct hand_ll hand_ll;
typedef struct hand_ll {
  Hand* hand;
  hand_ll* next;
  hand_ll* prev;
} Hand_List;

typedef struct {
  Hand_List* hands;
  // actual size, incremented as hands are added
  int size;
  // size we expect
  int e_size;
  // Used for enumerating all hands - deep pointer into the underlying hand
  // distributions.
  Hand_Dist_Ptr** hand_ptrs;
} Hands;

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

Hand* create_hand(void);
Hands* create_hands(int);
void insert_hand(Hands*, Hand*);;
void insert_hand_dist(Hand*, Hand_Dist*);
void insert_new(StdDeck_CardMask, Hand*);
void remove_hd(Hand_Dist*);
void remove_and_free(Hand_Dist*);
void print_hand_dist(Hand*);
void print_hands(Hands*);
void free_hand(Hand*);
void free_hands(Hands*);
void discard_card(StdDeck_CardMask*, int);
int get_next_set(Hands*, StdDeck_CardMask*, StdDeck_CardMask*);
void incr_hand_ptr(Hands*);
int ptr_iter_terminated(Hands*);

#endif
