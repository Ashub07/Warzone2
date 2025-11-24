#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "Map.h"
#include "Cards.h"
#include "Orders.h"

// ================= Player Class =================
// Represents a single player in the game, holding their
// name, territories, deck of cards, and orders list.
// Provides methods for defending, attacking, and issuing orders.

class Player {
public:
    // ===== Constructors & Destructor =====
    Player();   // default constructor
    Player(std::string pName1, std::vector<Territory*> t1, Deck* d1, OrdersList* o1); // parameterized (now uses pointers)
    Player(const Player& other);   // copy constructor
    ~Player();   // destructor

    // ===== Getters =====
    std::string getPName() const;
    std::vector<Territory*> getTerritory() const;
    Deck* getDeck() const;             // returns pointer to Deck
    OrdersList* getOrder() const;      // returns pointer to OrdersList
    bool* getOrderVal() ;

    // ===== Setters =====
    void setPName(std::string pName);
    void setTerritory(std::vector<Territory*> Pterritories);
    void setDeck(Deck* deck);                // sets Deck contents
    void setOrdersList(OrdersList* order);   // sets OrdersList contents
    void setOrderVal(bool* b);

    // ===== Gameplay methods =====
    std::vector<Territory*> toDefend(Player p);   // territories to defend
    std::vector<Territory*> toAttack(Player p);   // territories to attack
    void issueOrder();                            // issue an order

    int  getReinforcementPool() const;
    void addReinforcements(int n);
    void useReinforcements(int n);
    bool hasTerritories() const;

private:
    // ===== Member variables =====
    std::string* pName;                          // player's name
    std::vector<Territory*>* Pterritories;       // territories owned
    Deck* deck;                                  // deck of cards
    OrdersList* order;                           // player's orders list
    int* reinforcementPool;
    bool* orderVal;
};
