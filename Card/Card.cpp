#include "Card.h"

Suit Card::get_suit() {
    srand((int)time(0));
    int RanNum = rand() % 40 + -20;

    Suit S;

    if (RanNum > 0) {
                if (RanNum % 2 == 0) {
                    S = Hearts;
                } else {
                    S =Spades;
                }
            } else {
                if (RanNum % 2 == 0) {
                    S = Diamonds;
                } else {
                    S = Clubs;
                }
            }
    return S;
}