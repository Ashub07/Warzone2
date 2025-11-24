#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <string>
#include <map>
#include <vector>
#include <unordered_map>
#include "Map.h"
#include "Player.h"
#include "Cards.h"
#include "Orders.h"

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

class GameEngine {
public:
    GameEngine();

    std::string stateName() const;
    GameState getState() const { return state; }
    void setState(GameState newState) { state = newState; }
    std::vector<std::string> availableCommands() const;
    bool processCommand(const std::string& command);

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

private:
    GameState state;
    std::string mapFilename_;  // Added to store map filename

    MapLoader loader;
    Map* map;
    std::vector<Player*> players;

    std::unordered_map<GameState, std::map<std::string, GameState>> transitions;

    void buildTransitions();
    void clearPlayers();
    void distributeRoundRobin();

    static std::string toLower(std::string s);
    static std::string trim(const std::string& s);
};

void startupPhase();

#endif