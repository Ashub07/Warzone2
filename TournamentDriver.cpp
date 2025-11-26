#include <iostream>
#include "GameEngine.h"
#include "CommandProcessing.h"
#include "LoggingObserver.h"

int testTournament() {
    GameEngine game;
    CommandProcessor cp;

    // Attach logger to both, as in Part 5
    LogObserver logger;

    game.attach(&logger);
    cp.attach(&logger);

    std::cout << "Enter tournament command:\n";
    std::cout << "Example:\n";
    std::cout << "  tournament -M world.map europe.map -P Aggressive,Benevolent,Neutral,Cheater -G 3 -D 30\n\n";

    std::string command = cp.getCommand();   // reads and stores
    cp.validate(command, &game);             // validates and runs tournament if appropriate

    return 0;
}

main(){
    testTournament();
}
