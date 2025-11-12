#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <map>
#include <string>
#include <vector>
#include "Map.h"
#include "Player.h"
#include "Orders.h"
#include "Cards.h"

// ================== Game States ==================
// Enum representing the different states of the game
enum class GameState {
    Start,
    MapLoaded,
    MapValidated,
    PlayersAdded,
    AssignReinforcement,
    IssueOrders,
    ExecuteOrders,
    Win,
    End
};

// ================== GameEngine ==================
// Controls the main flow of the game and state transitions
class GameEngine {
private:
    GameState state_;   // current state
    std::map<GameState, std::map<std::string, GameState>> transitions_;  // transition table

    MapLoader loader_;      // loads maps from file
    Map* map_ = nullptr;    // pointer to the current map
    std::vector<Player*> players_;   // players in the game

    // Helpers
    static std::string toLower(std::string s);
    static std::string trim(const std::string& s);

    // Internal methods to build and clear state
    void buildTransitions();
    void clearPlayers();

    bool checkWinCondition();
    void removeDefeatedPlayers();

public:
    // ===== Constructor =====
    GameEngine();

    // ===== Accessors =====
    std::string stateName() const;       // returns current state's name
    GameState getState() const { return state_; }  
    GameState state() const { return state_; } 

    // ===== Core Methods =====
    bool processCommand(const std::string& in);        // process a command
    std::vector<std::string> availableCommands() const; // list possible commands

    // ===== State Handlers =====
    void onLoadMap();
    void onValidateMap();
    void onAddPlayer();
    void onAssignCountries();
    void onIssueOrder();
    void onEndIssueOrders();
    void onEndExecOrders();
    void onWin();
    void onPlayAgain();
    void onEnd();

    // Part 2 
    void startupPhase();

    // Part 3
    void reinforcementPhase();
    void issueOrdersPhase();
    void executeOrdersPhase();
    void mainGameLoop();

    // ===== Helper =====
    void distributeRoundRobin();
};


#endif // GAMEENGINE_H
