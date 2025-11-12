#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

// Any class that can be logged should implement this.
class ILoggable {
public:
    virtual ~ILoggable() = default;
    // Returns the text that should be written to the log.
    virtual std::string stringToLog() const = 0;
};

// Generic observer in the Observer pattern.
class Observer {
public:
    virtual ~Observer() = default;
    // Called by Subject when there is something to observe.
    virtual void update(ILoggable* loggable) = 0;
};

// Subject keeps a list of observers and can notify them.
class Subject {
public:
    Subject();                                  // Rule of Three: default constructor
    Subject(const Subject& other);              // Rule of Three: copy constructor
    Subject& operator=(const Subject& other);   // Rule of Three: copy assignment
    virtual ~Subject();                         // Rule of Three: destructor

    void attach(Observer* obs);   // Register an observer
    void detach(Observer* obs);   // Unregister an observer

protected:
    void notify();                // Inform all observers

private:
    std::vector<Observer*>* observers;
};

// Concrete observer that writes to a log file.
class LogObserver : public Observer {
public:
    LogObserver();                               // Rule of Three: default constructor
    LogObserver(const LogObserver& other);       // Rule of Three: copy constructor
    LogObserver& operator=(const LogObserver& other); // Rule of Three: copy assignment
    ~LogObserver();                              // Rule of Three: destructor

    // Called when a Subject notifies. Writes to gamelog.txt.
    void update(ILoggable* loggable) override;

private:
    std::ofstream* logStream;

    // Makes sure the log file is open (in append mode).
    void openFileIfClosed();
};




