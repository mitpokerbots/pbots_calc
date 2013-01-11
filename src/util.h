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
  double ev;
  char* text;
  // How many combinations of 2-card hands are contained in this hand? (either 1
  // or 3)
  int coms;
} Hand;

// For exhaustive enumeration, we use this meta-pointer to keep
// track of the next set of hand to try in a given hand range.
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

int char2rank(char);
int char2suit(char c);

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
