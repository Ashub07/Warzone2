#include "CommandProcessing.h"
using namespace std;

/*
 ---------------------------------------------------------
                      Command Class
 ---------------------------------------------------------
*/

Command::Command(string cmd) {
    command = new string(cmd);
    effect = new string("");
}

Command::Command(const Command& other) {
    command = new string(*other.command);
    effect = new string(*other.effect);
}

Command& Command::operator=(const Command& other) {
    if (this != &other) {
        delete command;
        delete effect;
        command = new string(*other.command);
        effect = new string(*other.effect);
    }
    return *this;
}

Command::~Command() {
    delete command;
    delete effect;
}

string Command::getCommand() const { return *command; }
string Command::getEffect() const { return *effect; }

void Command::saveEffect(const string& eff) {
    *effect = eff;
    notify(); //call notify to record effect for part 5
}

ostream& operator<<(ostream& out, const Command& c) {
    out << "Command: " << *c.command << " | Effect: " << *c.effect;
    return out;
}

std::string Command::stringToLog() const {       // Define for part 5
    return "EFFECT | " + *command + " -> " + *effect;
}


/*
 ---------------------------------------------------------
                 CommandProcessor Class
 ---------------------------------------------------------
*/

CommandProcessor::CommandProcessor() {
    commands = new vector<Command*>();
    inputSource = new string("console");
}

CommandProcessor::CommandProcessor(const CommandProcessor& other) {
    inputSource = new string(*other.inputSource);
    commands = new vector<Command*>();
    for (auto c : *other.commands)
        commands->push_back(new Command(*c));
}

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

CommandProcessor::~CommandProcessor() {
    delete inputSource;
    for (auto c : *commands) delete c;
    delete commands;
}

string CommandProcessor::readCommand() {
    string cmd;
    cout << "Enter command: ";
    getline(cin, cmd);
    return cmd;
}

void CommandProcessor::saveCommand(const string& cmd) {
    Command* c = new Command(cmd);
    commands->push_back(c);
    lastMessage = "COMMAND | " + cmd;
    notify(); //call notify to save command for part 5
}

string CommandProcessor::getCommand() {
    string cmd = readCommand();
    saveCommand(cmd);
    return cmd;
}


/*
 ---------------------------------------------------------
         Helper Function for GameState Conversion
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
          FIXED Command Validation Function
 ---------------------------------------------------------
 - Only checks if command is valid for current state
 - Does NOT change the state (GameEngine does that)
 ---------------------------------------------------------
*/
bool CommandProcessor::validate(const string& cmd, GameEngine* game) {
    string state = gameStateToString(game->getState());
    bool valid = false;
    // ---- split cmd into keyword + arguments ----
    string trimmed = cmd;

    // simple left trim (optional, but nice)
    auto firstNon = trimmed.find_first_not_of(" \t");
    if (firstNon != string::npos)
        trimmed = trimmed.substr(firstNon);
    else
        trimmed.clear();

    string keyword;
    string args;

    auto spacePos = trimmed.find(' ');
    if (spacePos == string::npos) {
        keyword = trimmed;           // e.g. "loadmap"
        args = "";                   // no args
    } else {
        keyword = trimmed.substr(0, spacePos);      // "loadmap"
        args    = trimmed.substr(spacePos + 1);     // "mymap.map" (maybe with spaces)
    }

    bool hasArgs = (args.find_first_not_of(" \t") != string::npos);


    // Only validate - don't change state
    if (state == "start" && keyword == "loadmap" && hasArgs) {
        valid = true;
    }
    else if (state == "maploaded" && cmd == "validatemap") {
        valid = true;
    }
    else if (state == "maploaded" && cmd.find("loadmap") == 0) {
        valid = true; // allows reloading a map
    }
    else if (state == "mapvalidated" && cmd.find("addplayer") == 0) {
        valid = true;
    }
    else if (state == "playersadded" && cmd.find("addplayer") == 0) {
        valid = true;
    }
    else if (state == "playersadded" && cmd == "gamestart") {
        valid = true;
    }
    else if (state == "win" && cmd == "replay") {
        valid = true;
    }
    else if (state == "win" && cmd == "quit") {
        valid = true;
        cout << "\nGame ended.\n";
    }

    // Record the outcome WITHOUT changing state
    if (valid) {
        // Let GameEngine process the command
        bool executed = game->processCommand(cmd);
        if (executed) {
            commands->back()->saveEffect("VALID | Effect: Command executed, state is now " +
                                         gameStateToString(game->getState()));
        } else {
            commands->back()->saveEffect("VALID | Effect: Command valid but execution failed, state remains " +
                                         gameStateToString(game->getState()));
        }
    }
    else {
        commands->back()->saveEffect("INVALID | Effect: Command not allowed in state " + state);
    }

    return valid;
}

vector<Command*>* CommandProcessor::getCommands() const {
    return commands;
}

ostream& operator<<(ostream& out, const CommandProcessor& cp) {
    out << "CommandProcessor [Source=" << *cp.inputSource
        << ", Commands stored=" << cp.commands->size() << "]";
    return out;
}

std::string CommandProcessor::stringToLog() const {       // Define for part 5
    return lastMessage;
}


/*
 ---------------------------------------------------------
          FileCommandProcessorAdapter Class
 ---------------------------------------------------------
*/

FileCommandProcessorAdapter::FileCommandProcessorAdapter(string filename) {
    file = new ifstream(filename);
    inputSource = new string("file");

    if (!file->is_open())
        cout << "Error: Could not open file '" << filename << "'." << endl;
}

string FileCommandProcessorAdapter::readCommand() {
    string cmd;
    if (file && getline(*file, cmd))
        return cmd;
    else
        return "EOF";
}

FileCommandProcessorAdapter::~FileCommandProcessorAdapter() {
    if (file) {
        if (file->is_open())
            file->close();
        delete file;
    }
}