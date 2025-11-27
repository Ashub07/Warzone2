#include <iostream>
#include <vector>
#include "Player.h"
#include "Map.h"
#include "Cards.h"
#include "PlayerStrategies.h"

using namespace std;

// Small helper to show a list of territory names (for debugging / demo).
static void printTerritoryList(const std::string& label, const std::vector<Territory*>& terrs) {
    cout << label << " (size = " << terrs.size() << "): ";
    for (auto* t : terrs) {
        if (t) cout << t->getName() << " ";
    }
    cout << "\n";
}

void testPlayerStrategies() {
    cout << "===== testPlayerStrategies(): START =====\n\n";

    // 1. Build a tiny map with a few territories + adjacency
    auto* adj1 = new std::vector<Territory*>();
    auto* adj2 = new std::vector<Territory*>();
    auto* adj3 = new std::vector<Territory*>();
    auto* adj4 = new std::vector<Territory*>();

    // Territories:
    //  - T1 owned by Human
    //  - T2 owned by Aggressive
    //  - T3 owned by Benevolent
    //  - T4 owned by Cheater
    Territory* t1 = new Territory("T1", "C1", "Human",      5, 1, adj1);
    Territory* t2 = new Territory("T2", "C1", "Aggressive", 3, 2, adj2);
    Territory* t3 = new Territory("T3", "C1", "Benevolent", 4, 3, adj3);
    Territory* t4 = new Territory("T4", "C1", "Cheater",    6, 4, adj4);

    // Adjacency: Cheater (T4) is adjacent to T2 and T3 
    adj1->push_back(t2);                       // T1 adjacent to T2
    adj2->push_back(t1);                       // T2 adjacent to T1
    adj2->push_back(t4);                       // T2 adjacent to T4 (cheater)
    adj3->push_back(t4);                       // T3 adjacent to T4 (cheater)
    adj4->push_back(t2);                       // T4 adjacent to T2
    adj4->push_back(t3);                       // T4 adjacent to T3

    Map map;
    map.addTerritory(t1);
    map.addTerritory(t2);
    map.addTerritory(t3);
    map.addTerritory(t4);

    cout << "Map created with 4 territories: T1, T2, T3, T4.\n";
    cout << "Cheater owns T4, which is adjacent to T2 and T3.\n\n";

    // 2. Create players and assign territories
    Player human;
    human.setPName("Human");
    std::vector<Territory*> humanTerrs = { t1 };
    human.setTerritory(humanTerrs);
    human.addReinforcements(5);

    Player aggressive;
    aggressive.setPName("Aggressive");
    std::vector<Territory*> aggrTerrs = { t2 };
    aggressive.setTerritory(aggrTerrs);
    aggressive.addReinforcements(5);

    Player benevolent;
    benevolent.setPName("Benevolent");
    std::vector<Territory*> beneTerrs = { t3 };
    benevolent.setTerritory(beneTerrs);
    benevolent.addReinforcements(5);

    Player neutral;
    neutral.setPName("Neutral");
    std::vector<Territory*> neutralTerrs = { t1 }; // give neutral some land to "defend"
    neutral.setTerritory(neutralTerrs);
    neutral.addReinforcements(5);

    Player cheater;
    cheater.setPName("Cheater");
    std::vector<Territory*> cheatTerrs = { t4 };
    cheater.setTerritory(cheatTerrs);
    cheater.addReinforcements(5);

    // 3. Put some cards in the decks so draw() never returns nullptr

    // Human deck
    if (Deck* d = human.getDeck()) {
        d->addBack(new Card(cardType::Reinforcement));
    }
    // Aggressive deck
    if (Deck* d = aggressive.getDeck()) {
        d->addBack(new Card(cardType::Bomb));
        d->addBack(new Card(cardType::Reinforcement));
    }
    // Benevolent deck
    if (Deck* d = benevolent.getDeck()) {
        d->addBack(new Card(cardType::Blockade));
        d->addBack(new Card(cardType::Airlift));
        d->addBack(new Card(cardType::Diplomacy));
    }
    // Neutral / Cheater never use cards

    // 4. Create one instance of each strategy
    HumanPlayerStrategy        humanStrat;
    AggressivePlayerStrategy   aggressiveStrat;
    BenevolentPlayerStrategy   benevolentStrat;
    NeutralPlayerStrategy      neutralStrat;
    CheaterPlayerStrategy      cheaterStrat;

    // 5. Demonstrate HUMAN player strategy
    cout << "===== HUMAN PLAYER STRATEGY =====\n";
    cout << "[Human] toDefend() – user chooses territories.\n";
    auto humanDef = humanStrat.toDefend(human);
    printTerritoryList("Human toDefend list", humanDef);

    cout << "[Human] toAttack() – user chooses territories.\n";
    auto humanAtk = humanStrat.toAttack(map, human);
    printTerritoryList("Human toAttack list", humanAtk);

    cout << "[Human] issueOrder() – expect interactive behavior and card usage.\n";
    humanStrat.issueOrder(human, map);
    cout << "\n";

    // 6. Demonstrate AGGRESSIVE player strategy
    cout << "===== AGGRESSIVE PLAYER STRATEGY =====\n";
    auto aggrDef = aggressiveStrat.toDefend(aggressive);
    printTerritoryList("Aggressive toDefend list", aggrDef);

    auto aggrAtk = aggressiveStrat.toAttack(map, aggressive);
    printTerritoryList("Aggressive toAttack list", aggrAtk);

    cout << "[Aggressive] issueOrder() – expect reinforcement on strongest and attacking behavior.\n";
    aggressiveStrat.issueOrder(aggressive, map);
    cout << "\n";

    // 7. Demonstrate BENEVOLENT player strategy
    cout << "===== BENEVOLENT PLAYER STRATEGY =====\n";
    auto beneDef = benevolentStrat.toDefend(benevolent);
    printTerritoryList("Benevolent toDefend list (weakest first)", beneDef);

    cout << "[Benevolent] toAttack() – prints that this player does not attack.\n";
    benevolentStrat.toAttack(map, benevolent);

    cout << "\n[Benevolent] issueOrder() – expect reinforcement of weakest territories + possible card play.\n";
    benevolentStrat.issueOrder(benevolent, map);
    cout << "\n";

    // 8. Demonstrate NEUTRAL player strategy + dynamic switch
    cout << "===== NEUTRAL PLAYER STRATEGY =====\n";
    auto neutDef = neutralStrat.toDefend(neutral);
    printTerritoryList("Neutral toDefend list (all owned territories)", neutDef);

    auto neutAtk = neutralStrat.toAttack(map, neutral);
    printTerritoryList("Neutral toAttack list (should be empty)", neutAtk);

    cout << "[Neutral] issueOrder() – should print that no orders are issued.\n";
    neutralStrat.issueOrder(neutral, map);

    cout << "\n--- Simulating that Neutral has been attacked ---\n";
    cout << "Switching behavior to AggressivePlayerStrategy for Neutral player.\n";
    AggressivePlayerStrategy neutralAggressiveStrat;
    auto neutAggDef = neutralAggressiveStrat.toDefend(neutral);
    printTerritoryList("Neutral (now Aggressive) toDefend", neutAggDef);
    auto neutAggAtk = neutralAggressiveStrat.toAttack(map, neutral);
    printTerritoryList("Neutral (now Aggressive) toAttack", neutAggAtk);

    cout << "[Neutral→Aggressive] issueOrder() – should now behave like an aggressive AI.\n";
    neutralAggressiveStrat.issueOrder(neutral, map);
    cout << "\n";

    // 9. Demonstrate CHEATER player strategy
    cout << "===== CHEATER PLAYER STRATEGY =====\n";
    auto cheatDef = cheaterStrat.toDefend(cheater);
    printTerritoryList("Cheater toDefend list (all owned territories)", cheatDef);

    auto cheatAtk = cheaterStrat.toAttack(map, cheater);
    printTerritoryList("Cheater toAttack list (adjacent enemy territories)", cheatAtk);

    cout << "[Cheater] issueOrder() – should automatically conquer all adjacent enemy territories once.\n";
    cheaterStrat.issueOrder(cheater, map);
    cout << "\n";

    cout << "===== testPlayerStrategies(): END =====\n\n";

}

int main() {
    testPlayerStrategies();
    return 0;
}
