#include "GameEngine.h"
#include "Orders.h"
#include <algorithm>
#include <cctype>
#include <sstream> //issue
#include <iostream>
#include "LoggingObserver.h"
#include "PlayerStrategies.h"

namespace {
    // Map states to their display name.
    const char* toStr(GameState s) {
        switch (s) {
            case GameState::Start:               return "start";
            case GameState::MapLoaded:           return "map loaded";
            case GameState::MapValidated:        return "map validated";
            case GameState::PlayersAdded:        return "players added";
            case GameState::AssignReinforcement: return "assign reinforcement";
            case GameState::IssueOrders:         return "issue orders";
            case GameState::ExecuteOrders:       return "execute orders";
            case GameState::Win:                 return "win";
            case GameState::End:                 return "end";
            default:                             return "unknown";
        }
    }
}

GameEngine::GameEngine()
    : state_(GameState::Start)
    , reinforcementPool_(new std::unordered_map<Player*, int*>())
    , lastLogMessage_(new std::string("")) //added for A3 to initialize log buffer
{
    buildTransitions();
}

GameEngine::~GameEngine() {
     if (reinforcementPool_) {
        for (auto& kv : *reinforcementPool_) delete kv.second;
        delete reinforcementPool_;
        reinforcementPool_ = nullptr;
    }
    if (lastLogMessage_) { //added for A3 for deletion of lastLogMessage_
        delete lastLogMessage_;
        lastLogMessage_ = nullptr;
    }
}

/**
 * Return the current state value.
 */
std::string GameEngine::stateName() const {
    return toStr(state_);
}

// Helpers
std::string GameEngine::toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    return s;
}

// Helpers
std::string GameEngine::trim(const std::string& s) {
    auto b = s.begin();
    while (b != s.end() && std::isspace(static_cast<unsigned char>(*b))) ++b;
    auto e = s.end();
    do { --e; } while (e >= b && std::isspace(static_cast<unsigned char>(*e)));
    return std::string(b, e + 1);
}

// Initialize the transition table
void GameEngine::buildTransitions() {
    //   start --loadmap--> map loaded
    //   map loaded --validatemap--> map validated
    //   map validated --addplayer--> players added
    //   players added --assigncountries--> assign reinforcement
    //   assign reinforcement --issueorder--> issue orders
    //   issue orders --endissueorders--> execute orders
    //   execute orders --endexecorders--> assign reinforcement
    //   execute orders --win--> win
    //   win --play--> assign reinforcement
    //   win --end--> end

    transitions_.clear();

    // start
    transitions_[GameState::Start] = {
        {"loadmap", GameState::MapLoaded}
    };

    // map loaded
    transitions_[GameState::MapLoaded] = {
        {"validatemap", GameState::MapValidated}
    };

    // map validated
    transitions_[GameState::MapValidated] = {
        {"addplayer", GameState::PlayersAdded}
    };

    // players added
    transitions_[GameState::PlayersAdded] = {
    {"addplayer", GameState::PlayersAdded},          // stay in playersadded
    {"gamestart", GameState::AssignReinforcement}    // go to assignreinforcement
};

    // assign reinforcement
    transitions_[GameState::AssignReinforcement] = {
        {"issueorder", GameState::IssueOrders}
    };

    // issue orders
    transitions_[GameState::IssueOrders] = {
        {"endissueorders", GameState::ExecuteOrders}
    };

    // execute orders
    transitions_[GameState::ExecuteOrders] = {
        {"endexecorders", GameState::AssignReinforcement},
        {"win",           GameState::Win}
    };

    // win
    transitions_[GameState::Win] = {
        {"play", GameState::AssignReinforcement},
        {"quit",  GameState::End} //changed from "end" to "quit" for A3
    };

    // end has no outgoing transitions
    transitions_[GameState::End] = {};
}

/**
 * Attempt to apply a command to the current state.
 *
 * @param command Command string
 * @return true if a valid transition exists; false if rejected.
 */
bool GameEngine::processCommand(const std::string& in) {
    if (state_ == GameState::End) return false;

// Trim overall input
    std::string trimmed = trim(in);
    if (trimmed.empty()) return false;

    // Split into command word + arguments
    std::string cmdWord;
    std::string args;

    auto pos = trimmed.find(' ');
    if (pos == std::string::npos) {
        cmdWord = trimmed;
        args = "";
    } else {
        cmdWord = trimmed.substr(0, pos);
        args = trim(trimmed.substr(pos + 1));
    }

    const auto cmd = toLower(cmdWord);

    // Lookup transition using ONLY the command word
    const auto itState = transitions_.find(state_);
    if (itState == transitions_.end()) return false;

    const auto itCmd = itState->second.find(cmd);
    if (itCmd == itState->second.end()) {
        std::cout << "Invalid command. No transition available from current state.\n";
        return false;
    }
    GameState nextState = itCmd->second;
    bool success = true;
    // Perform side-effect for the command (before flipping state) (also added some extra checks for reloading and a bool value)
    if      (state_ == GameState::Start && cmd == "loadmap")        success = onLoadMap(args);
    else if (state_ == GameState::MapLoaded && cmd == "validatemap")  success = onValidateMap();
    else if ((state_ == GameState::MapValidated || state_ == GameState::PlayersAdded) && cmd == "addplayer") onAddPlayer(args); //changed to not hard code players now
    else if (state_ == GameState::PlayersAdded && cmd == "gamestart") onAssignCountries();
    else if (state_ == GameState::AssignReinforcement && cmd == "issueorder") onIssueOrder();
    else if (state_ == GameState::IssueOrders && cmd == "endissueorders") onEndIssueOrders();
    else if (state_ == GameState::ExecuteOrders && cmd == "endexecorders") onEndExecOrders();
    else if (state_ == GameState::ExecuteOrders && cmd == "win")           onWin();
    else if (state_ == GameState::Win && cmd == "play")                    onPlayAgain();
    else if (state_ == GameState::Win && cmd == "quit")                     onEnd(); //changed from "end" to quit

    //added a success check to transition to next state or not
     if (!success) {
        // side-effect failed (e.g., bad filename or invalid map) → do NOT change state
        std::cout << "Command side-effect failed; state remains " << stateName() << "\n";
        return false;
    }

    setState(nextState);
    std::cout << "Transitioned to state: " << stateName() << "\n";
    return true;
}

/**
 * Show the possible commands from the current state.
 *
 * @return A list of command strings that are accepted in the current state.
 */
std::vector<std::string> GameEngine::availableCommands() const {
    std::vector<std::string> cmds;
    const auto it = transitions_.find(state_);
    if (it == transitions_.end()) return cmds;
    cmds.reserve(it->second.size());
    for (const auto& kv : it->second) cmds.push_back(kv.first);
    return cmds;
}

/**
 * Handles the "loadmap" command.
 *
 */
bool GameEngine::onLoadMap(const std::string& path) { //changed to bool for fixing A2 for A3
     
    if (path.empty()) {
        std::cout << "[loadmap] No filename provided.\n"; //no longer hard coded
        return false;
    }
    const bool ok = loader_.loadMap(path);
    map_ = loader_.getMap();
    if (ok && map_) {
        std::cout << "[loadmap] Loaded " << path << "\n";
        return true;
    } else {
        std::cout << "[loadmap] Failed to load " << path << "\n";
        return false;
    }
}

/**
 * Handles the "validatemap" command.
 *
 */
bool GameEngine::onValidateMap() {  //changed to bool for fixing A2 for A3
    if (!map_) {
        std::cout << "[validatemap] No map loaded.\n";
        return false;
    }
    const bool ok = map_->validate();
    std::cout << (ok ? "[validatemap] Map is valid.\n"
                     : "[validatemap] Map is invalid.\n");
    return ok;
}

/**
 * Releases all dynamically allocated Player objects.
 *
 */
void GameEngine::clearPlayers() {
    for (auto* p : players_) delete p;
    players_.clear();
}

/**
 * Handles the "addplayer" command.
 *
 */
void GameEngine::onAddPlayer(const std::string& name) {
    //clearPlayers();   removed this because we don't want to clear players on every call

    if (name.empty()) {
        std::cout << "[addplayer] No player name provided.\n";
        return;
    }

    // 2–6 players constraint
    if (players_.size() >= 6) {
        std::cout << "[addplayer] Cannot add more than 6 players.\n";
        return;
    }

    // prevent duplicate names
    for (auto* p : players_) {
        if (p->getPName() == name) {
            std::cout << "[addplayer] Player '" << name << "' already exists.\n";
            return;
        }
    }

    std::vector<Territory*> none;
    Player* p = new Player(name, none, new Deck(), new OrdersList());
    players_.push_back(p);

    std::cout << "[addplayer] Added player " << name
              << " (total " << players_.size() << ").\n";
}

/**
 * Assigns territories to players in round-robin fashion.
 *
 */
void GameEngine::distributeRoundRobin() {
    if (!map_) return;
    auto* terrs = map_->getTerritories();
    if (!terrs || terrs->empty() || players_.empty()) return;

    size_t pi = 0;
    for (auto* t : *terrs) {
        t->setOwner(players_[pi]->getPName());
        auto v = players_[pi]->getTerritory();
        v.push_back(t);
        players_[pi]->setTerritory(v);
        pi = (pi + 1) % players_.size();
    }
}

/**
 * Handles the "assigncountries" command.
 *
 */
void GameEngine::onAssignCountries() {
    if (!map_) {
        std::cout << "[assigncountries] No map loaded.\n";
        return;
    }
    if (players_.empty()) {
        std::cout << "[assigncountries] No players available.\n";
        return;
    }
    distributeRoundRobin();
    std::cout << "[assigncountries] Territories distributed to players.\n";
    state_ = GameState::AssignReinforcement;
}

/**
 * Handles the "issueorder" command.
 *
 */
void GameEngine::onIssueOrder() {
    for (auto* p : players_) {
        auto owned = p->getTerritory();
        if (owned.empty()) continue;

        int* one = new int(1);
        Orders* o = new Deploy(p, owned.front(), one);

        OrdersList* ol = p->getOrder();   // FIX: pointer
        ol->add(o);                       // FIX: call on pointer
        p->setOrdersList(ol);             // FIX: set pointer

        std::cout << "[issueorder] " << p->getPName()
                  << " issued Deploy(1) to " << owned.front()->getName() << "\n";
    }
}

/**
 * Handles the "endissueorders" command.
 *
 */
void GameEngine::onEndIssueOrders() {
    std::cout << "[endissueorders] Orders locked for execution.\n";
}

/**
 * Handles the "endexecorders" command.
 *
 */
void GameEngine::onEndExecOrders() {
    std::cout << "[endexecorders] Execution completed (placeholder). Returning to reinforcement.\n";
}

/**
 * Handles the "win" command.
 *
 */
void GameEngine::onWin() {
    std::cout << "[win] A winning condition was signaled.\n";
}

/**
 * Handles the "play" command from the Win state.
 *
 */
void GameEngine::onPlayAgain() {
    std::cout << "[play] New play cycle requested. Reinforcements will be assigned.\n";
}

/**
 * Handles the "end" command.
 *
 */
void GameEngine::onEnd() {
    std::cout << "[end] Terminating program.\n";
    clearPlayers();
}

//----------------------------------------------------------------------//
//--------------------------------A2------------------------------------//
//----------------------------------------------------------------------//

void GameEngine::startupPhase(){
    //create game engine an validate map
    GameEngine engine1;
    
    //create map loader and load map file
    engine1.onLoadMap("maps/valid_map1.map");
    //MapLoader loader;
    //std::string file1 = "maps/valid_map1.map";
    //loader.loadMap(file1);


    //validate the map
    engine1.onValidateMap();

    //addplayers
    engine1.onAddPlayer("Alice"); //given an argument for driver example

    //game start phase
    //assign territories to players
    engine1.onAssignCountries();
    
    //player random order

    //let player draw 2 cards
    std::vector<Player *> players_;
    Deck* deck;
    Hand h;
    for(int i=0; i<players_.size(); i++){
        deck = players_[i]->getDeck();
        deck->draw(h);
        std::cout<<"Cards drawn\n" << deck; //not getting done
    }
    
    //switch to play phase
    engine1.onIssueOrder();
    std::cout<<"Play started";
    
}

std::string GameEngine::stringToLog() const {//Added for A3
    // If tournament or something else set a special message, use it.
    if (lastLogMessage_ && !lastLogMessage_->empty()) {
        return *lastLogMessage_;
    }
    // Fallback: state change log (A2)
    return std::string("STATE_CHANGE | ") + stateName();
}


// ====== Part 3: Reinforcement / Issue Orders / Execute Orders ======

int GameEngine::continentBonusFor(Player* /*p*/) const {
    // Continent class in Map.cpp does not (yet) store a control bonus.
    // Return 0 here. 
    return 0;
}

int GameEngine::computeReinforcementsFor(Player* p) const {
    const int terrCount = static_cast<int>(p->getTerritory().size());
    int base = terrCount / 3;
    if (base < 3) base = 3;
    base += continentBonusFor(p);
    return base;
}

void GameEngine::reinforcementPhase() {
    if (!map_ || players_.empty()) {
        std::cout << "[reinforcement] Skipped (no map or no players)\n";
        return;
    }

    // Recompute reinforcement pool for all players
    for (auto* p : players_) {
        const int r = computeReinforcementsFor(p);
        // store heap ints to respect pointer-type user-defined members guideline
        auto it = reinforcementPool_->find(p);
        if (it == reinforcementPool_->end()) {
            (*reinforcementPool_)[p] = new int(r);
        } else {
            *it->second = r;
        }
        std::cout << "[reinforcement] " << p->getPName() << " receives " << r << " armies.\n";
    }

    if (state_ == GameState::AssignReinforcement) {
        state_ = GameState::IssueOrders;
        std::cout << "Transitioned to state: " << stateName() << "\n";
    }
}

void GameEngine::issueOrdersPhase() {
    if (players_.empty()) {
        std::cout << "[issueOrders] No players.\n";
        return;
    }

    // PHASE 1 — While anyone still has reinforcement pool > 0, only issue Deploy(1)
    bool deployedSomething = true;
    while (deployedSomething) {
        deployedSomething = false;
        for (auto* p : players_) {
            auto it = reinforcementPool_->find(p);
            const int left = (it == reinforcementPool_->end() ? 0 : *it->second);
            if (left <= 0) continue;

            // Pick a territory to deploy to (first owned)
            auto owned = p->getTerritory();
            if (owned.empty()) continue;

            int* one = new int(1);
            Orders* dep = new Deploy(p, owned.front(), one);
            p->getOrder()->add(dep);

            // decrement pool
            *(*reinforcementPool_)[p] = left - 1;
            deployedSomething = true;

            std::cout << "[issueOrders] " << p->getPName()
                      << " issues Deploy(1) to " << owned.front()->getName()
                      << " (pool left=" << *(*reinforcementPool_)[p] << ")\n";
        }
    }

    // PHASE 2 — Round-robin: after pools are zero, allow one “non-deploy” example order per player
    bool issuedNonDeploy = false;
    for (auto* p : players_) {
        auto owned = p->getTerritory();
        if (owned.empty()) continue;

        Territory* chosenSrc = nullptr;
        Territory* chosenDst = nullptr;

        // 1) Prefer attacking an adjacent enemy
        for (auto* src : owned) {
            auto* adj = src->getAdjacentTerritories(); // vector<Territory*>*
            if (!adj) continue;
            for (auto* nbr : *adj) {
                if (nbr->getOwner() != p->getPName()) { // enemy neighbor
                    chosenSrc = src;
                    chosenDst = nbr;
                    break;
                }
            }
            if (chosenSrc) break;
        }

        // 2) Otherwise, move within own adjacency (fortify)
        if (!chosenSrc) {
            for (auto* src : owned) {
                auto* adj = src->getAdjacentTerritories();
                if (!adj) continue;
                for (auto* nbr : *adj) {
                    if (nbr->getOwner() == p->getPName()) { // friendly neighbor
                        chosenSrc = src;
                        chosenDst = nbr;
                        break;
                    }
                }
                if (chosenSrc) break;
            }
        }

        if (chosenSrc && chosenDst) {
            int* one = new int(1);
            Orders* adv = new Advance(p, chosenDst, chosenSrc, one);
            p->getOrder()->add(adv);
            issuedNonDeploy = true;

            std::cout << "[issueOrders] " << p->getPName()
                    << " issues Advance(1) " << chosenSrc->getName()
                    << " -> " << chosenDst->getName() << "\n";
        }
    }

    // move to execution state
    if (state_ == GameState::IssueOrders) {
        state_ = GameState::ExecuteOrders;
        std::cout << "Transitioned to state: " << stateName() << "\n";
    }
}

bool GameEngine::anyOrdersRemain() const {
    for (auto* p : players_) {
        if (!p->getOrder()->getOrders().empty()) return true;
    }
    return false;
}

void GameEngine::removeDefeatedPlayers() {
    // Remove any player with 0 territories
    std::vector<Player*> survivors;
    survivors.reserve(players_.size());
    for (auto* p : players_) {
        if (p->getTerritory().empty()) {
            std::cout << "[executeOrders] Removing defeated player: " << p->getPName() << "\n";
            // cleanup per-pool entry
            auto it = reinforcementPool_->find(p);
            if (it != reinforcementPool_->end()) {
                delete it->second;
                reinforcementPool_->erase(it);
            }
            delete p;
        } else {
            survivors.push_back(p);
        }
    }
    players_.swap(survivors);
}

bool GameEngine::checkWinAndMaybeEnterWinState() {
    if (!map_) return false;
    const size_t total = map_->getTerritories() ? map_->getTerritories()->size() : 0;
    if (total == 0) return false;

    for (auto* p : players_) {
        if (p->getTerritory().size() == total) {
            std::cout << "[executeOrders] " << p->getPName() << " controls all territories! WIN\n";
            state_ = GameState::Win;
            std::cout << "Transitioned to state: " << stateName() << "\n";
            return true;
        }
    }
    return false;
}

void GameEngine::executeOrdersPhase() {
    if (players_.empty()) {
        std::cout << "[executeOrders] No players.\n";
        return;
    }

    // Round-robin: grab top order from each player's list and execute, repeat until all empty
    while (anyOrdersRemain()) {
        for (auto* p : players_) {
            OrdersList* ol = p->getOrder();
            auto v = ol->getOrders();           // snapshot (vector of pointers)
            if (v.empty()) continue;

            Orders* top = v.front();
            if (top) {
                // Detect if this is an Advance against a Neutral player
                Player* defenderBefore = nullptr;
                Advance* adv = dynamic_cast<Advance*>(top);
                if (adv) {
                    // Copy of the target territory *before* execution
                    Territory targBefore = adv->getTarg();
                    std::string defenderName = targBefore.getOwner();

                    // Find the corresponding Player*, if any
                    for (auto* candidate : players_) {
                        if (candidate && candidate->getPName() == defenderName) {
                            defenderBefore = candidate;
                            break;
                        }
                    }
                }

                bool ok = top->execute();

                // If that defender was Neutral, switch to Aggressive 
                if (ok && defenderBefore && defenderBefore->isNeutral()) {
                    defenderBefore->setStrategy(new AggressivePlayerStrategy());
                    std::cout << "[executeOrders] Player \""
                              << defenderBefore->getPName()
                              << "\" was Neutral and has become Aggressive after being attacked.\n";
                }
                
                std::cout << "[executeOrders] " << p->getPName()
                          << " executes " << typeid(*top).name()
                          << " -> " << (ok ? "OK" : "INVALID") << "\n";
                ol->remove(top); // also deletes the order
            }
        }

        // Players with 0 territories are removed (per rules)
        removeDefeatedPlayers();
        if (checkWinAndMaybeEnterWinState()) return; // stop if someone won
    }

    // Loop back to reinforcement
    if (state_ == GameState::ExecuteOrders) {
        state_ = GameState::AssignReinforcement;
        std::cout << "Transitioned to state: " << stateName() << "\n";
    }

    
    
    
    

}

// ===== A3: Tournament Mode =================================================

void GameEngine::runTournament(const std::vector<std::string>& mapFiles,
                               const std::vector<std::string>& playerStrategies,
                               int gamesPerMap,
                               int maxTurns)
{
    // Results table: results[mapIndex][gameIndex] = winner name or "Draw"
    std::vector<std::vector<std::string>> results(
        mapFiles.size(),
        std::vector<std::string>(gamesPerMap, "Draw")
    );

    for (size_t m = 0; m < mapFiles.size(); ++m) {
        const std::string& mapName = mapFiles[m];

        for (int g = 0; g < gamesPerMap; ++g) {
            // ---- create a fresh engine per game so games are independent ----
            GameEngine game;  // uses same rules/phases as normal game

            // ---- load and validate map (no console interaction) ----
            bool okMap = game.loader_.loadMap(mapName);
            game.map_ = game.loader_.getMap();
            if (!okMap || !game.map_ || !game.map_->validate()) {
                std::cout << "[tournament] ERROR: cannot use map '" << mapName << "'\n";
                results[m][g] = "InvalidMap";
                continue;
            }

            // ---- create players according to strategy names ----
            game.clearPlayers();
            std::vector<Territory*> none;

            for (const auto& stratName : playerStrategies) {
                // Player name = strategy name (Aggressive, Benevolent, etc.)
                // Strategy objects will be attached in Part 1 of A3 inside Player.
                Deck* d       = new Deck();
                OrdersList* o = new OrdersList();
                Player* p     = new Player(stratName, none, d, o);
                game.players_.push_back(p);
            }

            // ---- initial territory assignment ----
            game.distributeRoundRobin();

            // ---- very simple startup for play phase ----
            game.state_ = GameState::AssignReinforcement;

            // Each player draws 2 cards like startup phase
            for (auto* p : game.players_) {
                Deck* d = p->getDeck();
                if (d) {
                    Hand tempHand;
                    d->draw(tempHand);
                    d->draw(tempHand);
                }
            }

            // ---- play loop: reinforcement -> issue -> execute, up to maxTurns ----
            std::string winner = "Draw";

            for (int turn = 0; turn < maxTurns; ++turn) {
                std::cout << "\n[tournament] Map: " << mapName
                          << " Game: " << (g + 1)
                          << " Turn: " << (turn + 1) << "\n";

                game.reinforcementPhase();
                game.issueOrdersPhase();
                game.executeOrdersPhase();

                // Remove defeated players and check winner
                game.removeDefeatedPlayers();
                if (game.checkWinAndMaybeEnterWinState()) {
                    // Whoever owns all territories is the winner
                    size_t total = game.map_->getTerritories()
                                 ? game.map_->getTerritories()->size()
                                 : 0;

                    for (auto* p : game.players_) {
                        if (p->getTerritory().size() == total) {
                            winner = p->getPName();
                            break;
                        }
                    }
                    break; // stop this game
                }

                // If all players still alive but no winner, continue until maxTurns
            }

            results[m][g] = winner;
            game.clearPlayers(); // clean up for this game
        }
    }

    // ---- Build result summary exactly like the assignment wants ----
    std::ostringstream ss;
    ss << "Tournament Mode:\n";
    ss << "M: ";
    for (size_t i = 0; i < mapFiles.size(); ++i) {
        ss << mapFiles[i];
        if (i + 1 < mapFiles.size()) ss << " ";
    }
    ss << "\nP: ";
    for (size_t i = 0; i < playerStrategies.size(); ++i) {
        ss << playerStrategies[i];
        if (i + 1 < playerStrategies.size()) ss << ", ";
    }
    ss << "\nG: " << gamesPerMap << "\n";
    ss << "D: " << maxTurns << "\n\n";

    ss << "Results:\n";
    ss << "           ";
    for (int g = 0; g < gamesPerMap; ++g) {
        ss << "Game" << (g + 1) << "   ";
    }
    ss << "\n";

    for (size_t m = 0; m < mapFiles.size(); ++m) {
        ss << mapFiles[m] << "   ";
        for (int g = 0; g < gamesPerMap; ++g) {
            ss << results[m][g] << "   ";
        }
        ss << "\n";
    }

    std::string summary = ss.str();
    std::cout << "\n" << summary << "\n";

    if (lastLogMessage_) {
        *lastLogMessage_ = summary; // will be logged by LoggingObserver
    }
    notify();  // log one big entry for the tournament
}

