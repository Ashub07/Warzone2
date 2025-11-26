#include "CommandProcessing.h"
#include <sstream>
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


// ===== A3: Tournament command parsing =====================================

struct TournamentConfig {
    std::vector<std::string> maps;
    std::vector<std::string> strategies;
    int games     = 0;
    int maxTurns  = 0;
};

// split "A,B,C" or "A, B, C" into ["A","B","C"]
static std::vector<std::string> splitCSVList(const std::string& token) {
    std::vector<std::string> out;
    std::string current;
    for (char c : token) {
        if (c == ',') {
            if (!current.empty()) {
                // trim spaces
                size_t b = current.find_first_not_of(" \t");
                size_t e = current.find_last_not_of(" \t");
                if (b != std::string::npos)
                    out.push_back(current.substr(b, e - b + 1));
            }
            current.clear();
        } else {
            current.push_back(c);
        }
    }
    if (!current.empty()) {
        size_t b = current.find_first_not_of(" \t");
        size_t e = current.find_last_not_of(" \t");
        if (b != std::string::npos)
            out.push_back(current.substr(b, e - b + 1));
    }
    return out;
}

static bool parseTournamentArgs(const std::string& args, TournamentConfig& cfg) {
    std::istringstream iss(args);
    std::string token;
    std::string currentFlag;

    while (iss >> token) {
        if (token == "-M" || token == "-P" || token == "-G" || token == "-D") {
            currentFlag = token;
            continue;
        }

        if (currentFlag == "-M") {
            // maps can be given either as separate tokens or comma separated
            std::vector<std::string> pieces = splitCSVList(token);
            if (pieces.empty()) pieces.push_back(token);
            for (const auto& s : pieces) {
                if (!s.empty()) cfg.maps.push_back(s);
            }
        }
        else if (currentFlag == "-P") {
            std::vector<std::string> pieces = splitCSVList(token);
            if (pieces.empty()) pieces.push_back(token);
            for (const auto& s : pieces) {
                if (!s.empty()) cfg.strategies.push_back(s);
            }
        }
        else if (currentFlag == "-G") {
            cfg.games = std::stoi(token);
            currentFlag.clear();
        }
        else if (currentFlag == "-D") {
            cfg.maxTurns = std::stoi(token);
            currentFlag.clear();
        }
    }

    // basic presence
    if (cfg.maps.empty() || cfg.strategies.empty() || cfg.games <= 0 || cfg.maxTurns <= 0)
        return false;

    // Assignment 3 constraints
    if (cfg.maps.size() < 1 || cfg.maps.size() > 5)  return false;
    if (cfg.strategies.size() < 2 || cfg.strategies.size() > 4) return false;
    if (cfg.games < 1 || cfg.games > 5)             return false;
    if (cfg.maxTurns < 10 || cfg.maxTurns > 50)     return false;

    return true;
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

    // =====================================================================
    // A3: tournament command (only valid in start state)
    // =====================================================================
    if (state == "start" && keyword == "tournament") {
        TournamentConfig cfg;
        if (!parseTournamentArgs(args, cfg)) {
            std::cout << "[tournament] Invalid parameters. "
                      << "Usage: tournament -M <maps> -P <strategies> -G <1-5> -D <10-50>\n";
            if (!commands->empty()) {
                commands->back()->saveEffect("INVALID | Effect: bad tournament parameters");
            }
            return false;   // do NOT fall through to normal command handling
        }

        std::cout << "[tournament] Starting tournament...\n";
        if (!commands->empty()) {
            commands->back()->saveEffect("VALID | Effect: tournament executed");
        }

        // Run the tournament automatically – no further user interaction.
        game->runTournament(cfg.maps, cfg.strategies, cfg.games, cfg.maxTurns);

        return true;        // do NOT call game->processCommand on "tournament"
    }

    /*
     ---------------------------------------------------------
              Normal Command Validation (Part 1/2)
     ---------------------------------------------------------
     - Only checks if command is valid for current state
     - Does NOT change the state (GameEngine does that)
     ---------------------------------------------------------
    */

    // Only validate - don't change state
    if (state == "start" && keyword == "loadmap" && hasArgs) {
        valid = true;
    }
    else if (state == "maploaded" && keyword == "validatemap") {
        valid = true;
    }
    else if (state == "maploaded" && keyword == "loadmap" && hasArgs) {
        valid = true; // allows reloading a map (if you still want this)
    }
    else if (state == "mapvalidated" && keyword == "addplayer" && hasArgs) {
        valid = true;
    }
    else if (state == "playersadded" && keyword == "addplayer" && hasArgs) {
        valid = true;
    }
    else if (state == "playersadded" && keyword == "gamestart") {
        valid = true;
    }
    else if (state == "win" && keyword == "replay") {
        valid = true;
    }
    else if (state == "win" && keyword == "quit") {
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