#include "Card.h"

void Card::set_suit() {
    srand((int)time(0));
    int RanNum = rand() % 100 + -50;


    if (RanNum > 0) {
                if (RanNum % 2 == 0) {
                    card_suit = Hearts;
                } else {
                    card_suit = Spades;
                }
            } else {
                if (RanNum % 2 == 0) {
                    card_suit = Diamonds;
                } else {
                    card_suit = Clubs;
                }
            }
}

Suit Card::get_suit() {
    return card_suit;
}