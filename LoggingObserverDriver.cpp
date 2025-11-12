#include <iostream>
#include <string>
#include "LoggingObserver.h"
#include "CommandProcessing.h"
#include "GameEngine.h"
#include "Orders.h"
#include "Player.h"
#include "Cards.h"

using namespace std;

//The free function for the driver
void testLoggingObserver() {
    cout << "=== Part 5: LoggingObserver demo ===\n";

    LogObserver logger;

    //COMMAND + EFFECT + STATE
    CommandProcessor cp;
    GameEngine ge;               
    cp.attach(&logger);          // logs COMMAND via saveCommand()->notify()
    ge.attach(&logger);          // logs STATE when setState()/transition notifies

    // ORDER_ADDED + ORDER_EXECUTED
    Player alice("Alice", {}, new Deck(), new OrdersList());
     Territory* t = new Territory(); // dummy territory for simplicity 
    int* armies1 = new int(1);
    int* armies2 = new int(2);
    OrdersList* ol = alice.getOrder();
    ol->attach(&logger);          // logs ORDER_ADDED when we add
    Orders* d = new Deploy(&alice, t, armies1);
    Orders* d2 = new Deploy(&alice, t, armies2);
    ol->add(d);                   // -> ORDER_ADDED
    ol->add(d2);
    d->attach(&logger);           // so execute() will log ORDER_EXECUTED
    d->execute();                 // -> ORDER_EXECUTED

    //Interactive loop to produce COMMAND/EFFECT/STATE lines
    cout << "Enter commands (e.g., loadmap, validatemap, addplayer p, gamestart, replay, quit)\n";
    while (true) {
        string cmd = cp.getCommand();                 // -> COMMAND
        if (!cp.getCommands()->empty())
            cp.getCommands()->back()->attach(&logger);  // so Command::saveEffect() logs EFFECT
        cp.validate(cmd, &ge);                        // -> EFFECT + (inside) state change -> STATE
        if (cmd == "quit") break;
    }

    cout << "Done. Check 'gamelog.txt'.\n";
}



