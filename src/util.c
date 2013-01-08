#include <stdio.h>

#include "util.h"

Hand* create_hand(void) {
  Hand* hand = (Hand*)malloc(sizeof(Hand));
  hand->dist_n = 0;
  hand->randomized = 0;
  hand->ev = 0.0;
  return hand;
}

Hands* create_hands(int nhands) {
  Hands* hands = (Hands*)malloc(sizeof(Hands));
  hands->size = 0;
  hands->e_size = nhands;
  hands->hand_ptrs = (Hand_Dist_Ptr**)malloc(sizeof(Hand_Dist_Ptr*) * nhands);
  return hands;
}

void insert_hand(Hands* hands, Hand* hand) {
  Hand_List* h = (Hand_List*) malloc(sizeof(Hand_List));
  Hand_Dist_Ptr* hdp;
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
  hdp = (Hand_Dist_Ptr*) malloc(sizeof(Hand_Dist_Ptr));
  hdp->hand_dist = hand->hand_dist;
  hdp->start = hand->hand_dist;
  hdp->hand = hand;
  hands->hand_ptrs[hands->size] = hdp;
  hands->size++;
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
  Hand_Dist* new_hand_dist = (Hand_Dist*)malloc(sizeof(Hand_Dist));
  new_hand_dist->cards = cards;
  insert_hand_dist(hand, new_hand_dist);
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
  Hand_Dist* c = hand->hand_dist;
  int i;
  printf("HD(%d) [(",hand->dist_n);
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
  //printf("freeing %s\n", hand->text);
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

    free(hands->hand_ptrs[i]);
  }
  free(hands->hand_ptrs);
  free(hands);
}

void incr_hand_ptr(Hands* hands, int hand_index) {
  if (hand_index < hands->size) {
    //printf("incrementing pointer index %d\n",hand_index);
    Hand_Dist_Ptr* ptr=hands->hand_ptrs[hand_index];
    ptr->hand_dist = ptr->hand_dist->next;
    if (ptr->hand_dist == ptr->start) {
      incr_hand_ptr(hands, hand_index+1);
    }
  }
}

int ptr_iter_terminated(Hands* hands) {
  int i;
  for (i=0; i<hands->size; i++) {
    Hand_Dist_Ptr* ptr=hands->hand_ptrs[i];
    if (ptr->hand_dist != ptr->start)
      return 0;
  }
  return 1;
}

int get_hand_set(Hands* hands, StdDeck_CardMask* dead, StdDeck_CardMask* pockets) {
  int i;
  for (i=0; i<hands->size; i++) {
    pockets[i] = hands->hand_ptrs[i]->hand_dist->cards;
    // for singleton hands, we've already added them to the dead cards mask
    if (hands->hand_ptrs[i]->hand->dist_n > 1) {
      if (StdDeck_CardMask_ANY_SET(*dead, pockets[i])) {
        StdDeck_CardMask_RESET(pockets[i]);
        return 0;
      }
      StdDeck_CardMask_OR(*dead, *dead, pockets[i]);
    }
  }
  return 1;
}

int get_next_set(Hands* hands, StdDeck_CardMask* dead, StdDeck_CardMask* pockets) {
  StdDeck_CardMask dead_temp = *dead;
  for (*dead = dead_temp; !get_hand_set(hands, dead, pockets); *dead = dead_temp) {
    incr_hand_ptr(hands, 0);
    if (ptr_iter_terminated(hands)) {
      // we've iterated over entire set of possible hands
      return 0;
    }
    //*dead = dead_temp;
  }
  return 1;
}
