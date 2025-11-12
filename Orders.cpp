#include "Orders.h"
#include "LoggingObserver.h"
#include "Player.h"
#include "Map.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>

// ================= Orders =================
Orders::Orders() : player(nullptr) {}
Orders::Orders(Player* owner) : player(owner) {}
Orders::Orders(const Orders& other) : Subject(other), player(other.player) {}
Orders::~Orders() = default;

Orders& Orders::operator=(const Orders& other) {
    if (this != &other) player = other.player;
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Orders& order) {
    os << "Order owned by player@" << order.player;
    return os;
}

// ================= Deploy =================
Deploy::Deploy() : Orders(nullptr), targ(nullptr), armyNum(new int(0)) {}
Deploy::Deploy(Player* playr, Territory* target, int* armynum)
    : Orders(playr), targ(target), armyNum(armynum ? armynum : new int(0)) {}
Deploy::Deploy(const Deploy& o)
    : Orders(o), targ(o.targ), armyNum(new int(*o.armyNum)) {}
Deploy::~Deploy() { delete armyNum; }

Deploy& Deploy::operator=(const Deploy& o) {
    if (this != &o) {
        Orders::operator=(o);
        targ = o.targ;
        if (!armyNum) armyNum = new int(0);
        *armyNum = *o.armyNum;
    }
    return *this;
}

bool Deploy::validate() const {
    // Minimal: player owns target and has positive armies to place
    if (!player || !targ || !armyNum || *armyNum <= 0) return false;
    return (targ->getOwner() == player->getPName());
}

bool Deploy::execute() const {
    if (!validate()) return false;
    targ->setArmies(targ->getArmies() + *armyNum);
    const_cast<Deploy*>(this)->notify();
    return true;
}

std::string Deploy::stringToLog() const { return "ORDER_EXECUTED | Deploy"; }
Orders* Deploy::clone() const { return new Deploy(*this); }

// ================= Advance =================
Advance::Advance() : Orders(nullptr), targ(nullptr), source(nullptr), armyNum(new int(0)) {}
Advance::Advance(Player* playr, Territory* target, Territory* src, int* armynum)
    : Orders(playr), targ(target), source(src), armyNum(armynum ? armynum : new int(0)) {}
Advance::Advance(const Advance& o)
    : Orders(o), targ(o.targ), source(o.source), armyNum(new int(*o.armyNum)) {}
Advance::~Advance() { delete armyNum; }

Advance& Advance::operator=(const Advance& o) {
    if (this != &o) {
        Orders::operator=(o);
        targ = o.targ;
        source = o.source;
        if (!armyNum) armyNum = new int(0);
        *armyNum = *o.armyNum;
    }
    return *this;
}

bool Advance::validate() const {
    if (!player || !targ || !source || !armyNum || *armyNum <= 0) return false;
    if (source->getOwner() != player->getPName()) return false;
    if (source->getArmies() < *armyNum) return false;
    // If moving to own territory: must be adjacent
    // If attacking: must be adjacent as well (per Warzone-like rules)
    return source->isAdjacent(*targ);
}

bool Advance::execute() const {
    if (!validate()) return false;

    // Move armies out of source first
    source->setArmies(source->getArmies() - *armyNum);

    if (targ->getOwner() == player->getPName()) {
        // Friendly move
        targ->setArmies(targ->getArmies() + *armyNum);
    } else {
        // Simple combat simulation (kept close to partner’s intent)
        int atk = *armyNum;
        int def = targ->getArmies();

        std::srand((unsigned)std::time(nullptr));

        while (atk > 0 && def > 0) {
            // attacker 60% hit
            if ((std::rand() % 100) < 60) def--;
            // defender 70% hit
            if (def > 0 && (std::rand() % 100) < 70) atk--;
        }

        if (def <= 0 && atk > 0) {
            // take territory
            targ->setOwner(player->getPName());
            targ->setArmies(atk);
        } else {
            // defender holds with remaining def
            targ->setArmies(def);
        }
    }

    const_cast<Advance*>(this)->notify();
    return true;
}

std::string Advance::stringToLog() const { return "ORDER_EXECUTED | Advance"; }
Orders* Advance::clone() const { return new Advance(*this); }

// ================= Bomb =================
Bomb::Bomb() : Orders(nullptr), targ(nullptr) {}
Bomb::Bomb(Player* playr, Territory* target) : Orders(playr), targ(target) {}
Bomb::Bomb(const Bomb& o) : Orders(o), targ(o.targ) {}
Bomb::~Bomb() = default;

Bomb& Bomb::operator=(const Bomb& o) {
    if (this != &o) {
        Orders::operator=(o);
        targ = o.targ;
    }
    return *this;
}

bool Bomb::validate() const {
    if (!player || !targ) return false;
    if (targ->getOwner() == player->getPName()) return false;
    // Must be adjacent to at least one of player's territories
    for (auto* t : player->getTerritory()) {
        if (t && t->isAdjacent(*targ)) return true;
    }
    return false;
}

bool Bomb::execute() const {
    if (!validate()) return false;
    targ->setArmies(std::max(0, targ->getArmies() / 2));
    const_cast<Bomb*>(this)->notify();
    return true;
}

std::string Bomb::stringToLog() const { return "ORDER_EXECUTED | Bomb"; }
Orders* Bomb::clone() const { return new Bomb(*this); }

// ================= Blockade =================
Blockade::Blockade() : Orders(nullptr), targ(nullptr) {}
Blockade::Blockade(Player* playr, Territory* target) : Orders(playr), targ(target) {}
Blockade::Blockade(const Blockade& o) : Orders(o), targ(o.targ) {}
Blockade::~Blockade() = default;

Blockade& Blockade::operator=(const Blockade& o) {
    if (this != &o) {
        Orders::operator=(o);
        targ = o.targ;
    }
    return *this;
}

bool Blockade::validate() const {
    if (!player || !targ) return false;
    return (targ->getOwner() == player->getPName());
}

bool Blockade::execute() const {
    if (!validate()) return false;
    targ->setArmies(targ->getArmies() * 2);
    targ->setOwner("Neutral");
    const_cast<Blockade*>(this)->notify();
    return true;
}

std::string Blockade::stringToLog() const { return "ORDER_EXECUTED | Blockade"; }
Orders* Blockade::clone() const { return new Blockade(*this); }

// ================= Airlift =================
Airlift::Airlift() : Orders(nullptr), targ(nullptr), source(nullptr), armyNum(new int(0)) {}
Airlift::Airlift(Player* playr, Territory* target, Territory* src, int* armynum)
    : Orders(playr), targ(target), source(src), armyNum(armynum ? armynum : new int(0)) {}
Airlift::Airlift(const Airlift& o)
    : Orders(o), targ(o.targ), source(o.source), armyNum(new int(*o.armyNum)) {}
Airlift::~Airlift() { delete armyNum; }

Airlift& Airlift::operator=(const Airlift& o) {
    if (this != &o) {
        Orders::operator=(o);
        targ = o.targ;
        source = o.source;
        if (!armyNum) armyNum = new int(0);
        *armyNum = *o.armyNum;
    }
    return *this;
}

bool Airlift::validate() const {
    if (!player || !targ || !source || !armyNum || *armyNum <= 0) return false;
    if (source->getOwner() != player->getPName()) return false;
    if (targ->getOwner() != player->getPName()) return false;
    return (source->getArmies() >= *armyNum);
}

bool Airlift::execute() const {
    if (!validate()) return false;
    source->setArmies(source->getArmies() - *armyNum);
    targ->setArmies(targ->getArmies() + *armyNum);
    const_cast<Airlift*>(this)->notify();
    return true;
}

std::string Airlift::stringToLog() const { return "ORDER_EXECUTED | Airlift"; }
Orders* Airlift::clone() const { return new Airlift(*this); }

// ================= Negotiate =================
Negotiate::Negotiate() : Orders(nullptr), targ(nullptr) {}
Negotiate::Negotiate(Player* playr, Player* target) : Orders(playr), targ(target) {}
Negotiate::Negotiate(const Negotiate& o) : Orders(o), targ(o.targ) {}
Negotiate::~Negotiate() = default;

Negotiate& Negotiate::operator=(const Negotiate& o) {
    if (this != &o) {
        Orders::operator=(o);
        targ = o.targ;
    }
    return *this;
}

bool Negotiate::validate() const {
    if (!player || !targ) return false;
    return (targ != player);
}

bool Negotiate::execute() const {
    if (!validate()) return false;
    // You can mark diplomacy between player and targ in Player/Engine if you implement it.
    const_cast<Negotiate*>(this)->notify();
    return true;
}

std::string Negotiate::stringToLog() const { return "ORDER_EXECUTED | Negotiate"; }
Orders* Negotiate::clone() const { return new Negotiate(*this); }

// ================= OrdersList =================
OrdersList::OrdersList() { orders = new std::vector<Orders*>(); }

OrdersList::OrdersList(const OrdersList& other) {
    orders = new std::vector<Orders*>();
    for (auto* o : *other.orders) orders->push_back(o->clone());
}

OrdersList::~OrdersList() {
    for (auto* o : *orders) delete o;
    delete orders;
}

OrdersList& OrdersList::operator=(const OrdersList& other) {
    if (this != &other) {
        for (auto* o : *orders) delete o;
        orders->clear();
        for (auto* o : *other.orders) orders->push_back(o->clone());
    }
    return *this;
}

void OrdersList::add(Orders* o) {
    if (!o) return;
    orders->push_back(o);
    notify();
}

void OrdersList::remove(Orders* order) {
    auto it = std::find(orders->begin(), orders->end(), order);
    if (it != orders->end()) {
        delete *it;
        orders->erase(it);
    }
}

void OrdersList::move(Orders* o1, Orders* o2) {
    int p1 = -1, p2 = -1;
    for (int i = 0; i < (int)orders->size(); ++i) {
        if (orders->at(i) == o1) p1 = i;
        if (orders->at(i) == o2) p2 = i;
    }
    if (p1 >= 0 && p2 >= 0) std::swap(orders->at(p1), orders->at(p2));
}

bool OrdersList::empty() const { return orders->empty(); }
Orders* OrdersList::front() const { return orders->empty() ? nullptr : orders->front(); }

void OrdersList::pop_front() {
    if (orders->empty()) return;
    delete orders->front();
    orders->erase(orders->begin());
    notify();
}

const std::vector<Orders*>& OrdersList::getOrders() const { return *orders; }

std::string OrdersList::stringToLog() const {
    return "ORDER_ADDED | total=" + std::to_string((int)orders->size());
}

std::ostream& operator<<(std::ostream& os, const OrdersList& ol) {
    os << "OrdersList size=" << ol.orders->size();
    return os;
}
