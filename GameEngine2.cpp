#include "GameEngine.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>

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

GameEngine::GameEngine() : state_(GameState::Start), mapFilename_("") {
    buildTransitions();
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
    transitions_.clear();

    transitions_[GameState::Start] = {{"loadmap", GameState::MapLoaded}};
    transitions_[GameState::MapLoaded] = {{"validatemap", GameState::MapValidated}};
    transitions_[GameState::MapValidated] = {{"addplayer", GameState::PlayersAdded}};
    transitions_[GameState::PlayersAdded] = {{"assigncountries", GameState::AssignReinforcement}};
    transitions_[GameState::AssignReinforcement] = {{"issueorder", GameState::IssueOrders}};
    transitions_[GameState::IssueOrders] = {{"endissueorders", GameState::ExecuteOrders}};
    transitions_[GameState::ExecuteOrders] = {
        {"endexecorders", GameState::AssignReinforcement},
        {"win", GameState::Win}
    };
    transitions_[GameState::Win] = {
        {"play", GameState::AssignReinforcement},
        {"end", GameState::End}
    };
    transitions_[GameState::End] = {};
}

/**
 * Attempt to apply a command to the current state.
 */
bool GameEngine::processCommand(const std::string& in) {
    if (state_ == GameState::End) return false;

    std::string trimmedInput = trim(in);
    if (trimmedInput.empty()) return false;

    // Parse command and arguments
    std::string cmd;
    std::string args;
    size_t spacePos = trimmedInput.find(' ');

    if (spacePos != std::string::npos) {
        cmd = toLower(trimmedInput.substr(0, spacePos));
        args = trim(trimmedInput.substr(spacePos + 1));
    } else {
        cmd = toLower(trimmedInput);
    }

    // Lookup transition using just the command name (not args)
    const auto itState = transitions_.find(state_);
    if (itState == transitions_.end()) return false;
    const auto itCmd = itState->second.find(cmd);
    if (itCmd == itState->second.end()) {
        std::cout << "Invalid command '" << cmd << "'. No transition available from current state.\n";
        return false;
    }

    // Track success
    bool success = false;

    // Perform side-effect for the command
    if (state_ == GameState::Start && cmd == "loadmap") {
        if (args.empty()) {
            std::cout << "Error: loadmap requires a filename. Usage: loadmap <filename>\n";
            return false;
        }
        mapFilename_ = args;
        onLoadMap();
        success = (map_ != nullptr);
    }
    else if (state_ == GameState::MapLoaded && cmd == "validatemap") {
        onValidateMap();
        success = (map_ != nullptr && map_->validate());
    }
    else if (state_ == GameState::MapValidated && cmd == "addplayer") {
        onAddPlayer();
        success = true;
    }
    else if (state_ == GameState::PlayersAdded && cmd == "assigncountries") {
        onAssignCountries();
        success = !players_.empty();
    }
    else if (state_ == GameState::AssignReinforcement && cmd == "issueorder") {
        onIssueOrder();
        success = true;
    }
    else if (state_ == GameState::IssueOrders && cmd == "endissueorders") {
        onEndIssueOrders();
        success = true;
    }
    else if (state_ == GameState::ExecuteOrders && cmd == "endexecorders") {
        onEndExecOrders();
        success = true;
    }
    else if (state_ == GameState::ExecuteOrders && cmd == "win") {
        onWin();
        success = true;
    }
    else if (state_ == GameState::Win && cmd == "play") {
        onPlayAgain();
        success = true;
    }
    else if (state_ == GameState::Win && cmd == "end") {
        onEnd();
        success = true;
    }

    // Only transition if successful
    if (success) {
        state_ = itCmd->second;
        std::cout << "Transitioned to state: " << stateName() << "\n";
        return true;
    } else {
        std::cout << "Command failed. Remaining in state: " << stateName() << "\n";
        return false;
    }
}

std::vector<std::string> GameEngine::availableCommands() const {
    std::vector<std::string> cmds;
    const auto it = transitions_.find(state_);
    if (it == transitions_.end()) return cmds;
    cmds.reserve(it->second.size());
    for (const auto& kv : it->second) cmds.push_back(kv.first);
    return cmds;
}

void GameEngine::onLoadMap() {
    if (mapFilename_.empty()) {
        std::cout << "[loadmap] ERROR: No map file specified.\n";
        map_ = nullptr;
        return;
    }

    std::cout << "[loadmap] Attempting to load: " << mapFilename_ << "\n";
    const bool ok = loader_.loadMap(mapFilename_);

    if (ok) {
        map_ = loader_.getMap();
        if (map_) {
            std::cout << "[loadmap] SUCCESS: Loaded " << mapFilename_ << "\n";
        } else {
            std::cout << "[loadmap] ERROR: Map loaded but getMap() returned nullptr.\n";
        }
    } else {
        map_ = nullptr;
        std::cout << "[loadmap] ERROR: Failed to load '" << mapFilename_ << "'.\n";
    }
}

void GameEngine::onValidateMap() {
    if (!map_) {
        std::cout << "[validatemap] ERROR: No map loaded.\n";
        return;
    }
    const bool ok = map_->validate();
    std::cout << (ok ? "[validatemap] SUCCESS: Map is valid." : "[validatemap] ERROR: Map is NOT valid.") << "\n";
}

void GameEngine::clearPlayers() {
    for (auto* p : players_) delete p;
    players_.clear();
}

void GameEngine::onAddPlayer() {
    clearPlayers();

    std::vector<Territory*> none;
    Deck* d = new Deck();
    OrdersList* ol = new OrdersList();

    players_.push_back(new Player("Alice", none, d, ol));
    players_.push_back(new Player("Bob",   none, d, ol));

    std::cout << "[addplayer] Created " << players_.size() << " players.\n";
}

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
}

void GameEngine::onIssueOrder() {
    for (auto* p : players_) {
        auto owned = p->getTerritory();
        if (owned.empty()) continue;

        int* one = new int(1);
        Orders* o = new Deploy(p, owned.front(), one);

        OrdersList* ol = p->getOrder();
        ol->add(o);
        p->setOrdersList(ol);

        std::cout << "[issueorder] " << p->getPName()
                  << " issued Deploy(1) to " << owned.front()->getName() << "\n";
    }
}

void GameEngine::onEndIssueOrders() {
    std::cout << "[endissueorders] Orders locked for execution.\n";
}

void GameEngine::onEndExecOrders() {
    std::cout << "[endexecorders] Execution completed (placeholder). Returning to reinforcement.\n";
}

void GameEngine::onWin() {
    std::cout << "[win] A winning condition was signaled.\n";
}

void GameEngine::onPlayAgain() {
    std::cout << "[play] New play cycle requested. Reinforcements will be assigned.\n";
}

void GameEngine::onEnd() {
    std::cout << "[end] Terminating program.\n";
    clearPlayers();
}

void startupPhase(){
    GameEngine engine1;
    engine1.onLoadMap();
    engine1.onValidateMap();
    engine1.onAddPlayer();
    engine1.onAssignCountries();

    std::vector<Player *> players_;
    Deck* deck;
    Hand h;
    for(int i=0; i<players_.size(); i++){
        deck = players_[i]->getDeck();
        deck->draw(h);
        std::cout<<"Cards drawn\n" << deck;
    }

    engine1.onIssueOrder();
    std::cout<<"Play started";
}
