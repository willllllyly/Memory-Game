#ifndef CARD_H
#define CARD_H

#include "mbed.h"

enum Suit {
    Diamonds,
    Hearts,
    Clubs,
    Spades
};

class Card {
public:  
    Suit get_suit();
    void set_suit();
private:
    Suit card_suit;
};

#endif