#include "Orders.h"
#include <iostream>
#include <vector>

void testOrdersList() {
    std::cout << "----- Orders Driver Test -----" << std::endl;

    // ===== Territories =====
    std::cout << "[DEBUG] Creating territories..." << std::endl;

    // create explicit empty adjacency vectors so the Territory ctor never sees nullptr
    auto* adj1 = new std::vector<Territory*>();
    auto* adj2 = new std::vector<Territory*>();
    auto* adj3 = new std::vector<Territory*>();
    auto* adj4 = new std::vector<Territory*>();

    std::vector<Territory*> territories1;
    Territory* t1 = new Territory("T1", "C1", "Joe", 2, 1, adj1);
    territories1.push_back(t1);
    Territory* t2 = new Territory("T2", "C2", "Joe", 3, 2, adj2);
    territories1.push_back(t2);

    // ===== Deck for p1 =====
    std::cout << "[DEBUG] Creating deck for p1..." << std::endl;
    std::vector<Card*> cards1;
    cards1.push_back(new Card(cardType::Bomb));
    cards1.push_back(new Card(cardType::Diplomacy));
    cards1.push_back(new Card(cardType::Airlift));
    Deck* d1 = new Deck(cards1);

    OrdersList* ordli1 = new OrdersList();

    std::cout << "[DEBUG] Creating Player p1..." << std::endl;
    Player* p1 = new Player("Joe", territories1, d1, ordli1);

    // ===== Territories for p2 =====
    std::cout << "[DEBUG] Creating territories for p2..." << std::endl;

    auto* adj5 = new std::vector<Territory*>();
    auto* adj6 = new std::vector<Territory*>();

    std::vector<Territory*> territories2;
    Territory* t11 = new Territory("T11", "C1", "Joee", 2, 1, adj5);
    territories2.push_back(t11);
    Territory* t22 = new Territory("T22", "C2", "Joee", 3, 2, adj6);
    territories2.push_back(t22);

    // ===== Deck for p2 =====
    std::cout << "[DEBUG] Creating deck for p2..." << std::endl;
    std::vector<Card*> cards2;
    cards2.push_back(new Card(cardType::Bomb));
    cards2.push_back(new Card(cardType::Diplomacy));
    cards2.push_back(new Card(cardType::Airlift));
    Deck* d2 = new Deck(cards2);

    OrdersList* ordli2 = new OrdersList();

    std::cout << "[DEBUG] Creating Player p2..." << std::endl;
    Player* p2 = new Player("Joee", territories2, d2, ordli2);

    // ===== Orders =====
    std::cout << "[DEBUG] Creating orders..." << std::endl;
    int* num = new int(2);
    Deploy*   dep = new Deploy(p2, t11, num);
    Blockade* blk = new Blockade(p2, t22);
    Advance*  adv = new Advance(p2, t11, t1, num);
    Airlift*  air = new Airlift(p2, t11, t22, num);
    Bomb*     bom = new Bomb(p2, t22);
    Negotiate* neg = new Negotiate(p2, p1);

    // ===== Add orders to list =====
    std::cout << "[DEBUG] Adding orders to list..." << std::endl;
    ordli2->add(dep);  std::cout << "[DEBUG] Added Deploy" << std::endl;
    ordli2->add(blk);  std::cout << "[DEBUG] Added Blockade" << std::endl;
    ordli2->add(adv);  std::cout << "[DEBUG] Added Advance" << std::endl;
    ordli2->add(air);  std::cout << "[DEBUG] Added Airlift" << std::endl;
    ordli2->add(bom);  std::cout << "[DEBUG] Added Bomb" << std::endl;
    ordli2->add(neg);  std::cout << "[DEBUG] Added Negotiate" << std::endl;

    // ===== Print orders =====
    std::cout << "[DEBUG] Printing orders..." << std::endl;
    std::cout << *ordli2 << std::endl;

    std::cout << "[DEBUG] Test complete!" << std::endl;

    
}



