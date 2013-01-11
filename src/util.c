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

#include "util.h"

int char2rank(char c) {
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

int char2suit(char c) {
  switch(toupper(c)) {
    case 'h': return StdDeck_Suit_HEARTS;
    case 'd': return StdDeck_Suit_DIAMONDS;
    case 'c': return StdDeck_Suit_CLUBS;
    case 's': return StdDeck_Suit_SPADES;
    default: return -1;
  }
}

Hand* create_hand(void) {
  Hand* hand = malloc(sizeof(Hand));
  hand->dist_n = 0;
  hand->ev = 0.0;
  hand->coms = 1;
  return hand;
}

Hands* create_hands(int nhands) {
  Hands* hands = malloc(sizeof(Hands));
  hands->size = 0;
  hands->e_size = nhands;
  hands->hand_ptrs = malloc(sizeof(Hand_Dist_Ptr*) * nhands);
  return hands;
}

void insert_hand(Hands* hands, Hand* hand) {
  Hand_List* h = malloc(sizeof(Hand_List));
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
  hdp = malloc(sizeof(Hand_Dist_Ptr));
  hdp->hand_dist = hand->hand_dist;
  hdp->start = hand->hand_dist;
  hdp->hand = hand;
  hdp->discard_ptr = hand->coms;
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
  Hand_Dist* new_hand_dist = malloc(sizeof(Hand_Dist));
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

// Here's where the confusing magic trickery happens for exhaustive enumeration.
// Our method is to maintain a iterator over all the hand distributions,
// implemented as a set of meta-pointers into each of the hand
// distributions. This way, we only need to recurse to increment this "counter",
// and we can perform an almost-normal looking iteration for the high-level
// enumeration code.

void incr_hand_ptr_r(Hands* hands, int hand_index) {
  if (hand_index < hands->size) {
    //printf("incrementing pointer index %d\n",hand_index);
    Hand_Dist_Ptr* ptr=hands->hand_ptrs[hand_index];
    if (--ptr->discard_ptr <= 0) {
      ptr->discard_ptr = ptr->hand->coms;
      ptr->hand_dist = ptr->hand_dist->next;
      if (ptr->hand_dist == ptr->start) {
        incr_hand_ptr_r(hands, hand_index+1);
      }
    }
  }
}

void incr_hand_ptr(Hands* hands) {
  incr_hand_ptr_r(hands, 0);
}

// Since the counter (ptr) is not really well-defined, it's a bit complicated to
// define when we've reached the "end" of our enumeration. We define it as once
// we've looped back around to the very first set of hands we enumerated, which
// is our "0" value for the counter.
int ptr_iter_terminated(Hands* hands) {
  int i;
  for (i=0; i<hands->size; i++) {
    Hand_Dist_Ptr* ptr=hands->hand_ptrs[i];
    if (ptr->hand_dist != ptr->start)
      return 0;
    else if (ptr->discard_ptr != ptr->hand->coms)
      return 0;
  }
  return 1;
}

// discard (unset) specified card from given hand
void discard_card(StdDeck_CardMask* hand, int discard_index) {
  int i;
  for (i=0; i<StdDeck_N_CARDS && discard_index > 0; i++) {
    if (StdDeck_CardMask_CARD_IS_SET(*hand, i)) {
      discard_index--;
    }
  }
  //printf("discarding %s\n", StdDeck_cardString(i-1));
  StdDeck_CardMask_UNSET(*hand, i-1);
}

// Retrieve the set of pocket cards corresponding to the current value of the
// counter/pointer.
int get_hand_set(Hands* hands, StdDeck_CardMask* dead, StdDeck_CardMask* pockets) {
  int i;
  for (i=0; i<hands->size; i++) {
    StdDeck_CardMask_RESET(pockets[i]);
    pockets[i] = hands->hand_ptrs[i]->hand_dist->cards;
    // for singleton hands, we've already added them to the dead cards mask
    if (hands->hand_ptrs[i]->hand->dist_n > 1) {
      if (StdDeck_CardMask_ANY_SET(*dead, pockets[i])) {
        StdDeck_CardMask_RESET(pockets[i]);
        return 0;
      }
      StdDeck_CardMask_OR(*dead, *dead, pockets[i]);
    }
    if (StdDeck_numCards(hands->hand_ptrs[i]->start->cards) > 2) {
      // we treat all 3 cards as dead, but the player can only use 2 of them.
      discard_card(pockets+i, hands->hand_ptrs[i]->discard_ptr);
    }
  }
  return 1;
}

int get_next_set(Hands* hands, StdDeck_CardMask* dead, StdDeck_CardMask* pockets) {
  StdDeck_CardMask dead_temp = *dead;

  // iterate until we get a possible hand (taking into account dead cards)
  // or we reach the end of the set of hands.
  while (!get_hand_set(hands, dead, pockets)) {
    incr_hand_ptr(hands);
    if (ptr_iter_terminated(hands)) {
      // we've iterated over entire set of possible hands
      return 0;
    }
    *dead = dead_temp;
  }
  return 1;
}
