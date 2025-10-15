//
// Created by Arshdeep Bhullar on 2025-10-14.
//

#ifndef A2_345_COMMANDPROCESSING_H
#define A2_345_COMMANDPROCESSING_H


#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "GameEngine.h"   // use existing GameEngine from A1 for state validation

using namespace std;

/*
 ---------------------------------------------------------
                    Command Class
 ---------------------------------------------------------
 - Represents a single command entered by the user
 - Stores the command string and its execution effect
 - Implements Rule of Three (copy, assign, destroy)
 ---------------------------------------------------------
*/
class Command {
private:
    string* command;   // the actual command text
    string* effect;    // description of the effect after execution

public:
    // Constructors / Rule of Three
    explicit Command(string cmd);           // regular constructor
    Command(const Command& other);          // copy constructor
    Command& operator=(const Command& other); // assignment operator
    ~Command();                             // destructor

    // Accessors
    string getCommand() const;              // return the command text
    string getEffect() const;               // return the saved effect

    // Mutator
    void saveEffect(const string& eff);     // store the result/effect of command

    // Output stream operator
    friend ostream& operator<<(ostream& out, const Command& c);
};


/*
 ---------------------------------------------------------
                 CommandProcessor Class
 ---------------------------------------------------------
 - Reads, validates, and stores commands
 - By default reads from console
 - Uses Adapter Pattern for file-based commands
 - Implements Rule of Three and maintains a list of Command objects
 ---------------------------------------------------------
*/
class CommandProcessor {
protected:
    vector<Command*>* commands;   // list of command objects
    string* inputSource;          // "console" or "file"

    virtual string readCommand(); // reads a command (can be overridden)
    void saveCommand(const string& cmd); // stores command in the list

public:
    // Constructors / Rule of Three
    CommandProcessor();
    CommandProcessor(const CommandProcessor& other);
    CommandProcessor& operator=(const CommandProcessor& other);
    virtual ~CommandProcessor();

    // Core behavior
    virtual string getCommand();                 // fetch next command and store it
    bool validate(const string& cmd, GameEngine* game); // check if command is valid for current game state

    // Utility
    vector<Command*>* getCommands() const;       // returns list of stored commands
    friend ostream& operator<<(ostream& out, const CommandProcessor& cp);
};


/*
 ---------------------------------------------------------
          FileCommandProcessorAdapter Class
 ---------------------------------------------------------
 - Adapter that allows CommandProcessor to read commands
   from a text file instead of console input
 - Overrides readCommand() to read line by line from file
 ---------------------------------------------------------
*/
class FileCommandProcessorAdapter : public CommandProcessor {
private:
    ifstream* file;                      // input file stream

protected:
    string readCommand() override;       // reads commands from file

public:
    explicit FileCommandProcessorAdapter(string filename); // constructor taking file name
    ~FileCommandProcessorAdapter() override;               // destructor
};


#endif //A2_345_COMMANDPROCESSING_H