#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <map>
#include <string>
#include <vector>
#include <unordered_map>
#include "Map.h"
#include "Player.h"
#include "Orders.h"
#include "Cards.h"
#include "LoggingObserver.h"

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
class GameEngine : public Subject, public ILoggable {
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

    // Helpers for Part 3
    int  computeReinforcementsFor(Player* p) const;
    int  continentBonusFor(Player* p) const; // returns 0 if your Continent has no bonus field
    bool anyOrdersRemain() const;
    void removeDefeatedPlayers();            // drop players with zero territories
    bool checkWinAndMaybeEnterWinState();    // returns true if someone owns all territories

    // Track reinforcement pools per player (heap-allocated to respect pointer rule)
    std::unordered_map<Player*, int*>* reinforcementPool_ = nullptr;

public:
    // ===== Constructor/Destructor =====
    GameEngine();
    ~GameEngine();        

    // ===== Accessors =====
    std::string stateName() const;       // returns current state's name
    GameState getState() const { return state_; }  
    GameState state() const { return state_; } 
    void setState(GameState newState) { // ✅ Added setter defined here
        state_ = newState;                     
        notify();                       //added notify() for part 5
    }


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

    // ===== Helper =====
    void distributeRoundRobin();

    void reinforcementPhase();
    void issueOrdersPhase();
    void executeOrdersPhase();

    //----------A2----------//
    void startupPhase();

    std::string stringToLog() const override; // part5
};

#endif // GAMEENGINE_H
