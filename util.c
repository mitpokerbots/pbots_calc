#include "util.h"

Hand* create_hand(void) {
  Hand* hand = (Hand*)malloc(sizeof(Hand));
  hand->dist_n = 0;
  hand->randomized = 0;
  hand->ev = 0.0;
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
void insert_hand_dist(Hand* hand, Hand_Dist* h) {
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
  insert_hand_dist(hand, new);
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
    printf("%s: ", c->hand->text);
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
  free(hand->text);
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
