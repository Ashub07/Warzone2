#pragma once
#include <vector>
#include <string>
#include <ostream>
#include "LoggingObserver.h"

class Player;
class Territory;

// ---------- Base Orders ----------
class Orders : public ILoggable, public Subject {
public:
    Orders();
    explicit Orders(Player* owner);
    Orders(const Orders& other);
    virtual ~Orders();

    Orders& operator=(const Orders& other);

    // Non-owning: Orders never delete player/territories.
    Player* getPlayer() const { return player; }
    void setPlayer(Player* p) { player = p; }

    // Each concrete order must implement these:
    virtual bool validate() const = 0;
    virtual bool execute() const = 0;
    virtual Orders* clone() const = 0;

    // Logging
    virtual std::string stringToLog() const = 0;

    friend std::ostream& operator<<(std::ostream& os, const Orders& order);

protected:
    Player* player;
};

// ---------- Deploy ----------
class Deploy : public Orders {
public:
    Deploy();
    Deploy(Player* playr, Territory* target, int* armynum);
    Deploy(const Deploy& other);
    ~Deploy() override;

    Deploy& operator=(const Deploy& other);

    bool validate() const override;
    bool execute() const override;
    Orders* clone() const override;

    std::string stringToLog() const override;

private:
    Territory* targ;  // non-owning
    int* armyNum;     // owning small heap int is fine (or you can store by value)
};

// ---------- Advance ----------
class Advance : public Orders {
public:
    Advance();
    Advance(Player* playr, Territory* target, Territory* source, int* armynum);
    Advance(const Advance& other);
    ~Advance() override;

    Advance& operator=(const Advance& other);

    bool validate() const override;
    bool execute() const override;
    Orders* clone() const override;

    std::string stringToLog() const override;

private:
    Territory* targ;   // non-owning
    Territory* source; // non-owning
    int* armyNum;      // owning
};

// ---------- Bomb ----------
class Bomb : public Orders {
public:
    Bomb();
    Bomb(Player* playr, Territory* target);
    Bomb(const Bomb& other);
    ~Bomb() override;

    Bomb& operator=(const Bomb& other);

    bool validate() const override;
    bool execute() const override;
    Orders* clone() const override;

    std::string stringToLog() const override;

private:
    Territory* targ; // non-owning
};

// ---------- Blockade ----------
class Blockade : public Orders {
public:
    Blockade();
    Blockade(Player* playr, Territory* target);
    Blockade(const Blockade& other);
    ~Blockade() override;

    Blockade& operator=(const Blockade& other);

    bool validate() const override;
    bool execute() const override;
    Orders* clone() const override;

    std::string stringToLog() const override;

private:
    Territory* targ; // non-owning
};

// ---------- Airlift ----------
class Airlift : public Orders {
public:
    Airlift();
    Airlift(Player* playr, Territory* target, Territory* source, int* armynum);
    Airlift(const Airlift& other);
    ~Airlift() override;

    Airlift& operator=(const Airlift& other);

    bool validate() const override;
    bool execute() const override;
    Orders* clone() const override;

    std::string stringToLog() const override;

private:
    Territory* targ;   // non-owning
    Territory* source; // non-owning
    int* armyNum;      // owning
};

// ---------- Negotiate ----------
class Negotiate : public Orders {
public:
    Negotiate();
    Negotiate(Player* playr, Player* target);
    Negotiate(const Negotiate& other);
    ~Negotiate() override;

    Negotiate& operator=(const Negotiate& other);

    bool validate() const override;
    bool execute() const override;
    Orders* clone() const override;

    std::string stringToLog() const override;

private:
    Player* targ; // non-owning
};

// ---------- OrdersList ----------
class OrdersList : public ILoggable, public Subject {
public:
    OrdersList();
    OrdersList(const OrdersList& other);
    ~OrdersList() override;

    OrdersList& operator=(const OrdersList& other);

    void add(Orders* order);
    void remove(Orders* order);
    void move(Orders* order1, Orders* order2);

    // Helpers needed by Part 3 loop:
    bool empty() const;
    Orders* front() const;
    void pop_front();

    // For debugging/inspection:
    const std::vector<Orders*>& getOrders() const;

    std::string stringToLog() const override;

    friend std::ostream& operator<<(std::ostream& os, const OrdersList& ol);

private:
    std::vector<Orders*>* orders;
};
