#include "Cards.h"
#include "Player.h"
#include "Orders.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <random>

// ================= Local Helpers =================
namespace {
    // Random generator used for drawing cards at random from the deck
    std::mt19937& rng() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return gen;
    }

    // Helper function to convert enum cardType to a string
    const char* to_string(cardType t) {
        switch (t) {
            case cardType::Bomb:          return "Bomb";
            case cardType::Reinforcement: return "Reinforcement";
            case cardType::Blockade:      return "Blockade";
            case cardType::Airlift:       return "Airlift";
            case cardType::Diplomacy:     return "Diplomacy";
            default:                      return "Unknown";
        }
    }
}

// ================= Card =================

// Constructor: allocate card type on the heap
Card::Card(cardType t) : type(new cardType(t)) {}

// Copy constructor: deep copy the enum value
Card::Card(const Card& other) : type(new cardType(*other.type)) {}

// Copy assignment: safely deep copy and replace old value
Card& Card::operator=(const Card& other) {
    if (this != &other) {
        auto* fresh = new cardType(*other.type);
        delete type;
        type = fresh;
    }
    return *this;
}

// Destructor: free the allocated enum
Card::~Card() {
    delete type;
}

// Getter: return the card type
cardType Card::getType() const {
    return *type;
}

// Convert a card into the appropriate Order object for the given player
Orders* Card::toOrder(Player& /*p*/) const {
    switch (*type) {
        case cardType::Bomb:          return new Bomb();
        case cardType::Blockade:      return new Blockade();
        case cardType::Airlift:       return new Airlift();
        case cardType::Diplomacy:     return new Negotiate();
        case cardType::Reinforcement: return new Deploy();
        default:                      return nullptr;
    }
}


// Play a card: create the order, add it to player's order list, remove card from hand, and return it to deck
void Card::play(Player& p, Deck& d, Hand& h) {
    // Create the order from the card
    Orders* o = toOrder(p);
    if (!o) {
        std::cerr << "[Card::play] Could not create order for card type: "
                  << to_string(*type) << "\n";
        return;
    }

    // Add the order to player's orders list
    OrdersList* list = p.getOrder();
    if (!list) {
        std::cerr << "[Card::play] Player has no OrdersList; dropping order.\n";
        delete o;
        return;
    }
    list->add(o);

    // Remove the card from the player's hand
    if (!h.remove(this)) {
        std::cerr << "[Card::play] Card not found in hand (already removed?).\n";
    }

    // Return the card back to the deck
    d.addBack(this);
}

// ================= Hand =================

// Constructor: allocate a fresh vector of Card*
Hand::Hand() : cards_(new std::vector<Card*>) {}

// Copy constructor: deep copy each card into a new Hand
Hand::Hand(const Hand& other) : cards_(new std::vector<Card*>) {
    cards_->reserve(other.cards_->size());
    for (Card* c : *other.cards_) {
        cards_->push_back(new Card(*c));
    }
}

// Copy assignment: build deep copy first, then replace old
Hand& Hand::operator=(const Hand& other) {
    if (this != &other) {
        auto* fresh = new std::vector<Card*>;
        fresh->reserve(other.cards_->size());
        for (Card* c : *other.cards_) {
            fresh->push_back(new Card(*c));
        }
        for (Card* c : *cards_) delete c;
        delete cards_;
        cards_ = fresh;
    }
    return *this;
}

// Destructor: clean up all cards and vector
Hand::~Hand() {
    for (Card* c : *cards_) delete c;
    delete cards_;
}

// Add a card pointer to the hand
void Hand::add(Card* c) {
    if (c) cards_->push_back(c);
}

// Remove a specific card pointer from the hand without deleting it
bool Hand::remove(Card* c) {
    if (!c) return false;
    auto it = std::find(cards_->begin(), cards_->end(), c);
    if (it == cards_->end()) return false;
    cards_->erase(it);
    return true;
}

// Expose read-only access to the internal vector
const std::vector<Card*>* Hand::cards() const {
    return cards_;
}

// ================= Deck =================

// Constructor: allocate a fresh vector of Card*
Deck::Deck() : cards_(new std::vector<Card*>) {}

// Copy constructor: deep copy each card into a new Deck
Deck::Deck(const Deck& other) : cards_(new std::vector<Card*>) {
    cards_->reserve(other.cards_->size());
    for (Card* c : *other.cards_) {
        cards_->push_back(new Card(*c));
    }
}

// Construct a Deck from an existing vector of Card*
Deck::Deck(const std::vector<Card*>& cards) : cards_(new std::vector<Card*>) {
    cards_->reserve(cards.size());
    for (Card* c : cards) {
        cards_->push_back(new Card(*c));
    }
}

// Copy assignment: deep copy then replace old data
Deck& Deck::operator=(const Deck& other) {
    if (this != &other) {
        auto* fresh = new std::vector<Card*>;
        fresh->reserve(other.cards_->size());
        for (Card* c : *other.cards_) {
            fresh->push_back(new Card(*c));
        }
        for (Card* c : *cards_) delete c;
        delete cards_;
        cards_ = fresh;
    }
    return *this;
}

// Destructor: clean up all cards and vector
Deck::~Deck() {
    for (Card* c : *cards_) delete c;
    delete cards_;
}

// Draw a card at random from the deck and move ownership to the target Hand
Card* Deck::draw(Hand& targetHand) {
    if (cards_->empty()) {
        std::cerr << "[Deck::draw] Deck is empty.\n";
        return nullptr;
    }
    std::uniform_int_distribution<size_t> dist(0, cards_->size() - 1);
    size_t idx = dist(rng());

    Card* picked = cards_->at(idx);
    cards_->erase(cards_->begin() + static_cast<std::ptrdiff_t>(idx));
    targetHand.add(picked);

    return picked;
}

// Add a card back to the deck
void Deck::addBack(Card* c) {
    if (c) cards_->push_back(c);
}

// Return number of cards in the deck
size_t Deck::size() const {
    return cards_->size();
}

// ================= Output =================

// Print a Card
std::ostream& operator<<(std::ostream& os, const Card& c) {
    os << "Card(" << to_string(c.getType()) << ")";
    return os;
}

// Print a Deck
std::ostream& operator<<(std::ostream& os, const Deck& d) {
    os << "Deck[" << d.size() << "]: { ";
    for (const Card* c : *d.cards_) os << *c << " ";
    os << "}";
    return os;
}

// Print a Hand
std::ostream& operator<<(std::ostream& os, const Hand& h) {
    os << "Hand[" << h.cards()->size() << "]: { ";
    for (const Card* c : *h.cards()) os << *c << " ";
    os << "}";
    return os;
}
