#include <iostream>

// Include all test function declarations
void testCommandProcessor();    // from CommandProcessingDriver.cpp
void testStartupPhase();        // from GameEngineDriver.cpp (Part 2)
void testMainGameLoop();        // from GameEngineDriver.cpp (Part 3)
//void testOrderExecution();      // from OrdersDriver.cpp (Part 4)
void testLoggingObserver();     // from LoggingObserverDriver.cpp (Part 5)
int main(int argc, char** argv) {
    std::cout << "=====================================\n";
    std::cout << " COMP345 PROJECT - MAIN DRIVER\n";
    std::cout << "=====================================\n\n";

    if (argc > 1 && std::string(argv[1]) == "-test") {
        std::cout << "Running all test drivers...\n\n";

        std::cout << "\n--- PART 1: Command Processor ---\n";
        testCommandProcessor();

        std::cout << "\n--- PART 2: Game Startup Phase ---\n";
        testStartupPhase();

        std::cout << "\n--- PART 3: Main Game Loop ---\n";
        testMainGameLoop();
        /*
        std::cout << "\n--- PART 4: Order Execution ---\n";
        testOrderExecution();
        */
        std::cout << "\n--- PART 5: Logging Observer ---\n";
        testLoggingObserver();

        std::cout << "\nAll test drivers completed.\n";
    } else {
        std::cout << "Running normal game execution...\n";
        std::cout << "(Use '-test' along with the file name to run all test drivers.)\n";
    }

    return 0;
}