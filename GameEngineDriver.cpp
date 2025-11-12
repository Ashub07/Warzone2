#include "GameEngine.h"

#include <iostream>
#include <string>

/**
 * Repeatedly displays the current state and the list of available commands,
 * reads a command and applies it. Invalid commands are rejected without changing the state.
 */
void testGameStates() {
    GameEngine engine;

    std::cout << "=== COMP345 - Game Engine ===\n";
    std::cout << "Type commands shown for each state. Type Ctrl+D (Linux/macOS) or Ctrl+Z then Enter (Windows) to exit.\n\n";

    while (engine.getState() != GameState::End) {
        // Show state and available commands
        std::cout << "[state] " << engine.stateName() << "\n";
        const auto cmds = engine.availableCommands();

        if (cmds.empty()) {
            std::cout << "(No available commands from this state.)\n";
        } else {
            std::cout << "Available commands: ";
            for (size_t i = 0; i < cmds.size(); ++i) {
                std::cout << cmds[i] << (i + 1 < cmds.size() ? ", " : "");
            }
            std::cout << "\n";
        }

        // Read one command line
        std::cout << "> ";
        std::string input;
        if (!std::getline(std::cin, input)) {
            std::cout << "\nInput closed. Exiting.\n";
            break;
        }

        // Apply command (engine prints transition or invalid message).
        engine.processCommand(input);
        std::cout << "\n";
    }

    std::cout << "Program terminated.\n";
}

void TestStartupPhase(){
    std::cout << "Startup Phase Begun ";
    GameEngine engine; 
    engine.startupPhase();
    std::cout << "Startup Phase Finished ";
}

