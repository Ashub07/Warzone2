#include "CommandProcessing.h"
#include <iostream>
using namespace std;

/*
 ---------------------------------------------------------
             Command Processing Driver
 ---------------------------------------------------------
 - Tests both console and file-based command input
 - Demonstrates reading, validating, and storing effects
 ---------------------------------------------------------
*/

void testCommandProcessor() {
    cout << "==============================\n";
    cout << " TESTING COMMAND PROCESSOR\n";
    cout << "==============================\n";

    GameEngine game;              // uses your full A1 GameEngine
    CommandProcessor* cp = new CommandProcessor();

    cout << "\n--- Console Mode ---" << endl;
    cout << "Enter commands (type 'quit' to stop):\n";

    while (true) {
        string cmd = cp->getCommand();
        if (cmd == "quit")
            break;
        bool valid = cp->validate(cmd, &game);
        cout << " → " << (valid ? "VALID" : "INVALID")
             << " | Effect: " << cp->getCommands()->back()->getEffect() << endl;
    }

    cout << "\n--- Stored Commands ---" << endl;
    for (auto c : *cp->getCommands())
        cout << *c << endl;

    delete cp;
}

/*
 ---------------------------------------------------------
                 Main Entry Point
 ---------------------------------------------------------
*/

/*
int main(int argc, char** argv) {
    // default mode: console
    if (argc == 1 || string(argv[1]) == "-console") {
        testCommandProcessor();
    }
    // file mode
    else if (argc == 3 && string(argv[1]) == "-file") {
        string filename = argv[2];
        cout << "==============================\n";
        cout << " FILE COMMAND PROCESSOR MODE\n";
        cout << "==============================\n";
        FileCommandProcessorAdapter fcp(filename);
        GameEngine game;

        while (true) {
            string cmd = fcp.getCommand();
            if (cmd == "EOF" || cmd.empty())
                break;
            cout << "Command read: " << cmd << endl;
            bool valid = fcp.validate(cmd, &game);
            cout << " → " << (valid ? "VALID" : "INVALID") << endl;
        }
    }
    else {
        cout << "Usage:\n";
        cout << "  ./A2_345 -console\n";
        cout << "  ./A2_345 -file <filename>\n";
    }

    return 0;
}//
// Created by Arshdeep Bhullar on 2025-10-14.
//
*/