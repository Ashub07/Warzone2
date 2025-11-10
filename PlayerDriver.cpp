#include <iostream>
#include <vector>
#include "Player.h"
#include "Map.h"

using namespace std;

void testPlayer() {
    cout << "----- Player Driver Test -----\n";

    // ===== Create territories with explicit empty adjacency vectors =====
    auto* adj1 = new std::vector<Territory*>();
    auto* adj2 = new std::vector<Territory*>();
    auto* adj3 = new std::vector<Territory*>();
    auto* adj4 = new std::vector<Territory*>();

    std::vector<Territory*> territories;
    Territory* t1 = new Territory("T1", "C1", "Joe", 2, 1, adj1);
    territories.push_back(t1);
    Territory* t2 = new Territory("T2", "C2", "Joe", 3, 2, adj2);
    territories.push_back(t2);
    Territory* t3 = new Territory("T3", "C1", "Lily", 2, 3, adj3);
    territories.push_back(t3);
    Territory* t4 = new Territory("T4", "C2", "Joe", 1, 4, adj4);
    territories.push_back(t4);

    cout << "Territories created.\n";

    // ===== Create a deck dynamically (Player expects Deck*) =====
    std::vector<Card*> cards1;
    cards1.push_back(new Card(cardType::Bomb));
    cards1.push_back(new Card(cardType::Diplomacy));
    cards1.push_back(new Card(cardType::Airlift));

    Deck* d1 = new Deck(cards1);
    cout << "Deck created.\n";

    // ===== Create an OrdersList dynamically (Player expects OrdersList*) =====
    OrdersList* ordli1 = new OrdersList();
    cout << "OrdersList created.\n";

    // ===== Create Player =====
    Player p1("Joe", territories, d1, ordli1);
    cout << "Player created.\n";

    // ===== Test methods =====
    auto defendList = p1.toDefend(p1);
    cout << "toDefend() returned " << defendList.size() << " territories\n";

    auto attackList = p1.toAttack(p1);
    cout << "toAttack() returned " << attackList.size() << " territories\n";

    p1.issueOrder();
    cout << "issueOrder() called successfully\n";

    // ===== Cleanup dynamically allocated memory =====
    delete d1;
    delete ordli1;
    for (auto* t : territories) delete t;
    for (auto* c : cards1) delete c;
    delete adj1;
    delete adj2;
    delete adj3;
    delete adj4;

    cout << "Cleanup complete.\n";
}