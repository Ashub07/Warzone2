#include "Cards.h"
#include "Player.h"   // Part 2
#include "Orders.h"   // Part 3
#include <iostream>

void testCards() {
    std::cout << "=== Part 4: Cards deck/hand demo ===\n";

    // 1) Build a finite deck containing all card kinds (2× each for randomness)
    Deck deck;
    deck.addBack(new Card(cardType::Bomb));
    deck.addBack(new Card(cardType::Reinforcement));
    deck.addBack(new Card(cardType::Blockade));
    deck.addBack(new Card(cardType::Airlift));
    deck.addBack(new Card(cardType::Diplomacy));
    deck.addBack(new Card(cardType::Bomb));
    deck.addBack(new Card(cardType::Reinforcement));
    deck.addBack(new Card(cardType::Blockade));
    deck.addBack(new Card(cardType::Airlift));
    deck.addBack(new Card(cardType::Diplomacy));

    std::cout << "Initial " << deck << "\n";

    // 2) Create a player and a hand
    Player alice;
    Hand hand;

    // 3) Draw several cards at random
    for (int i = 0; i < 5; ++i) {
        Card* c = deck.draw(hand);
        if (!c) break;
        std::cout << "Drew " << *c << "\n";
    }
    std::cout << "After draws:\n" << deck << "\n" << hand << "\n";

    // 4) Play every card currently in the hand
    auto snapshot = *hand.cards(); // copy pointer list since play() mutates hand
    for (Card* c : snapshot) {
        std::cout << "Playing " << *c << "...\n";
        c->play(alice, deck, hand);
    }

    // 5) Show results: hand empty, cards back in deck, orders added
    std::cout << "After playing all cards:\n" << deck << "\n" << hand << "\n";
}


   
   