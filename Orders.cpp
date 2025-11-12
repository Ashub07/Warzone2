#include "Orders.h"
#include <iostream>
#include <typeinfo>
#include "LoggingObserver.h"

// =================== Base: Orders ===================
// Non-owning: player* is referenced only (never deleted / never deep-copied)

Orders::Orders() : player(nullptr) {}
Orders::Orders(Player* p) : player(p) {}

Orders::Orders(const Orders& other)
    : Subject(other)   // keep observer wiring
    , player(other.player) // shallow copy, non-owning
{}

Orders::~Orders() {} // never delete player

Orders& Orders::operator=(const Orders& other) {
    if (this != &other) {
        Subject::operator=(other);
        player = other.player; // shallow
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Orders& order) {
    os << "Order owned by player: "
       << (order.player ? order.player->getPName() : "(null)");
    return os;
}

Player Orders::getPlayer() const {
    return player ? *player : Player(); // return-by-value per header
}

// We cannot safely store a reference to a temporary value param.
// If a real Player object is already referenced, copy-assign into it.
// Otherwise, do nothing (GameEngine never calls this anyway).
void Orders::setPlayer(Player p) {
    if (player) *player = p;
}

// =================== Deploy ===================
// Non-owning: targ is a borrowed Territory* from the Map.
// Owned: armyNum is heap int created by caller; we assume ownership and delete.

Deploy::Deploy() : Orders(nullptr), targ(nullptr), armyNum(nullptr) {}

Deploy::Deploy(Player* playr, Territory* target, int* armynum)
    : Orders(playr), targ(target), armyNum(armynum) {}

Deploy::Deploy(const Deploy& other)
    : Orders(other)
    , targ(other.targ) // shallow, non-owning
    , armyNum(other.armyNum ? new int(*other.armyNum) : nullptr) {}

Deploy::~Deploy() {
    delete armyNum; // do NOT delete targ
}

Deploy& Deploy::operator=(const Deploy& other) {
    if (this != &other) {
        Orders::operator=(other);
        // targ is non-owning
        targ = other.targ;
        // deep-copy armyNum
        if (armyNum) delete armyNum;
        armyNum = other.armyNum ? new int(*other.armyNum) : nullptr;
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Deploy& o) {
    os << "Deploy " << (o.armyNum ? *o.armyNum : 0)
       << " to " << (o.targ ? o.targ->getName() : "(null territory)")
       << " by " << (o.player ? o.player->getPName() : "(null player)");
    return os;
}

Player    Deploy::getPlayer() const { return Orders::getPlayer(); }
void      Deploy::setPlayer(Player p) { Orders::setPlayer(p); }

Territory Deploy::getTarg() const { return targ ? *targ : Territory(); }
void      Deploy::setTarget(Territory t) { if (targ) *targ = t; /* non-owning */ }

int  Deploy::getArmynum() const { return armyNum ? *armyNum : 0; }
void Deploy::setArmynum(int n)  { if (!armyNum) armyNum = new int(n); else *armyNum = n; }

bool Deploy::validate() const {
    return player && targ && armyNum && *armyNum > 0;
}

bool Deploy::execute() const {
    if (!validate()) return false;
    const_cast<Deploy*>(this)->notify();
    return true;
}

std::string Deploy::stringToLog() const { return "ORDER_EXECUTED | Deploy"; }
Deploy*     Deploy::clone() const { return new Deploy(*this); }

// =================== Advance ===================
// Non-owning: targ/source borrowed from Map; armyNum owned.

Advance::Advance()
    : Orders(nullptr), targ(nullptr), source(nullptr), armyNum(nullptr) {}

Advance::Advance(Player* p, Territory* target, Territory* src, int* armynum)
    : Orders(p), targ(target), source(src), armyNum(armynum) {}

Advance::Advance(const Advance& other)
    : Orders(other)
    , targ(other.targ)     // shallow
    , source(other.source) // shallow
    , armyNum(other.armyNum ? new int(*other.armyNum) : nullptr) {}

Advance::~Advance() {
    delete armyNum; // do NOT delete targ/source
}

Advance& Advance::operator=(const Advance& other) {
    if (this != &other) {
        Orders::operator=(other);
        targ   = other.targ;
        source = other.source;
        if (armyNum) delete armyNum;
        armyNum = other.armyNum ? new int(*other.armyNum) : nullptr;
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Advance& o) {
    os << "Advance " << (o.armyNum ? *o.armyNum : 0)
       << " from " << (o.source ? o.source->getName() : "(null)")
       << " to "   << (o.targ   ? o.targ->getName()   : "(null)")
       << " by "   << (o.player ? o.player->getPName() : "(null player)");
    return os;
}

Player    Advance::getPlayer() const { return Orders::getPlayer(); }
void      Advance::setPlayer(Player p) { Orders::setPlayer(p); }
Territory Advance::getTarg() const { return targ ? *targ : Territory(); }
void      Advance::setTarget(Territory t) { if (targ) *targ = t; }
Territory Advance::getSource() const { return source ? *source : Territory(); }
void      Advance::setSource(Territory s) { if (source) *source = s; }
int       Advance::getArmynum() const { return armyNum ? *armyNum : 0; }
void      Advance::setArmynum(int n) { if (!armyNum) armyNum = new int(n); else *armyNum = n; }

bool Advance::validate() const {
    return player && targ && source && armyNum && *armyNum > 0;
}

bool Advance::execute() const {
    if (!validate()) return false;
    const_cast<Advance*>(this)->notify();
    return true;
}

std::string Advance::stringToLog() const { return "ORDER_EXECUTED | Advance"; }
Advance*    Advance::clone() const { return new Advance(*this); }

// =================== Bomb ===================
// Non-owning: targ borrowed.

Bomb::Bomb() : Orders(nullptr), targ(nullptr) {}

Bomb::Bomb(Player* p, Territory* target) : Orders(p), targ(target) {}

Bomb::Bomb(const Bomb& other)
    : Orders(other), targ(other.targ) {} // shallow

Bomb::~Bomb() {
    // do NOT delete targ
}

Bomb& Bomb::operator=(const Bomb& other) {
    if (this != &other) {
        Orders::operator=(other);
        targ = other.targ;
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Bomb& o) {
    os << "Bomb " << (o.targ ? o.targ->getName() : "(null)")
       << " by "  << (o.player ? o.player->getPName() : "(null player)");
    return os;
}

Player    Bomb::getPlayer() const { return Orders::getPlayer(); }
void      Bomb::setPlayer(Player p) { Orders::setPlayer(p); }
Territory Bomb::getTarg() const { return targ ? *targ : Territory(); }
void      Bomb::setTarget(Territory t) { if (targ) *targ = t; }

bool Bomb::validate() const { return player && targ; }

bool Bomb::execute() const {
    if (!validate()) return false;
    const_cast<Bomb*>(this)->notify();
    return true;
}

std::string Bomb::stringToLog() const { return "ORDER_EXECUTED | Bomb"; }
Bomb*       Bomb::clone() const { return new Bomb(*this); }

// =================== Blockade ===================
// Non-owning: targ borrowed.

Blockade::Blockade() : Orders(nullptr), targ(nullptr) {}

Blockade::Blockade(Player* p, Territory* target) : Orders(p), targ(target) {}

Blockade::Blockade(const Blockade& other)
    : Orders(other), targ(other.targ) {} // shallow

Blockade::~Blockade() {
    // do NOT delete targ
}

Blockade& Blockade::operator=(const Blockade& other) {
    if (this != &other) {
        Orders::operator=(other);
        targ = other.targ;
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Blockade& o) {
    os << "Blockade " << (o.targ ? o.targ->getName() : "(null)")
       << " by "      << (o.player ? o.player->getPName() : "(null player)");
    return os;
}

Player    Blockade::getPlayer() const { return Orders::getPlayer(); }
void      Blockade::setPlayer(Player p) { Orders::setPlayer(p); }
Territory Blockade::getTarg() const { return targ ? *targ : Territory(); }
void      Blockade::setTarget(Territory t) { if (targ) *targ = t; }

bool Blockade::validate() const { return player && targ; }

bool Blockade::execute() const {
    if (!validate()) return false;
    const_cast<Blockade*>(this)->notify();
    return true;
}

std::string Blockade::stringToLog() const { return "ORDER_EXECUTED | Blockade"; }
Blockade*   Blockade::clone() const { return new Blockade(*this); }

// =================== Airlift ===================
// Non-owning: targ/source borrowed; armyNum owned.

Airlift::Airlift()
    : Orders(nullptr), targ(nullptr), source(nullptr), armyNum(nullptr) {}

Airlift::Airlift(Player* p, Territory* target, Territory* src, int* armynum)
    : Orders(p), targ(target), source(src), armyNum(armynum) {}

Airlift::Airlift(const Airlift& other)
    : Orders(other)
    , targ(other.targ)     // shallow
    , source(other.source) // shallow
    , armyNum(other.armyNum ? new int(*other.armyNum) : nullptr) {}

Airlift::~Airlift() {
    delete armyNum; // do NOT delete targ/source
}

Airlift& Airlift::operator=(const Airlift& other) {
    if (this != &other) {
        Orders::operator=(other);
        targ   = other.targ;
        source = other.source;
        if (armyNum) delete armyNum;
        armyNum = other.armyNum ? new int(*other.armyNum) : nullptr;
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Airlift& o) {
    os << "Airlift " << (o.armyNum ? *o.armyNum : 0)
       << " from "  << (o.source ? o.source->getName() : "(null)")
       << " to "    << (o.targ   ? o.targ->getName()   : "(null)")
       << " by "    << (o.player ? o.player->getPName() : "(null player)");
    return os;
}

Player    Airlift::getPlayer() const { return Orders::getPlayer(); }
void      Airlift::setPlayer(Player p) { Orders::setPlayer(p); }
Territory Airlift::getTarg() const { return targ ? *targ : Territory(); }
void      Airlift::setTarget(Territory t) { if (targ) *targ = t; }
Territory Airlift::getSource() const { return source ? *source : Territory(); }
void      Airlift::setSource(Territory s) { if (source) *source = s; }
int       Airlift::getArmynum() const { return armyNum ? *armyNum : 0; }
void      Airlift::setArmynum(int n) { if (!armyNum) armyNum = new int(n); else *armyNum = n; }

bool Airlift::validate() const {
    return player && targ && source && armyNum && *armyNum > 0;
}

bool Airlift::execute() const {
    if (!validate()) return false;
    const_cast<Airlift*>(this)->notify();
    return true;
}

std::string Airlift::stringToLog() const { return "ORDER_EXECUTED | Airlift"; }
Airlift*    Airlift::clone() const { return new Airlift(*this); }

// =================== Negotiate ===================
// Non-owning: targ (other player) is borrowed.

Negotiate::Negotiate() : Orders(nullptr), targ(nullptr) {}

Negotiate::Negotiate(Player* p, Player* target) : Orders(p), targ(target) {}

Negotiate::Negotiate(const Negotiate& other)
    : Orders(other), targ(other.targ) {} // shallow

Negotiate::~Negotiate() {
    // do NOT delete targ
}

Negotiate& Negotiate::operator=(const Negotiate& other) {
    if (this != &other) {
        Orders::operator=(other);
        targ = other.targ; // shallow
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Negotiate& o) {
    os << "Negotiate between "
       << (o.player ? o.player->getPName() : "(null)")
       << " and "
       << (o.targ ? o.targ->getPName() : "(null)");
    return os;
}

Player Negotiate::getPlayer() const { return Orders::getPlayer(); }
void   Negotiate::setPlayer(Player p) { Orders::setPlayer(p); }
Player Negotiate::getTarget() const { return targ ? *targ : Player(); }
void   Negotiate::setTarget(Player p) { if (targ) *targ = p; /* non-owning */ }

bool Negotiate::validate() const {
    return player && targ && (targ != player);
}

bool Negotiate::execute() const {
    if (!validate()) return false;
    const_cast<Negotiate*>(this)->notify();
    return true;
}

std::string Negotiate::stringToLog() const { return "ORDER_EXECUTED | Negotiate"; }
Negotiate*  Negotiate::clone() const { return new Negotiate(*this); }

// =================== OrdersList ===================

OrdersList::OrdersList() : orders(new std::vector<Orders*>()) {}

OrdersList::OrdersList(const OrdersList& other) : Subject(other) {
    orders = new std::vector<Orders*>();
    orders->reserve(other.orders->size());
    for (Orders* o : *other.orders) orders->push_back(o->clone());
}

OrdersList::~OrdersList() {
    for (auto* o : *orders) delete o;
    delete orders;
}

OrdersList& OrdersList::operator=(const OrdersList& other) {
    if (this != &other) {
        Subject::operator=(other);
        for (auto* o : *orders) delete o;
        orders->clear();
        orders->reserve(other.orders->size());
        for (Orders* o : *other.orders) orders->push_back(o->clone());
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const OrdersList& list) {
    os << "Orders: ";
    for (size_t i = 0; i < list.orders->size(); ++i) {
        os << *list.orders->at(i);
        if (i + 1 < list.orders->size()) os << ", ";
    }
    os << ".";
    return os;
}

std::vector<Orders*> OrdersList::getOrders() const { return *orders; }

void OrdersList::setOrders(std::vector<Orders*> newOrders) {
    for (auto* o : *orders) delete o;
    *orders = std::move(newOrders);
}

void OrdersList::add(Orders* order) {
    orders->push_back(order);
    notify(); // logging hook
}

std::string OrdersList::stringToLog() const {
    return "ORDER_ADDED | total=" + std::to_string(orders ? (int)orders->size() : 0);
}

void OrdersList::remove(Orders* order) {
    for (auto it = orders->begin(); it != orders->end(); ++it) {
        if (*it == order) {
            delete *it;
            orders->erase(it);
            break;
        }
    }
}

void OrdersList::move(Orders* a, Orders* b) {
    int ia = -1, ib = -1;
    for (int i = 0; i < (int)orders->size(); ++i) {
        if (orders->at(i) == a) ia = i;
        if (orders->at(i) == b) ib = i;
    }
    if (ia > -1 && ib > -1) std::swap(orders->at(ia), orders->at(ib));
}