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
#include "PlayerStrategies.h"

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
    std::string mapFilename_;  //I added this to fix undefined file problem while fixing A2 for A3
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

    // Helpers for A2
    int  computeReinforcementsFor(Player* p) const;
    int  continentBonusFor(Player* p) const; // returns 0 if your Continent has no bonus field
    bool anyOrdersRemain() const;
    void removeDefeatedPlayers();            // drop players with zero territories
    bool checkWinAndMaybeEnterWinState();    // returns true if someone owns all territories

    // Track reinforcement pools per player (heap-allocated to respect pointer rule)
    std::unordered_map<Player*, int*>* reinforcementPool_ = nullptr;

    // ===== A3: Tournament logging helper =====
    std::string* lastLogMessage_ = nullptr;   // buffer used by stringToLog()

public:
    // ===== Constructor/Destructor =====
    GameEngine();
    ~GameEngine();        

    // ===== Accessors =====
    std::string stateName() const;       // returns current state's name
    GameState getState() const { return state_; }  
    GameState state() const { return state_; } 

    void setState(GameState newState) { // Added setter defined here
        state_ = newState;    

        if (lastLogMessage_) { //added for A3
            *lastLogMessage_ = std::string("STATE_CHANGE | ") + stateName();
        }    

        notify();                       //added notify() for part 5
    }


    // ===== Core Methods =====
    bool processCommand(const std::string& in);        // process a command
    std::vector<std::string> availableCommands() const; // list possible commands

    // ===== State Handlers =====
    bool onLoadMap(const std::string& filename);
    bool onValidateMap();
    void onAddPlayer(const std::string& name);
    void onAssignCountries();
    void onIssueOrder();
    void onEndIssueOrders();
    void onEndExecOrders();
    void onWin();
    void onPlayAgain();
    void onEnd();

    // ===== Helper =====
    void distributeRoundRobin();

    //----------A2----------//
    void reinforcementPhase();
    void issueOrdersPhase();
    void executeOrdersPhase();

    void startupPhase();

     //===== A3: Tournament mode =====
    void runTournament(const std::vector<std::string>& mapFiles,
                       const std::vector<std::string>& playerStrategies,
                       int gamesPerMap,
                       int maxTurns);
                       
    std::string stringToLog() const override; // part5
};

#endif // GAMEENGINE_H
