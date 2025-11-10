#ifndef CARDS_H
#define CARDS_H

#include <iostream>
#include <string>
#include <vector>
#include <cstddef>

class Player;
class Orders;
class OrdersList;
class Hand;  



//enum class to store the different types of cards
enum class cardType{Bomb, Reinforcement, Blockade, Airlift, Diplomacy };

class Card{
private:

    cardType* type;

     Orders* toOrder(Player& p) const; //for Orders List Part 3


public:
    //Rule of three is being applied here
     Card(cardType t);
    Card(const Card& other);           //Copy Constructor
    Card& operator=(const Card& other);     //Assignment Operator
    ~Card();    //Deconstructor

    void play(Player& p, class Deck& d, class Hand& h); //create an order, add it to orders list for a player, remove from hand and return to deck
    cardType getType() const;

     friend std::ostream& operator<<(std::ostream& os, const Card& c);
};

class Deck{
private:
    std::vector<Card*>* cards_;

public:
    Deck();
    Deck(const Deck& other);    //Copy Constructor
    Deck(const std::vector<Card*>& cards);
    Deck& operator=(const Deck& other);     //Assignment Operator
    ~Deck();    //Deconstructor

    Card* draw(Hand& targetHand);       //removes one random card from the deck and adds it into the target hand
    void addBack(Card* C);              //Returns a played card back into the deck
    size_t size() const;

    friend std::ostream& operator<<(std::ostream& os, const Deck& d);
};
class Hand{
private:
    std::vector<Card*>* cards_;


public:
    Hand();
    Hand(const Hand& other);    //Copy Constructor
    Hand& operator=(const Hand& other);     //Assignment Operator
    ~Hand();    //Deconstructor

    void add(Card* c);      //takes ownership
    bool remove(Card* c);       //releases ownership when a card is played
    const std::vector<Card*>* cards() const;        //read-only access for testing

    friend std::ostream& operator<<(std::ostream& os, const Hand& h);
};

#endif