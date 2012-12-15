#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#include "util.h"


//#include "inlines\eval.h"


typedef enum {
  ERROR,
  SINGULAR,
  RAND,
  PAIR,
  SUITED,
  OFFSUIT,
  NONE
} pocket_type;

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
    insert_hand_dist(hand, cur_h);
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
  StdDeck_CardMask_RESET(*cards);
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
  hand->text = strdup(hand_text);
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

static inline int count_chars(const char* string, char *ch)
{
  int count = 0;
  for(; *string; count += (*string++ == *ch)) ;
  return count;
}

Hands* get_hands(const char* hand_str, StdDeck_CardMask* dead) {
  int nhands = count_chars(hand_str, ":")+1;
  if (nhands < 2) {
    printf("error: need to specify more than one hand!\n");
    return NULL;
  }

  // Extract hand strings
  char* hand_str_copy = strdup(hand_str);
  char* hand_strings[nhands];
  int i;
  char *str, *token, *str_save_ptr;
  for (i=0, str = hand_str_copy; ; str=NULL, i++) {
    token = strtok_r(str, ":", &str_save_ptr);
    if (token == NULL)
      break;
    hand_strings[i] = strdup(token);
  }
  free(hand_str_copy);
  if (i != nhands) {
    printf("huh? nhands=%d, i=%d\n",nhands, i);
    for (i=0; i<nhands; i++) {
      free(hand_strings[i]);
    }
    return NULL;
  }

  Hands* hands = create_hands(nhands);
  Hand* hand[nhands];

  // get all singular (non-ranged) hands
  printf("get_hands:singular:%s\n", hand_str_copy);
  for (i=0; i<nhands; i++) {
    pocket_type p = get_pocket_type(hand_strings[i]);
    // TODO: There are cases where range is specified, but because of dead
    // cards, it is reduced to a single possible hand. However, we can't know
    // this until we call parse_pockets...
    if (p == SINGULAR) {
      if ((hand[i] = parse_pocket(hand_strings[i], *dead)) == NULL) {
        printf("calc: Improperly formatted singular hand %s\n",hand_strings[i]);
        goto error;
      } else if (hand[i]->dist_n != 1) {
        printf("calc: Found multiple pockets when only one was specified? Saw %s but got ",
               hand_strings[i]);
        print_hand_dist(hand[i]);
        goto error;
      } else {
        // Since we know which cards this hand is, add them to dead cards
        StdDeck_CardMask_OR(*dead, *dead, hand[i]->hand_dist->cards);
        // we don't insert hands until later, to preserve ordering...
      }
    }
  }

  // get remaining hands
  for (i=0; i<nhands; i++) {
    pocket_type p = get_pocket_type(hand_strings[i]);
    if (p != SINGULAR) {
      if ((hand[i] = parse_pocket(hand_strings[i], *dead)) == NULL) {
        printf("calc: Improperly formatted ranged hand %s\n",hand_strings[i]);
        goto error;
      }
    }
    // insert all hands in order, so we can report back to the user accurately.
    insert_hand(hands, hand[i]);
  }

  if (hands->e_size != nhands) {
    printf("calc: Strange, didn't parse correct number of hands (%d!=%d)!\n",
           hands->e_size, nhands);
    goto error;
  }

  return hands;

error:
  for (i=0; i<nhands; i++) {
    free(hand_strings[i]);
  }
  free_hands(hands);
  return NULL;
}

// record intermediate results
void accumulate_results(Hand_List* h, enum_result_t *result) {
  int i;
  Hand_List* cur = h;
  for (i=0; i<result->nplayers; i++) {
    cur->hand->ev += result->ev[i];
    cur = cur->next;
  }
}

void print_results(Hands* hands, int iters) {
  int i;
  printf("After %d iterations, EV:\n", iters);
  Hand_List* h = hands->hands;
  for (i=0; i<hands->size; i++) {
    printf("%s: %8.6f\n", h->hand->text, h->hand->ev / iters);
    h = h->next;
  }
}

// nboard is the number of cards on the board
// ndead is the number of cards that are dead, not including those in hands or
// on the board.
unsigned long long num_outcomes_UL(Hands* hands, int nboard, int ndead) {
  unsigned long long last = 1;
  unsigned long long total = 1;
  Hand_List* h = hands->hands;
  do {
    total *= h->hand->dist_n;
    if (last > total) // overflow
      return ULLONG_MAX;
    last = total;
    h = h->next;
  } while (h != hands->hands);

  int avail_cards = 52 - (hands->size * 2) - nboard - ndead;
  int i;
  for (i=0; i<5-nboard; i++) {
    total *= avail_cards - i;
    if (last > total) // overflow
      return ULLONG_MAX;
    total /= i + 1;
    last = total;
  }

  return (total < last) ? ULLONG_MAX : total;
}

int enumerate(Hands* hands, StdDeck_CardMask dead, StdDeck_CardMask board,
               int nboard) {
  StdDeck_CardMask pockets[hands->size];
  enum_result_t result;
  int count = 0;
  StdDeck_CardMask dead_temp = dead;
  int err, i;
  //while (1) {
  //for (; !ptr_iter_terminated(hands); incr_hand_ptr(hands,0)) {
  do {
    if (!get_next_set(hands,&dead_temp,pockets)) {
      break;
    }
    /*
    printf("(%d) ",count);
    for (i=0; i<hands->size-1; i++) {
      DprintMask(StdDeck, pockets[i]);
      printf(":");
    }
    DprintMask(StdDeck, pockets[i]);
    printf("\n");
    */
    err = enumExhaustive(game_holdem, pockets, board, dead_temp, hands->size,
                         nboard, 0, &result);
    if (err) {
      printf("enumeration failed, error=%d\n",err);
      return 0;
    }
    accumulate_results(hands->hands, &result);
    count+= result.nsamples;
    dead_temp = dead;
    incr_hand_ptr(hands,0);
  } while(!ptr_iter_terminated(hands));

  /*
  while (select_next_set(hands, &dead_temp, pockets, hands->size)) {
    int i;
    printf("(%d) ",count);
    for (i=0; i<hands->size-1; i++) {
      DprintMask(StdDeck, pockets[i]);
      printf(":");
    }
    DprintMask(StdDeck, pockets[i]);
    printf("\n");

    err = enumExhaustive(game_holdem, pockets, board, dead_temp, hands->size,
                         nboard, 0, &result);
    if (err) {
      printf("enumeration failed, error=%d\n",err);
      return 0;
    }
    accumulate_results(hands->hands, &result);
    count+= result.nsamples;
    dead_temp = dead;
  }
  */
  print_results(hands, count);
  enumResultFree(&result);
  return 1;
}

int run_MC(Hands* hands, StdDeck_CardMask dead, StdDeck_CardMask board,
           int nboard, int iters) {
  StdDeck_CardMask pockets[hands->size];
  enum_result_t result;
  StdDeck_CardMask dead_temp;
  int trials = 0;
  Hand_List* h = hands->hands;
  while (trials < iters) {
    dead_temp = dead;
    int i;
    int err = 0;
    // try to choose pocket cards for this trial
    for (i=0; i<hands->size; i++) {
      if (h->hand->dist_n > 1) {
        if(!choose_D(h->hand, dead_temp, pockets+i)) {
          err=1;
          break;
        }
        StdDeck_CardMask_OR(dead_temp, dead_temp, pockets[i]);
      } else {
        pockets[i] = h->hand->hand_dist->cards;
      }
      h = h->next;
    }
    if (err) {
      //printf("encountered collision\n");
      continue;
    }
    /*
    printf("(%d) ",trials);
    for (i=0; i<hands->size-1; i++) {
      DprintMask(StdDeck, pockets[i]);
      printf(":");
    }
    DprintMask(StdDeck, pockets[i]);
    printf("\n");
    */
    if (nboard < 5) {
      //enumResultClear(&result);
      err = enumSample(game_holdem, pockets, board, dead_temp, hands->size,
                       nboard, 1, 0, &result);
    } else {
      err = enumExhaustive(game_holdem, pockets, board, dead_temp, hands->size,
                           nboard, 0, &result);
    }
    if (err) {
      printf("enumeration failed, error=%d\n",err);
      return 0;
    }
    accumulate_results(h, &result);
    //enumResultPrintTerse(&result, pockets, board);
    trials++;
    // change which hand gets selected first each loop
    h=h->next;
  }
  print_results(hands, iters);
  enumResultFree(&result);
  return 1;
}

int calc(const char* hand_str, char* board_str, char* dead_str, int iters) {
  StdDeck_CardMask board, dead;
  if (!extract_single_cards(dead_str, &dead)) {
    printf("calc: Improperly formatted dead cards %s\n", dead_str);
    return 0;
  }
  int ndead = StdDeck_numCards(dead);
  if (!extract_single_cards(board_str, &board)) {
    printf("calc: Improperly formatted board cards %s\n", board_str);
    return 0;
  }
  int nboard = StdDeck_numCards(board);
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
  unsigned long long coms = num_outcomes_UL(hands, nboard, ndead);
  printf("num_coms=%llu\n", coms);

  int err;
  if (coms > iters) {
    err = run_MC(hands, dead, board, nboard, iters);
  } else {
    err = enumerate(hands, dead, board, nboard);
  }

  free_hands(hands);

  return err;
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
  calc(argv[1], board, dead, 5000000);
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
