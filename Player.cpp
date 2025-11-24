#include <iostream>
#include <vector>
#include "Map.h"
#include "Player.h"
#include "Cards.h"
#include "Orders.h"
#include <string>

// ================= Constructors & Destructor =================

// default constructor
Player::Player() {
    pName = new std::string;
    Pterritories = new std::vector<Territory*>;
    deck = new Deck;              // allocate Deck on heap
    order = new OrdersList;       // allocate OrdersList on heap
}

// parameterized constructor
Player::Player(std::string pName1, std::vector<Territory*> t1, Deck* d1, OrdersList* o1) {
    this->pName = new std::string(pName1);
    this->Pterritories = new std::vector<Territory*>(t1);
    this->deck = d1;       // use provided Deck pointer
    this->order = o1;      // use provided OrdersList pointer
}

// copy constructor
Player::Player(const Player& other) {
    pName = new std::string(*other.pName);
    Pterritories = new std::vector<Territory*>(*other.Pterritories);
    deck = new Deck(*other.deck);
    order = new OrdersList(*other.order);
}

// destructor
Player::~Player() {
    delete pName;
    delete Pterritories;
    delete deck;
    delete order;
}

// ================= Getters =================

// getter for player name
std::string Player::getPName() const {
    return *pName;
}

// getter for territory param
std::vector<Territory*> Player::getTerritory() const {
    return *Pterritories;
}

// getter for deck param (returns pointer)
Deck* Player::getDeck() const {
    return deck;
}

// getter for orders list param (returns pointer)
OrdersList* Player::getOrder() const {
    return order;
}

bool* Player::getOrderlVal() const{
    return orderVal;
}

// ================= Setters =================

// setter for player name
void Player::setPName(std::string pName) {
    *this->pName = pName;
}

// setter for territory
void Player::setTerritory(std::vector<Territory*> Pterritories) {
    *this->Pterritories = Pterritories;
}

// setter for deck
void Player::setDeck(Deck* deck) {
    *(this->deck) = *deck;   // deep copy contents of provided deck
}

// setter for orders list
void Player::setOrdersList(OrdersList* order) {
    *(this->order) = *order; // deep copy contents of provided orders list
}

void Player::setOrderVal(bool* b){
    *(this->orderVal) = *b;
}

// ================= Gameplay Methods =================

// toDefend method that returns a list of territories to defend
std::vector<Territory*> Player::toDefend(Player p) {
    // get territory
    std::vector<Territory*> t2 = p.getTerritory();

    // create list to store territories that will be defended
    std::vector<Territory*> defend;

    // goes through list of territories belonging to player
    for (int i = 0; i < t2.size(); i++) {
        // store owner name
        std::string owName = t2[i]->getOwner();

        // selects the territories to defend based on if they belong to the player
        if (owName == p.getPName()) {
            // add the territory to the player defend list
            defend.push_back(t2[i]);
        }
    }
    // return the list of defending territories
    return defend;
}

// toAttack method that returns a list of territories to attack
std::vector<Territory*> Player::toAttack(Player p) {
    // get territory
    std::vector<Territory*> t2 = p.getTerritory();

    // create list to store territories to attack
    std::vector<Territory*> attack;

    // goes through list of territories belonging to other players
    for (int i = 0; i < t2.size(); i++) {
        // store owner name
        std::string owName = t2[i]->getOwner();

        // selects the territories to attack based on if they belong to the player or not
        if (owName != p.getPName()) {
            // add the territory to the player attack list
            attack.push_back(t2[i]);
        }
    }
    // return the list of attacking territories
    return attack;
}

int Player::getReinforcementPool() const { return *reinforcementPool; }
void Player::addReinforcements(int n){ if(n>0) *reinforcementPool += n; }
void Player::useReinforcements(int n){ if(n>0){ *reinforcementPool -= n; if(*reinforcementPool<0)*reinforcementPool=0; } }
bool Player::hasTerritories() const { return !Pterritories->empty(); }

void Player::issueOrder() {
    if (!hasTerritories()) return;
    if (*reinforcementPool > 0) {
        int amount = 1; // simple RR deploy
        if (*reinforcementPool < amount) amount = *reinforcementPool;
        *reinforcementPool -= amount;

        Territory* target = Pterritories->front();
        auto* n = new int(amount);
        Orders* o = new Deploy(this, target, n);
        order->add(o);
        std::cout << "[issueOrder] " << *pName << " Deploy " << amount
                  << " to " << target->getName()
                  << " (pool=" << *reinforcementPool << ")\n";
    }
}
