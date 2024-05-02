#include "Card.h"

void Card::generate_suit() {
    int RanNum = rand() % 40 + -20;

        if (RanNum > 0) {
                if (RanNum % 2 == 0) {
                    card_suit = Hearts;
                }
                else {
                    card_suit = Spades;
                }
        } 
        else {
            if (RanNum % 2 == 0) {
                card_suit = Diamonds;
            } 
            else {
                card_suit = Clubs;
            }
        }
    }        


Suit Card::get_suit() {
    return card_suit;
}

void Card::set_suit(Suit suit) {
    card_suit = suit;
}