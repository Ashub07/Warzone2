#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

// -------- ILoggable --------
class ILoggable {
public:
    virtual ~ILoggable() = default;
    virtual std::string stringToLog() const = 0;
};

// -------- Observer --------
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(ILoggable* loggable) = 0;
};

// -------- Subject --------
class Subject {
public:
    Subject();
    Subject(const Subject& other);
    Subject& operator=(const Subject& other);
    virtual ~Subject();

    void attach(Observer* obs);
    void detach(Observer* obs);

protected:
    void notify(); // call when something should be logged

private:
    std::vector<Observer*>* observers;
};

// -------- LogObserver --------
class LogObserver : public Observer {
public:
    LogObserver();
    LogObserver(const LogObserver& other);
    LogObserver& operator=(const LogObserver& other);
    ~LogObserver();

    void update(ILoggable* loggable) override;

private:
    std::ofstream* logStream;
    void openFileIfClosed();
};


