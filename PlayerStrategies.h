#pragma once
#include <iostream>
#include <vector>
#include "Player.h"
#include "Map.h"
#include "Cards.h"

//abstract player strategy class
class PlayerStrategy {
    public:
        virtual ~PlayerStrategy() {}

        // All concrete strategies follow this signature pattern
        virtual void issueOrder(Player p, Map m) = 0;
        virtual std::vector<Territory*> toAttack(Map m, Player p) = 0;
        virtual std::vector<Territory*> toDefend(Player p) = 0;
};

//human player class
class HumanPlayerStrategy : public PlayerStrategy{
    public:
        void issueOrder(Player p, Map m);
        std::vector<Territory*> toAttack(Map m, Player p);
        std::vector<Territory*> toDefend(Player p);
};

//aggressive player class
class AggressivePlayerStrategy : public PlayerStrategy{
    public:
        void issueOrder(Player p, Map m);
        std::vector<Territory*> toAttack(Map m, Player p);
        std::vector<Territory*> toDefend(Player p);
};

//benevolent player class
class BenevolentPlayerStrategy : public PlayerStrategy{
    public:
        void issueOrder(Player p, Map m) override;
        std::vector<Territory*> toAttack(Map m, Player p) override; 
        std::vector<Territory*> toDefend(Player p) override;
};

//neutral player class
class NeutralPlayerStrategy : public PlayerStrategy{
    public:
        void issueOrder(Player p, Map m) override;
        std::vector<Territory*> toAttack(Map m, Player p) override;
        std::vector<Territory*> toDefend(Player p) override;
};

//cheater player class
class CheaterPlayerStrategy : public PlayerStrategy{
    public:
        void issueOrder(Player p, Map m) override;
        std::vector<Territory*> toAttack(Map m, Player p) override;
        std::vector<Territory*> toDefend(Player p) override;
};


//context class that lets you decide which player strategy subclass to use
class SortContext{
    private:
        PlayerStrategy* strategy;
    
    public:
        //sets the strategy thst is to be played
        void setStrategy(PlayerStrategy* strategy);
        //executes the strategy previously set
        void executeStrategy(Player& player, Map& map);
};
