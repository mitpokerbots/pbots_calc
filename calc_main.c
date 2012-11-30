#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

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
  int randomized;
} Hand;

typedef struct hand_ll hand_ll;
typedef struct hand_ll {
  Hand* hand;
  hand_ll* next;
  hand_ll* prev;
} Hand_List;

typedef struct {
  Hand_List* hands;
  int size;
} Hands;

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

Hand* create_hand(void) {
  Hand* hand = (Hand*)malloc(sizeof(Hand));
  hand->dist_n = 0;
  hand->randomized = 0;
  return hand;
}

Hands* create_hands(void) {
  Hands* hands = (Hands*)malloc(sizeof(Hands));
  hands->size = 0;
  return hands;
}

void insert_hand(Hands* hands, Hand* hand) {
  Hand_List* h = (Hand_List*) malloc(sizeof(Hand_List));
  h->hand = hand;
  if (hands->size == 0) {
    h->next = h;
    h->prev = h;
    hands->hands = h;
  } else {
    // set my refs
    h->next = hands->hands;
    h->prev = hands->hands->prev;
    // update other refs
    h->prev->next = h;
    hands->hands->prev = h;
  }
  hands->size += 1;
}

// insert entry into linked list at "tail", as in right before entry pointed to
// by hand.
void insert(Hand* hand, Hand_Dist* h) {
  if (hand->dist_n < 1) {
    // first entry, needs to be self-referencing
    h->next = h;
    h->prev = h;
    hand->hand_dist = h;
  } else {
    Hand_Dist* cur = hand->hand_dist;
    // ensure no duplicate entries
    int i;
    for (i=0; i<hand->dist_n; i++) {
      if (StdDeck_CardMask_EQUAL(h->cards, cur->cards)) {
        return;
      }
      cur = cur->next;
    }
    // set my refs
    h->next = hand->hand_dist;
    h->prev = hand->hand_dist->prev;
    // update other refs
    h->prev->next = h;
    hand->hand_dist->prev = h;
  }
  // incr counter
  hand->dist_n++;
}

// create and insert new hand_distribution entry of given cards
void insert_new(StdDeck_CardMask cards, Hand* hand) {
  Hand_Dist* new = (Hand_Dist*)malloc(sizeof(Hand_Dist));
  new->cards = cards;
  insert(hand, new);
}

void remove_hd(Hand_Dist* h) {
  // update pointers from other nodes
  h->next->prev = h->prev;
  h->prev->next = h->next;
  // make this operation idemnipotent
  // update node pointers, making self-referencing
  h->next = h;
  h->prev = h;
}

void remove_and_free(Hand_Dist* h) {
  remove_hd(h);
  free(h);
}

// given a hand, generate a random permutation, IN PLACE.
// Conceptually, this is done by randomly creating a new linked list from the
// nodes of the current linked list by iterating hand->dist_n times, selecting a
// random node from the current linked list and removing it and inserting into
// the new list.
void randomize(Hand* hand) {
  Hand_Dist* cur_h = hand->hand_dist;
  Hand_Dist* next_h;

  // perform operation in place, so start by tricking hand into thinking it's
  // empty
  int j = hand->dist_n;
  hand->dist_n = 0;

  int i;
  for (; j>0; j--) {
    for (i=rand()%j; i>=0; i--) {
      cur_h = cur_h->next;
    }
    // keep reference to "old" linked_list
    next_h = cur_h->next;
    remove_hd(cur_h);
    insert(hand, cur_h);
    // move back to "old" linked_list and continue
    cur_h = next_h;
  }
  hand->randomized = 1;
}

void choose(Hand* hand, StdDeck_CardMask* cards) {
  Hand_Dist* cur_h = hand->hand_dist;
  int i;
  // Might consider using
  // http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/SFMT/index.html instead
  // of built in rand...
  for (i=rand()%hand->dist_n; i>=0; i--) {
    cur_h = cur_h->next;
  }
  hand->hand_dist = cur_h->next;
  *cards = cur_h->cards;
}

int choose_D(Hand* hand, StdDeck_CardMask dead, StdDeck_CardMask* cards) {
  int i;
  for (i=0; i<10; i++) {
    choose(hand, cards);
    if (!StdDeck_CardMask_ANY_SET(dead, *cards)) {
      return 1;
    }
  }
  StdDeck_CardMask_RESET(*cards);
  return 0;
}

void print_hand_dist(Hand* hand) {
  printf("HD(%d) [(",hand->dist_n);
  Hand_Dist* c = hand->hand_dist;
  int i;
  for (i=0; i<hand->dist_n-1; i++) {
    DprintMask(StdDeck, c->cards);
    printf("),(");
    c = c->next;
  }
  DprintMask(StdDeck, c->cards);
  printf(")]\n");
}

void print_hands(Hands* hands) {
  Hand_List* c = hands->hands;
  int i;
  for (i=0; i<hands->size; i++) {
    print_hand_dist(c->hand);
    c = c->next;
  }
}

void free_hand(Hand* hand) {
  Hand_Dist* next;
  Hand_Dist* cur = hand->hand_dist;
  int i;
  for (i=0; i<hand->dist_n; i++) {
    next = cur->next;
    //printf("freeing %d\n", next->cards);
    free(cur);
    cur = next;
  }
  free(hand);
}

void free_hands(Hands* hands) {
  Hand_List* next;
  Hand_List* cur = hands->hands;
  int i;
  for (i=0; i<hands->size; i++) {
    next = cur->next;
    //printf("freeing");print_hand_dist(cur->hand)
    free_hand(cur->hand);
    free(cur);
    cur = next;
  }
  free(hands);
}

pocket_type get_pocket_type(const char* pocket) {
  printf("strlen: %d\n", (int)strlen(pocket));
  if (strchr("x", tolower(pocket[0])) != NULL)
    return RAND;
  size_t n_p = strlen(pocket);
  // filter out blatant errors in input
  int i;
  for (i=0; i<n_p; i++) {
    if (strchr("23456789TJQKA+-SHDCO", toupper(pocket[i])) == NULL)
      return ERROR;
  }
  if (n_p < 2)
    return ERROR;

  if (strchr("23456789TJQKA", toupper(pocket[0])) == NULL)
    return ERROR;
  if (n_p == 4) {
    if (strchr("SHDC", toupper(pocket[1])) != NULL &&
        strchr("SHDC", toupper(pocket[3])) != NULL &&
        strchr("23456789TJQKA", toupper(pocket[2])) != NULL) {
      if (pocket[0] == pocket[2] && pocket[1] == pocket[3]) {
        return ERROR;
      } else {
        return SINGULAR;
      }
    }
  }
  // final sanity check for remaining cases
  if (strchr("23456789TJQKA", toupper(pocket[1])) == NULL)
    return ERROR;
  if (n_p >= 2 && pocket[0] == pocket[1]) {
    if (n_p == 2 ||
        (n_p == 3 && pocket[2] == '+') ||
        (n_p == 5 && pocket[2] == '-' &&
         strchr("23456789TJQKA", toupper(pocket[3])) != NULL &&
         pocket[3] == pocket[4])) {
      return PAIR;
    }
    return ERROR;
  }
  if (n_p == 2)
    return NONE;
  if (n_p >= 3) {
    if (n_p == 3 ||
        (n_p == 4 && pocket[3] == '+') ||
        (n_p == 7 && pocket[3] == '-' &&
         strchr("23456789TJQKA", toupper(pocket[4])) != NULL &&
         strchr("23456789TJQKA", toupper(pocket[5])) != NULL &&
         pocket[2] == pocket[6] &&
         (pocket[0] == pocket[4] ||
          pocket[1] == pocket[5]))) {
      if (pocket[2] == 's')
        return SUITED;
      if (pocket[2] == 'o')
        return OFFSUIT;
    }
  }

  return ERROR;
}

int extract_cards_singular(char* cards, Hand* hand, StdDeck_CardMask dead) {
  StdDeck_CardMask pocket;
  StdDeck_CardMask_RESET(pocket);
  int card;

  if (!DstringToCard(StdDeck, cards, &card)) {
    printf("R1: parsing card1 failed\n");
    return 0;
  }
  StdDeck_CardMask_SET(pocket, card);
  if (!DstringToCard(StdDeck, cards+2, &card)) {
    printf("R1: parsing card2 failed\n");
    return 0;
  }
  StdDeck_CardMask_SET(pocket, card);
  if (StdDeck_CardMask_ANY_SET(dead, pocket)) {
    printf("R1: in dead cards\n");
    return 1;
  }
  insert_new(pocket, hand);
  return 1;
}

void extract_cards_random(Hand* hand, StdDeck_CardMask dead) {
  StdDeck_CardMask curHand;
  DECK_ENUMERATE_2_CARDS_D(StdDeck, curHand, dead, insert_new(curHand, hand););
}

int extract_cards_pair(char* cards, Hand* hand, StdDeck_CardMask dead) {
  // extract limits to possible ranges
  int ceil = char2rank(cards[0]);
  int floor = char2rank(cards[0]);
  if (strchr(cards, '-') != NULL) {
    if ((floor = char2rank(cards[3])) < 0) {
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
  if ( floor < 0 || ceil < 0 ) {
    return 0;
  }

  StdDeck_CardMask pocket;
  // enumerate all cards in range
  int rank, suit1, suit2;
  for (rank=floor; rank <= ceil; rank++) {
    for(suit1 = StdDeck_Suit_FIRST; suit1 <= StdDeck_Suit_LAST; suit1++) {
      for (suit2 = suit1 + 1; suit2 <= StdDeck_Suit_LAST; suit2++) {
        StdDeck_CardMask_RESET(pocket);
        StdDeck_CardMask_SET(pocket, StdDeck_MAKE_CARD(rank, suit1) );
        StdDeck_CardMask_SET(pocket, StdDeck_MAKE_CARD(rank, suit2) );
        if (!StdDeck_CardMask_ANY_SET(dead, pocket)) {
          insert_new(pocket, hand);
        }
      }
    }
  }
  return 1;
}

// Helper function to extract bounds for use in extract_suited and offsuit
int extract_bounds(char* cards, int* floor, int* ceil, int* rank) {
  // highest card is always fixed, must appear in both limits (if range)
  int rank1 = char2rank(cards[0]);
  int rank2 = char2rank(cards[1]);
  if (rank1 < 0 || rank2 < 0) {
    return 0;
  }
  int high_index = 0;
  if (rank2 > rank1) {
    high_index = 1;
    rank1 = rank2;
  }

  // extract limits to possible ranges
  *floor = char2rank(cards[1-high_index]);
  *ceil = *floor;
  if (strchr(cards, '-') != NULL) {
    const char* index = strchr(cards, '-');
    *floor = char2rank(*((index+1)+(1-high_index)));
    // if inverse range specified, just flip it
    if (*floor > *ceil) {
      int temp = *floor;
      *floor = *ceil;
      *ceil = temp;
    }
  } else if (strchr(cards, '+') != NULL) {
    *ceil = StdDeck_Rank_ACE;
  }
  /* what does this do?
  if (*ceil >= rank1) {
    *ceil = rank1-1;
  }
  */
  if ( *floor < 0 || *ceil < 0 ) {
    return 0;
  }
  *rank = rank1;
  return 1;
}
int extract_cards_suited(char* cards, Hand* hand, StdDeck_CardMask dead) {
  int floor, ceil, rank1;
  if (!extract_bounds(cards, &floor, &ceil, &rank1)) {
    return 0;
  }

  StdDeck_CardMask pocket;
  // enumerate all cards in range
  int rank2, suit;
  for (rank2=floor; rank2 <= ceil; rank2++) {
    for(suit = StdDeck_Suit_FIRST; suit <= StdDeck_Suit_LAST; suit++) {
      if (rank1 == rank2)
        continue;
      StdDeck_CardMask_RESET(pocket);
      StdDeck_CardMask_SET(pocket, StdDeck_MAKE_CARD(rank1, suit) );
      StdDeck_CardMask_SET(pocket, StdDeck_MAKE_CARD(rank2, suit) );
      if (!StdDeck_CardMask_ANY_SET(dead, pocket)) {
        insert_new(pocket, hand);
      }
    }
  }

  return 1;
}

int extract_cards_offsuit(char* cards, Hand* hand, StdDeck_CardMask dead) {
  int floor, ceil, rank1;
  if (!extract_bounds(cards, &floor, &ceil, &rank1)) {
    return 0;
  }

  StdDeck_CardMask pocket;
  // enumerate all cards in range
  int rank2, suit1, suit2;
  for (rank2=floor; rank2 <= ceil; rank2++) {
    for(suit1 = StdDeck_Suit_FIRST; suit1 <= StdDeck_Suit_LAST; suit1++) {
      for(suit2 = StdDeck_Suit_FIRST; suit2 <= StdDeck_Suit_LAST; suit2++) {
        if (suit1 == suit2)
          continue;
        StdDeck_CardMask_RESET(pocket);
        StdDeck_CardMask_SET(pocket, StdDeck_MAKE_CARD(rank1, suit1) );
        StdDeck_CardMask_SET(pocket, StdDeck_MAKE_CARD(rank2, suit2) );
        if (!StdDeck_CardMask_ANY_SET(dead, pocket)) {
          insert_new(pocket, hand);
        }
      }
    }
  }

  return 1;
}

Hand* parse_pocket(const char* hand_text, StdDeck_CardMask dead) {
  char* hand_text_copy = strdup(hand_text);
  printf("parse_pocket %s\n",hand_text);
  Hand* hand = create_hand();
  int err = 0;
  char *str, *token, *str_save_ptr;
  for (str = hand_text_copy; ; str=NULL) {
    token = strtok_r(str, ",", &str_save_ptr);
    if (token == NULL)
      break;
    printf("here2: %s\n", token);
    pocket_type p = get_pocket_type(token);
    printf("here3 %d\n", p);
    if (p == SINGULAR) {
      if (!extract_cards_singular(token, hand, dead)) {
        err = 1;
        break;
      }
    } else if (p == RAND) {
      extract_cards_random(hand, dead);
    } else if (p == PAIR) {
      if (!extract_cards_pair(token, hand, dead)) {
        err = 1;
        break;
      }
    } else if (p == SUITED) {
      extract_cards_suited(token, hand, dead);
    } else if (p == OFFSUIT) {
      extract_cards_offsuit(token, hand, dead);
    } else if (p == NONE) {
      extract_cards_suited(token, hand, dead);
      extract_cards_offsuit(token, hand, dead);
    } else {
      err = 1;
      break;
    }
  }
  free(hand_text_copy);
  if (hand->dist_n == 0 || err) {
    free_hand(hand);
    return NULL;
  }
  return hand;
}

int extract_single_cards(char* cards_str, StdDeck_CardMask* cards) {
  StdDeck_CardMask_RESET(*cards);
  int i;
  int card;
  for (i=0; i<strlen(cards_str); i+=2) {
    if (!DstringToCard(StdDeck, cards_str+i, &card)) {
      printf("R1: parsing card1 failed\n");
      return 0;
    }
    StdDeck_CardMask_SET(*cards, card);
  }
  return 1;
}

Hands* get_hands(const char* hand_str, StdDeck_CardMask* dead) {
  Hands* hands = create_hands();
  Hand* hand;

  // get all singular (non-ranged) hands
  char* hand_str_copy = strdup(hand_str);
  printf("get_hands:singular:%s\n", hand_str_copy);
  int err = 0;
  char *str, *token, *str_save_ptr;
  for (str = hand_str_copy; ; str=NULL) {
    token = strtok_r(str, ":", &str_save_ptr);
    if (token == NULL)
      break;
    pocket_type p = get_pocket_type(token);
    // TODO: There are cases where range is specified, but because of dead
    // cards, it is reduced to a single possible hand. However, we can't know
    // this until we call parse_pockets...
    if (p == SINGULAR) {
      if ((hand = parse_pocket(token, *dead)) == NULL) {
        printf("calc: Improperly formatted singular hand %s\n",token);
        err = 1;
        break;
      } else {
        if (hand->dist_n != 1) {
          printf("calc: Found multiple pockets when only one was specified? Saw %s but got ",
                 token);
          print_hand_dist(hand);
          err = 1;
          break;
        } else {
          insert_hand(hands, hand);
          // Since we know which cards this hand is, add them to dead cards
          StdDeck_CardMask_OR(*dead, *dead, hand->hand_dist->cards);
        }
      }
    }
  }
  free(hand_str_copy);
  if (err) {
    free_hands(hands);
    return NULL;
  }

  // get remaining hands
  char* hand_str_copy2 = strdup(hand_str);
  printf("get_hands:ranged (%d) %s\n", (int)strlen(hand_str_copy2), hand_str_copy2);
  char *str_save_ptr2;
  for (str = hand_str_copy2; ; str=NULL) {
    token = strtok_r(str, ":", &str_save_ptr2);
    if (token == NULL)
      break;
    pocket_type p = get_pocket_type(token);
    if (p != SINGULAR) {
      if ((hand = parse_pocket(token, *dead)) == NULL) {
        printf("calc: Improperly formatted ranged hand %s\n",token);
        err = 1;
        break;
      } else {
        insert_hand(hands, hand);
      }
    }
  }
  free(hand_str_copy2);
  if (err) {
    free_hands(hands);
    return NULL;
  }

  return hands;
}

int calc(const char* hand_str, char* board_str, char* dead_str, int iters) {
  StdDeck_CardMask board, dead;
  if (!extract_single_cards(dead_str, &dead)) {
    printf("calc: Improperly formatted dead cards %s\n", dead_str);
    return 0;
  }
  if (!extract_single_cards(board_str, &board)) {
    printf("calc: Improperly formatted board cards %s\n", board_str);
    return 0;
  }
  StdDeck_CardMask_OR(dead, dead, board);

  Hands* hands;
  if ((hands = get_hands(hand_str, &dead)) == NULL) {
    return 0;
  }
  print_hands(hands);
  printf("board: ");
  DprintMask(StdDeck, board);
  printf("\ndead: ");
  DprintMask(StdDeck, dead);
  printf("\n");
  return 1;
}

int main(int argc, char **argv) {
  srand(time(NULL));
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
  calc(argv[1], board, dead, 10);
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
