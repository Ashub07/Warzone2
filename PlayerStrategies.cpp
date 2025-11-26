#include <iostream>
#include "PlayerStrategies.h"
#include "Player.h"
#include "Map.h"
#include "Orders.h"
#include "Cards.h"
#include <list>



//------------------------------------
//---------HUMAN PLAYER---------------
//------------------------------------

//human player issue order function
void HumanPlayerStrategy::issueOrder(Player p, Map m){
    //orders list
    std::list<Orders*> orders;

    //player reinforcement pool
    int reinPool = p.getReinforcementPool();
    //get map
    Map map = m;
    //get player
    Player p1 = p;
    //get territories
    std::vector<Territory*> attackList = toAttack(map, p1);
    //get territories
    std::vector<Territory*> defendList = toDefend(p1);
    //index
    int index = 0;

    //reinforcements
    while(reinPool > 0){
        Territory* t = defendList[index];
        //get army number
        int arm = t->getArmies();
        int added = 0;
        std::cout << "How many army units would you like to add to this territory? " << t->getName() << " You have " << reinPool << " left: ";
        std::cin >> added;
        //add 1 to army
        arm += added;
        // Add 1 army (example logic)
        t->setArmies(arm);
        reinPool -= arm;
        // move to next territory (loop around)
        index = (index + 1) % defendList.size();
        Orders* ord = new Deploy(&p1, t, &arm);
        orders.push_back(ord);
    }

    //advance
    while(!attackList.empty()){
        Territory* t1 = attackList[index];
        Territory* t2 = defendList[index];
        int arm = t2->getArmies();
        Orders* ord = new Advance(&p1, t1, t2, &arm);
        orders.push_back(ord);
        // move to next territory (loop around)
        index = (index + 1) % defendList.size();
    }

    //CARDS
//cards HELP TO CHECK HOW TO GET CARDS**
    Hand h;
    Card* c1;
    Deck* d1 = p1.getDeck();
    std::string choice;

    if(p1.getDeck()){
        Deck* d = p1.getDeck();
        c1 = d->draw(h);
        std::cout << "Here is your card " << c1 << " Would you like to play it? Please answer by yes or no";
        std::cin >> choice;

        if(choice == "Yes" || choice == "yes"){
            c1->play(p1, *d1, h);
        }

    }
}

//human player to attack function
std::vector<Territory*> HumanPlayerStrategy::toAttack(Map m, Player p){
    //get map of game and all its territories
    std::vector<Territory*>* terris = m.getTerritories();

    //number of territories to attack
    int attackNumber;
    //terirtory name
    std::string terriName;
    //to attack list
    std::vector<Territory*> attack;

    //check the entire list of territories to find which one to attack
    std::cout << "Please enter the number of territories you wish to attack: ";
    std::cin >> attackNumber;

    //add territories to attack
    for(int i = 0; i < attackNumber; i++){
        std::cout << "Please enter the Territory name: ";
        std::cin >> terriName;

        //go through list of territories and find matching name
        for(int j = 0; j < terris->size(); j++){
            Territory* t = (*terris)[j];
            if(t->getName() == terriName){
                attack.push_back((*terris)[j]);
            }
        }
    }
    return attack;


}

//human player to defend function
std::vector<Territory*> HumanPlayerStrategy::toDefend(Player p){
    //get player territories
    std::vector<Territory*> pt = p.getTerritory();

    //create list to return
    std::vector<Territory*> defend;

    //number of territories to attack
    int defendNumber;
    //terirtory name
    std::string terriName;

    //check the entire list of territories to find which one to defend
    std::cout << "Please enter the number of territories you wish to defend: ";
    std::cin >> defendNumber;

    //go through list of territories and put the weakest in to defend list
    for(int i = 0; i < defendNumber; i++){
        std::cout << "Please enter the Territory name: ";
        std::cin >> terriName;

        for(int j = 0; j < pt.size(); j++){
            if(pt[i]->getName() == terriName){
                defend.push_back(pt[i]);
            }
        }
    }
    

    return defend;

}


//------------------------------------
//---------AGGRESSIVE PLAYER----------
//------------------------------------

//aggressive player issue order function
void AggressivePlayerStrategy::issueOrder(Player p, Map m){
    //orders list
    std::list<Orders*> orders;

    //player reinforcement pool
    int reinPool = p.getReinforcementPool();
    //get map
    Map map = m;
    //get player
    Player p1 = p;
    Deck* d1 = p1.getDeck();
    //get territories
    std::vector<Territory*> attackList = toAttack(map, p1);
    //get territories
    std::vector<Territory*> defendList = toDefend(p1);
    //index
    int index = 0;

    //reinforcements
    while(reinPool > 0){
        Territory* t = defendList[index];
        //get army number
        int arm = t->getArmies();
        //add 1 to army
        arm += 1;
        // Add 1 army (example logic)
        t->setArmies(arm);
        reinPool--;
        // move to next territory (loop around)
        index = (index + 1) % defendList.size();
        Orders* ord = new Deploy(&p1, t, &arm);
        orders.push_back(ord);
    }

    //advance
    while(!attackList.empty()){
        Territory* t1 = attackList[index];
        Territory* t2 = defendList[index];
        int arm = t2->getArmies();
        Orders* ord = new Advance(&p1, t1, t2, &arm);
        orders.push_back(ord);
        // move to next territory (loop around)
        index = (index + 1) % defendList.size();
    }

    //cards HELP TO CHECK HOW TO GET CARDS**
    Hand h;
    Card* c1;

    if(p1.getDeck()){
        Deck* d = p1.getDeck();
        c1 = d->draw(h);

        if(c1->getType() == cardType::Bomb || c1->getType() == cardType::Reinforcement){
            c1->play(p1, *d1, h);
        }

    }

}

//aggressive player to attack function
std::vector<Territory*> AggressivePlayerStrategy::toAttack(Map m, Player p){
    //check the entire list of territories to find which one to attack
    std::vector<Territory*>* terris = m.getTerritories();

    //get player name
    std::string player = p.getPName();

    //get player terirtories
    std::vector<Territory*> pt = p.getTerritory();

    //create list to return
    std::vector<Territory*> attack;

    for (int i = 0; i < terris->size(); i++) {
        Territory* t = (*terris)[i];

        // Skip territories owned by this player
        if (t->getOwner() == player)
            continue;

        // Compare armies: enemyTerritoryArmies <= player’s largest army count
        // First, find the strongest territory the player owns:

        int strongest = 0;
        for (Territory* myT : pt) {
            if (myT->getArmies() > strongest)
                strongest = myT->getArmies();
        }

        // Only add if enemy has less/equal armies
        if (t->getArmies() <= strongest) {
            attack.push_back(t);
        }
    }

    return attack;

}

//aggressive player to defend function
std::vector<Territory*> AggressivePlayerStrategy::toDefend(Player p){
    //get player territories
    std::vector<Territory*> pt = p.getTerritory();

    //create list to return
    std::vector<Territory*> defend;

    //go through list of territories and put the weakest in to defend list
    for(int i = 0; i < pt.size(); i++){
        //MAKE SURE TO FIND THE CORRECT NUMBER OF ARMIES TO CHECK FOR
        if(pt[i]->getArmies() > 5){
            defend.push_back(pt[i]);
        }
    }

    return defend;


}


//------------------------------------
//---------BENEVOLENT PLAYER----------
//------------------------------------

//benevolent player issue order function
void BenevolentPlayerStrategy::issueOrder(Player p, Map m){
    //orders list
    std::list<Orders*> orders;

    //player reinforcement pool
    int reinPool = p.getReinforcementPool();
    //get map
    Map map = m;
    //get player
    Player p1 = p;
    //get territories
    std::vector<Territory*> defendList = toDefend(p1);
    //index
    int index = 0;

    //reinforcements
    while(reinPool > 0){
        Territory* t = defendList[index];
        //get army number
        int arm = t->getArmies();
        //add 1 to army
        arm += 1;
        // Add 1 army (example logic)
        t->setArmies(arm);
        reinPool--;
        // move to next territory (loop around)
        index = (index + 1) % defendList.size();
        Orders* ord = new Deploy(&p1, t, &arm);
        orders.push_back(ord);
    }


    //CARDS
    //cards HELP TO CHECK HOW TO GET CARDS**
    Hand h;
    Card* c1;
    Deck* d1 = p1.getDeck();

    if(p1.getDeck()){
        Deck* d = p1.getDeck();
        c1 = d->draw(h);

        if(c1->getType() == cardType::Blockade || c1->getType() == cardType::Reinforcement || c1->getType() == cardType::Airlift || c1->getType() == cardType::Diplomacy){
            c1->play(p1, *d1, h);
        }

    }
}

//benevolent player to attack function
void BenevolentPlayerStrategy::toAttack(){
    //this player doesn't attack so do nothing
    std::cout << "no attack";

}

//benevolent player to defend function
std::vector<Territory*> BenevolentPlayerStrategy::toDefend(Player p){
    //get player territories
    std::vector<Territory*> pt = p.getTerritory();

    //create list to return
    std::vector<Territory*> defend;

    //go through list of territories and put the weakest in to defend list
    for(int i = 0; i < pt.size(); i++){
        //MAKE SURE TO FIND THE CORRECT NUMBER OF ARMIES TO CHECK FOR
        if(pt[i]->getArmies() < 5){
            defend.push_back(pt[i]);
        }
    }

    return defend;

}


//------------------------------------
//---------NEUTRAL PLAYER-------------
//------------------------------------



//------------------------------------
//---------CHEATER PLAYER-------------
//------------------------------------
