#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <string>
#include <vector>

// ============================================================================
// Territory Class
// ============================================================================
// Each Territory has:
//  - name
//  - continent (string, not pointer to Continent to keep things simple)
//  - owner (just a string for now, could be Player later in assignment)
//  - armies
//  - unique ID
//  - adjacency list (vector of Territory*)
// I use raw pointers for all fields to practice Rule of 3.

class Territory {
private:
    std::string* name;
    std::string* continent;
    std::string* owner;
    int* armies;
    int* id;
    std::vector<Territory*>* adjacentTerritories;

public:
    // Constructors / destructor
    Territory();
    Territory(const Territory& other);
    Territory(std::string name, std::string continent, std::string owner,
              int armies, int id, std::vector<Territory*>* adjacent);
    Territory& operator=(const Territory& other);
    ~Territory();

    // Getters
    std::string getName() const;
    std::string getContinent() const;
    std::string getOwner() const;
    int getArmies() const;
    int getId() const;
    std::vector<Territory*>* getAdjacentTerritories() const;

    // Setters
    void setName(std::string name);
    void setContinent(std::string continent);
    void setOwner(std::string owner);
    void setArmies(int armies);
    void setId(int id);
    void setAdjacentTerritories(std::vector<Territory*>* adj);

    // Adjacent handling
    void addAdjacentTerritory(Territory* territory);
    void removeAdjacentTerritory(Territory* territory);

    // Printing
    void printTerritoryInfo() const;

    // Operators
    friend std::ostream& operator<<(std::ostream& out, const Territory& t);
    bool operator==(const Territory& other) const;
    Territory& operator++();     // prefix ++ (armies++)
    Territory operator++(int);   // postfix ++
    Territory& operator--();     // prefix -- (armies--)
    Territory operator--(int);   // postfix --
    bool isAdjacent(const Territory& other) const;
    bool isAdjacent(const std::string& name) const;
};

// ============================================================================
// Continent Class
// ============================================================================
// Each continent has:
//  - name
//  - id
//  - list of territories that belong to it
// MapLoader will link these properly.

class Continent {
private:
    std::string* name;
    int* id;
    std::vector<Territory*>* territories;

public:
    Continent();
    Continent(const Continent& other);
    Continent(std::string name, int id, std::vector<Territory*>* terrs);
    Continent& operator=(const Continent& other);
    ~Continent();

    // Getters
    std::string getName() const;
    int getId() const;
    std::vector<Territory*>* getTerritories() const;

    // Setters
    void setName(std::string n);
    void setId(int i);
    void setTerritories(std::vector<Territory*>* terrs);

    // Modifiers
    void addTerritory(Territory* t);
    void removeTerritory(Territory* t);

    // Printing
    void printContinentInfo() const;
    friend std::ostream& operator<<(std::ostream& out, const Continent& c);
};

// ============================================================================
// Map Class
// ============================================================================
// The Map owns all Continent* and Territory* objects. It is responsible for
// deleting them when destroyed (so we avoid leaks).
// It must also validate itself according to the assignment rules.

class Map {
private:
    std::vector<Territory*>* territories;
    std::vector<Continent*>* continents;

public:
    Map();
    Map(const Map& other);
    Map(std::vector<Territory*>* t, std::vector<Continent*>* c);
    Map& operator=(const Map& other);
    ~Map();

    // Getters/setters
    std::vector<Territory*>* getTerritories() const;
    std::vector<Continent*>* getContinents() const;
    void setTerritories(std::vector<Territory*>* t);
    void setContinents(std::vector<Continent*>* c);

    // Modifiers
    void addTerritory(Territory* t);
    void removeTerritory(Territory* t);
    void addContinent(Continent* c);
    void removeContinent(Continent* c);

    // Validation
    bool validate() const;

    // Debug printing
    void printMapInfo() const;
    friend std::ostream& operator<<(std::ostream& out, const Map& m);
};

// ============================================================================
// MapLoader Class
// ============================================================================
// Reads a file into a Map object.
// Owns its Map* and provides deep copy control.

class MapLoader {
private:
    Map* map;

public:
    MapLoader();
    MapLoader(const MapLoader& other);
    MapLoader& operator=(const MapLoader& other);
    ~MapLoader();

    Map* getMap() const;
    bool loadMap(const std::string& filename);
};

#endif // MAP_H