#include "CommandProcessing.h"
using namespace std;

/*
 ---------------------------------------------------------
                      Command Class
 ---------------------------------------------------------
*/

// Regular constructor
Command::Command(string cmd) {
    command = new string(cmd);
    effect = new string("");
}

// Copy constructor
Command::Command(const Command& other) {
    command = new string(*other.command);
    effect = new string(*other.effect);
}

// Assignment operator
Command& Command::operator=(const Command& other) {
    if (this != &other) {
        delete command;
        delete effect;
        command = new string(*other.command);
        effect = new string(*other.effect);
    }
    return *this;
}

// Destructor
Command::~Command() {
    delete command;
    delete effect;
}

// Accessors
string Command::getCommand() const { return *command; }
string Command::getEffect() const { return *effect; }

// Save the command effect
void Command::saveEffect(const string& eff) {
    *effect = eff;
}

// Stream output
ostream& operator<<(ostream& out, const Command& c) {
    out << "Command: " << *c.command << " | Effect: " << *c.effect;
    return out;
}


/*
 ---------------------------------------------------------
                 CommandProcessor Class
 ---------------------------------------------------------
*/

// Default constructor
CommandProcessor::CommandProcessor() {
    commands = new vector<Command*>();
    inputSource = new string("console");
}

// Copy constructor
CommandProcessor::CommandProcessor(const CommandProcessor& other) {
    inputSource = new string(*other.inputSource);
    commands = new vector<Command*>();
    for (auto c : *other.commands)
        commands->push_back(new Command(*c));
}

// Assignment operator
CommandProcessor& CommandProcessor::operator=(const CommandProcessor& other) {
    if (this != &other) {
        delete inputSource;
        for (auto c : *commands) delete c;
        delete commands;

        inputSource = new string(*other.inputSource);
        commands = new vector<Command*>();
        for (auto c : *other.commands)
            commands->push_back(new Command(*c));
    }
    return *this;
}

// Destructor
CommandProcessor::~CommandProcessor() {
    delete inputSource;
    for (auto c : *commands) delete c;
    delete commands;
}

// Reads command from console (default)
string CommandProcessor::readCommand() {
    string cmd;
    cout << "Enter command: ";
    getline(cin, cmd);
    return cmd;
}

// Saves command to the list
void CommandProcessor::saveCommand(const string& cmd) {
    Command* c = new Command(cmd);
    commands->push_back(c);
}

// Returns command from input source and saves it
string CommandProcessor::getCommand() {
    string cmd = readCommand();
    saveCommand(cmd);
    return cmd;
}


/*
 ---------------------------------------------------------
         Helper Function for GameState Conversion
 ---------------------------------------------------------
 - Converts the GameEngine's enum GameState to string
   for easier comparison during validation
 ---------------------------------------------------------
*/
string gameStateToString(GameState state) {
    switch (state) {
        case GameState::Start: return "start";
        case GameState::MapLoaded: return "maploaded";
        case GameState::MapValidated: return "mapvalidated";
        case GameState::PlayersAdded: return "playersadded";
        case GameState::AssignReinforcement: return "assignreinforcement";
        case GameState::IssueOrders: return "issueorders";
        case GameState::ExecuteOrders: return "executeorders";
        case GameState::Win: return "win";
        default: return "unknown";
    }
}


/*
 ---------------------------------------------------------
          Command Validation Function
 ---------------------------------------------------------
 - Checks if the command is valid for the current state
 - Matches against the Assignment 2 specification
 ---------------------------------------------------------
*/
bool CommandProcessor::validate(const string& cmd, GameEngine* game) {
    string state = gameStateToString(game->getState());
    bool valid = false;

    // Validation + state transitions
    if (state == "start" && cmd.find("loadmap") == 0) {
        valid = true;
        game->setState(GameState::MapLoaded);
    }
    else if (state == "maploaded" && cmd == "validatemap") {
        valid = true;
        game->setState(GameState::MapValidated);
    }
    else if (state == "maploaded" && cmd.find("loadmap") == 0) {
        valid = true; // allows reloading a map
        game->setState(GameState::MapLoaded);
    }
    else if (state == "mapvalidated" && cmd.find("addplayer") == 0) {
        valid = true;
        game->setState(GameState::PlayersAdded);
    }
    else if (state == "playersadded" && cmd.find("addplayer") == 0) {
        valid = true; // remain in PlayersAdded
        game->setState(GameState::PlayersAdded);
    }
    else if (state == "playersadded" && cmd == "gamestart") {
        valid = true;
        game->setState(GameState::AssignReinforcement);
    }
    else if (state == "win" && cmd == "replay") {
        valid = true;
        game->setState(GameState::Start);
    }
    else if (state == "win" && cmd == "quit") {
        valid = true;
        cout << "\nGame ended.\n";
    }

    // Record the outcome
    if (valid)
        commands->back()->saveEffect("Command validated, state changed to " +
                                     gameStateToString(game->getState()));
    else
        commands->back()->saveEffect("Invalid command for current game state.");

    return valid;
}

// Return list of stored commands
vector<Command*>* CommandProcessor::getCommands() const {
    return commands;
}

// Stream output
ostream& operator<<(ostream& out, const CommandProcessor& cp) {
    out << "CommandProcessor [Source=" << *cp.inputSource
        << ", Commands stored=" << cp.commands->size() << "]";
    return out;
}


/*
 ---------------------------------------------------------
          FileCommandProcessorAdapter Class
 ---------------------------------------------------------
*/

// Constructor that opens a file
FileCommandProcessorAdapter::FileCommandProcessorAdapter(string filename) {
    file = new ifstream(filename);
    inputSource = new string("file");

    if (!file->is_open())
        cout << "Error: Could not open file '" << filename << "'." << endl;
}

// Reads next command from file
string FileCommandProcessorAdapter::readCommand() {
    string cmd;
    if (file && getline(*file, cmd))
        return cmd;
    else
        return "EOF"; // end of file
}

// Destructor closes the file
FileCommandProcessorAdapter::~FileCommandProcessorAdapter() {
    if (file) {
        if (file->is_open())
            file->close();
        delete file;
    }
}