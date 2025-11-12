#include "LoggingObserver.h"

// -------- Subject --------
// Rule of Three implementation for Subject: ctor, copy ctor, assignment, dtor.

Subject::Subject() : observers(new std::vector<Observer*>) {}

Subject::Subject(const Subject& other)
    : observers(new std::vector<Observer*>(*other.observers)) {}

Subject& Subject::operator=(const Subject& other) {
    if (this != &other)
        *observers = *other.observers;
    return *this;
}

Subject::~Subject() {
    delete observers;   // We only delete the container, not the observers themselves.
}

void Subject::attach(Observer* obs) {
    observers->push_back(obs);
}

void Subject::detach(Observer* obs) {
    observers->erase(std::remove(observers->begin(), observers->end(), obs), observers->end());
}

void Subject::notify() {
    // Only notify if this Subject is also ILoggable.
    ILoggable* src = dynamic_cast<ILoggable*>(this);
    if (!src) return;

    // Call update on each attached observer.
    for (auto o : *observers)
        o->update(src);
}

// -------- LogObserver --------
// Rule of Three implementation for LogObserver: ctor, copy ctor, assignment, dtor.

LogObserver::LogObserver() : logStream(nullptr) {}

LogObserver::LogObserver(const LogObserver&) : logStream(nullptr) {}

LogObserver& LogObserver::operator=(const LogObserver&) {
    return *this;  
}

LogObserver::~LogObserver() {
    if (logStream) {
        logStream->close();
        delete logStream;
    }
}
//file manipulation
void LogObserver::openFileIfClosed() {
    if (!logStream)
        logStream = new std::ofstream("gamelog.txt", std::ios::app);
    else if (!logStream->is_open())
        logStream->open("gamelog.txt", std::ios::app);
}

void LogObserver::update(ILoggable* src) {
    openFileIfClosed();
    *logStream << src->stringToLog() << "\n";
    logStream->flush();
}


