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

/**
 * === A2 Part 2: Demo of the start-up phase ===
 */
void testStartupPhase() {
    std::cout << "=== testStartupPhase ===\n";
    GameEngine engine;
    engine.startupPhase();
    std::cout << "[driver] Startup complete. Current state: " << engine.stateName() << "\n";
    std::cout << "You can now enter your Part-3 loop if desired.\n";
}

/**
 * === A2 Part 3: Demo of the main game loop ===
 */
void testMainGameLoop() {
    GameEngine engine;

    engine.onLoadMap();
    engine.onValidateMap();
    engine.onAddPlayer();
    engine.onAssignCountries();

    std::cout << "\n=== TURN 1 ===\n";
    engine.reinforcementPhase();
    engine.issueOrdersPhase();
    engine.executeOrdersPhase();

    if (engine.getState() != GameState::Win) {
        std::cout << "\n=== TURN 2 ===\n";
        engine.reinforcementPhase();
        engine.issueOrdersPhase();
        engine.executeOrdersPhase();
    }

    std::cout << "\n(Main game loop demo complete.)\n";
}

/**
 * Entry point to run either test.
 */
int main() {
    // Uncomment ONE of the following lines at a time to test
    // testGameStates();
    // testStartupPhase();
    testMainGameLoop();

    return 0;
}
