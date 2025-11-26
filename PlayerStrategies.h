#include <iostream>

//abstract player strategy class
class PlayerStrategy{
    public:
        virtual void issueOrder() =0;
        virtual void toAttack() =0;
        virtual void toDefend() =0;
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
        void issueOrder(Player p, Map m);
        void toAttack();
        std::vector<Territory*> toDefend(Player p);
};

//neutral player class


//cheater player class




//context class that lets you decide which player strategy subclass to use
class SortContext{
    private:
        PlayerStrategy* strategy;
    
    public:
        //sets the strategy thst is to be played
        void setStrategy(PlayerStrategy* strategy);
        //executes the strategy previously set
        void executeStrategy();
};