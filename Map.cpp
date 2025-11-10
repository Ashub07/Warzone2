#include "Map.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <fstream>
#include <sstream>
#include <map>
#include <unordered_map>

// ============================================================================
// Territory Implementation
// ============================================================================
// I keep all user-defined fields as pointers (per assignment rules).
// I also implement the Rule of 3 (copy ctor, assignment, dtor).

// Default constructor: safe defaults so a "blank" territory won't crash
Territory::Territory() {
    name = new std::string("Unknown");
    continent = new std::string("Unknown");
    owner = new std::string("Neutral");
    armies = new int(0);
    id = new int(-1);
    adjacentTerritories = new std::vector<Territory*>();
}

// Copy constructor: deep copy all pointer fields
Territory::Territory(const Territory& other) {
    name = new std::string(*other.name);
    continent = new std::string(*other.continent);
    owner = new std::string(*other.owner);
    armies = new int(*other.armies);
    id = new int(*other.id);
    // Shallow with respect to neighbor objects, but we copy the container
    adjacentTerritories = new std::vector<Territory*>(*other.adjacentTerritories);
}

// Parameterized constructor: normal creation path
Territory::Territory(std::string name, std::string continent, std::string owner,
                     int armies, int id, std::vector<Territory*>* adjacent) {
    this->name = new std::string(name);
    this->continent = new std::string(continent);
    this->owner = new std::string(owner);
    this->armies = new int(armies);
    this->id = new int(id);
    // We copy the vector so external callers keep ownership of their container
    // BUT: the caller might pass nullptr; handle that safely.
    if (adjacent) {
        this->adjacentTerritories = new std::vector<Territory*>(*adjacent);
    } else {
        this->adjacentTerritories = new std::vector<Territory*>();
    }
}

// Assignment operator: clean old heap data, then deep-copy
Territory& Territory::operator=(const Territory& other) {
    if (this != &other) {
        delete name;
        delete continent;
        delete owner;
        delete armies;
        delete id;
        delete adjacentTerritories;

        name = new std::string(*other.name);
        continent = new std::string(*other.continent);
        owner = new std::string(*other.owner);
        armies = new int(*other.armies);
        id = new int(*other.id);
        adjacentTerritories = new std::vector<Territory*>(*other.adjacentTerritories);
    }
    return *this;
}

// Destructor: free everything I allocated
Territory::~Territory() {
    delete name;
    delete continent;
    delete owner;
    delete armies;
    delete id;
    delete adjacentTerritories; // note: we don't own neighbor territories
}

// --- Getters ---
std::string Territory::getName() const { return *name; }
std::string Territory::getContinent() const { return *continent; }
std::string Territory::getOwner() const { return *owner; }
int Territory::getArmies() const { return *armies; }
int Territory::getId() const { return *id; }
std::vector<Territory*>* Territory::getAdjacentTerritories() const { return adjacentTerritories; }

// --- Setters ---
void Territory::setName(std::string name) { *this->name = name; }
void Territory::setContinent(std::string continent) { *this->continent = continent; }
void Territory::setOwner(std::string owner) { *this->owner = owner; }
void Territory::setArmies(int armies) { *this->armies = armies; }
void Territory::setId(int id) { *this->id = id; }
// --- Setters ---
void Territory::setAdjacentTerritories(std::vector<Territory*>* adj) {
    delete adjacentTerritories;
    // Guard nullptr: replace with empty vector if none provided
    if (adj) {
        adjacentTerritories = new std::vector<Territory*>(*adj);
    } else {
        adjacentTerritories = new std::vector<Territory*>();
    }
}

// Add a neighbor. I guard against duplicates (by ID equality).
void Territory::addAdjacentTerritory(Territory* territory) {
    for (auto t : *adjacentTerritories) {
        if (*t == *territory) return;
    }
    adjacentTerritories->push_back(territory);
}

// Remove a neighbor by matching ID
void Territory::removeAdjacentTerritory(Territory* territory) {
    auto it = std::remove_if(adjacentTerritories->begin(), adjacentTerritories->end(),
                             [territory](Territory* t) { return *t == *territory; });
    if (it != adjacentTerritories->end()) adjacentTerritories->erase(it, adjacentTerritories->end());
}

// Nice console output helper for debugging
void Territory::printTerritoryInfo() const {
    std::cout << "Territory ID: " << *id << "\n";
    std::cout << "Name: " << *name << "\n";
    std::cout << "Continent: " << *continent << "\n";
    std::cout << "Owner: " << *owner << "\n";
    std::cout << "Armies: " << *armies << "\n";
    std::cout << "Adjacent: ";
    for (auto t : *adjacentTerritories) std::cout << t->getName() << " ";
    std::cout << "\n";
}

// Stream insertion (minimal one-line summary)
std::ostream& operator<<(std::ostream& out, const Territory& t) {
    out << "Territory ID: " << *t.id
        << " Name: " << *t.name
        << " Continent: " << *t.continent
        << " Owner: " << *t.owner
        << " Armies: " << *t.armies;
    return out;
}

// Equality by ID (IDs are the primary identity)
bool Territory::operator==(const Territory& other) const {
    return *id == *other.id;
}

// Increment/decrement army count
Territory& Territory::operator++() { ++(*armies); return *this; }
Territory Territory::operator++(int) { Territory tmp = *this; ++(*armies); return tmp; }
Territory& Territory::operator--() { if (*armies > 0) --(*armies); return *this; }
Territory Territory::operator--(int) { Territory tmp = *this; if (*armies > 0) --(*armies); return tmp; }

// Adjacency checks (by object or by name)
bool Territory::isAdjacent(const Territory& other) const {
    for (auto t : *adjacentTerritories) if (*t == other) return true;
    return false;
}
bool Territory::isAdjacent(const std::string& name) const {
    for (auto t : *adjacentTerritories) if (t->getName() == name) return true;
    return false;
}


// ============================================================================
// Continent Implementation
// ============================================================================

// Default ctor: simple placeholders
Continent::Continent() {
    name = new std::string("Unknown");
    id = new int(-1);
    territories = new std::vector<Territory*>();
}

// Copy ctor: deep copy pointer fields (container is copied, elements are non-owned)
Continent::Continent(const Continent& other) {
    name = new std::string(*other.name);
    id = new int(*other.id);
    territories = new std::vector<Territory*>(*other.territories);
}

// Param ctor
Continent::Continent(std::string name, int id, std::vector<Territory*>* terrs) {
    this->name = new std::string(name);
    this->id = new int(id);
    territories = new std::vector<Territory*>(*terrs);
}

// Dtor
Continent::~Continent() {
    delete name;
    delete id;
    delete territories; // we do not own the Territory* elements
}

// Assignment operator
Continent& Continent::operator=(const Continent& other) {
    if (this != &other) {
        delete name; delete id; delete territories;
        name = new std::string(*other.name);
        id = new int(*other.id);
        territories = new std::vector<Territory*>(*other.territories);
    }
    return *this;
}

// --- Getters/Setters ---
std::string Continent::getName() const { return *name; }
int Continent::getId() const { return *id; }
std::vector<Territory*>* Continent::getTerritories() const { return territories; }

void Continent::setName(std::string n) { *name = n; }
void Continent::setId(int i) { *id = i; }
void Continent::setTerritories(std::vector<Territory*>* terrs) {
    delete territories;
    territories = new std::vector<Territory*>(*terrs);
}

// Add/remove territory pointers (no ownership)
void Continent::addTerritory(Territory* t) {
    for (auto terr : *territories) if (*terr == *t) return; // avoid duplicates
    territories->push_back(t);
}
void Continent::removeTerritory(Territory* t) {
    auto it = std::remove_if(territories->begin(), territories->end(),
                             [t](Territory* terr) { return *terr == *t; });
    if (it != territories->end()) territories->erase(it, territories->end());
}

// Pretty-print a quick list
void Continent::printContinentInfo() const {
    std::cout << "Continent " << *name << " (ID=" << *id << "): ";
    for (auto t : *territories) std::cout << t->getName() << " ";
    std::cout << "\n";
}

std::ostream& operator<<(std::ostream& out, const Continent& c) {
    out << "Continent " << *c.name << " (ID=" << *c.id << ")";
    return out;
}


// ============================================================================
// Map Implementation
// ============================================================================
// The Map *owns* the Territory* and Continent* it stores (we delete them in ~Map).
// That means external callers should not delete what they pass in (shared ownership
// would complicate Part 1; we keep it simple and explicit).

// Default ctor
Map::Map() {
    territories = new std::vector<Territory*>();
    continents = new std::vector<Continent*>();
}

// Copy ctor: deep copy owned objects
Map::Map(const Map& other) {
    territories = new std::vector<Territory*>();
    for (auto t : *other.territories) territories->push_back(new Territory(*t));
    continents = new std::vector<Continent*>();
    for (auto c : *other.continents) continents->push_back(new Continent(*c));
}

// Assignment operator: free current, deep copy from other
Map& Map::operator=(const Map& other) {
    if (this != &other) {
        for (auto t : *territories) delete t;
        delete territories;
        for (auto c : *continents) delete c;
        delete continents;

        territories = new std::vector<Territory*>();
        for (auto t : *other.territories) territories->push_back(new Territory(*t));
        continents = new std::vector<Continent*>();
        for (auto c : *other.continents) continents->push_back(new Continent(*c));
    }
    return *this;
}

// Param ctor: deep copy passed-in containers
Map::Map(std::vector<Territory*>* t, std::vector<Continent*>* c) {
    territories = new std::vector<Territory*>();
    for (auto terr : *t) territories->push_back(new Territory(*terr));
    continents = new std::vector<Continent*>();
    for (auto cont : *c) continents->push_back(new Continent(*cont));
}

// Dtor: we own and delete everything
Map::~Map() {
    for (auto t : *territories) delete t;
    delete territories;
    for (auto c : *continents) delete c;
    delete continents;
}

// --- Getters ---
std::vector<Territory*>* Map::getTerritories() const { return territories; }
std::vector<Continent*>* Map::getContinents() const { return continents; }

// --- Setters (replace entire collections with deep copies) ---
void Map::setTerritories(std::vector<Territory*>* t) {
    for (auto terr : *territories) delete terr;
    delete territories;
    territories = new std::vector<Territory*>();
    for (auto terr : *t) territories->push_back(new Territory(*terr));
}
void Map::setContinents(std::vector<Continent*>* c) {
    for (auto cont : *continents) delete cont;
    delete continents;
    continents = new std::vector<Continent*>();
    for (auto cont : *c) continents->push_back(new Continent(*cont));
}

// Add/remove territory pointers.
// NOTE: We store the *same pointer* the loader creates so cross-links (continent->territory)
// and validations by pointer/ID stay consistent.
void Map::addTerritory(Territory* t) {
    for (auto terr : *territories) {
        if (*terr == *t) return; // avoid duplicates by ID
    }
    territories->push_back(t);
}

void Map::removeTerritory(Territory* t) {
    auto it = std::remove_if(territories->begin(), territories->end(),
                             [t](Territory* terr) { return *terr == *t; });
    if (it != territories->end()) {
        delete *it; // we own the territory
        territories->erase(it, territories->end());
    }
}

// Add/remove continent pointers (same-pointer rule as above)
void Map::addContinent(Continent* c) {
    for (auto cont : *continents) {
        if (cont->getId() == c->getId()) return; // avoid duplicate same ID
    }
    continents->push_back(c);
}
void Map::removeContinent(Continent* c) {
    auto it = std::remove_if(continents->begin(), continents->end(),
                             [c](Continent* cont) { return cont->getId() == c->getId(); });
    if (it != continents->end()) {
        delete *it; // we own the continent
        continents->erase(it, continents->end());
    }
}

// ============================================================================
// Validation (Assignment Part 1)
//  1) Entire map is a connected graph.
//  2) Each continent is a connected subgraph.
//  3) Every territory belongs to exactly one continent.
// ============================================================================
bool Map::validate() const {
    if (territories->empty() || continents->empty()) {
        std::cout << " Validation failed: map has no territories or continents.\n";
        return false;
    }

    // --- Rule 3: Each territory must belong to exactly one continent
    for (auto terr : *territories) {
        int count = 0;
        for (auto cont : *continents) {
            for (auto t : *cont->getTerritories()) {
                if (*t == *terr) count++;
            }
        }
        if (count != 1) {
            std::cout << " Validation failed: territory " << terr->getName()
                      << " (ID=" << terr->getId()
                      << ") belongs to " << count << " continents.\n";
            return false;
        }
    }

    // Guard: no empty continents (helps catch typos in map files)
    for (auto cont : *continents) {
        if (cont->getTerritories()->empty()) {
            std::cout << " Validation failed: continent " << cont->getName()
                      << " has no territories.\n";
            return false;
        }
    }

    // --- Rule 1: Whole-map connectivity via DFS (by ID indexing)
    std::map<int, int> idToIndex;
    for (int i = 0; i < (int)territories->size(); i++) {
        idToIndex[(*territories)[i]->getId()] = i;
    }

    std::vector<bool> visited(territories->size(), false);
    std::function<void(Territory*)> dfs = [&](Territory* t) {
        int idx = idToIndex[t->getId()];
        if (visited[idx]) return;
        visited[idx] = true;
        for (auto neighbor : *(t->getAdjacentTerritories())) {
            dfs(neighbor);
        }
    };

    dfs(territories->front());
    for (int i = 0; i < (int)visited.size(); i++) {
        if (!visited[i]) {
            std::cout << " Validation failed: territory "
                      << (*territories)[i]->getName()
                      << " (ID=" << (*territories)[i]->getId()
                      << ") is not connected to the map.\n";
            return false;
        }
    }

    // --- Rule 2: Per-continent connectivity (subgraph induced by continent)
    for (auto cont : *continents) {
        auto terrs = cont->getTerritories();
        if (terrs->empty()) continue; // already guarded, but cheap

        std::map<int, int> localMap;
        for (int i = 0; i < (int)terrs->size(); i++) {
            localMap[terrs->at(i)->getId()] = i;
        }

        std::vector<bool> visitedCont(terrs->size(), false);
        std::function<void(Territory*)> dfsCont = [&](Territory* t) {
            int idx = localMap[t->getId()];
            if (visitedCont[idx]) return;
            visitedCont[idx] = true;
            for (auto neighbor : *(t->getAdjacentTerritories())) {
                if (localMap.count(neighbor->getId())) dfsCont(neighbor);
            }
        };

        dfsCont(terrs->front());
        for (int i = 0; i < (int)visitedCont.size(); i++) {
            if (!visitedCont[i]) {
                std::cout << " Validation failed: continent " << cont->getName()
                          << " is not fully connected. Territory "
                          << terrs->at(i)->getName()
                          << " (ID=" << terrs->at(i)->getId()
                          << ") is isolated.\n";
                return false;
            }
        }
    }

    std::cout << " Map validation passed.\n";
    return true;
}

// Quick dump of the map contents for debugging
void Map::printMapInfo() const {
    std::cout << "=== Map Information ===\n";
    std::cout << "Continents:\n";
    for (auto c : *continents) {
        c->printContinentInfo();
    }
    std::cout << "\nTerritories:\n";
    for (auto t : *territories) {
        t->printTerritoryInfo();
        std::cout << "---\n";
    }
}

std::ostream& operator<<(std::ostream& out, const Map& m) {
    m.printMapInfo();
    return out;
}

// ============================================================================
// MapLoader Implementation
// ============================================================================
// MapLoader owns a Map*. I add copy ctor + assignment to avoid double-deletes
// and to satisfy the spirit of the assignment’s copy-control requirements.

MapLoader::MapLoader() {
    map = new Map();
}

// Copy constructor: deep-copy the map (so two loaders don’t share one Map*)
MapLoader::MapLoader(const MapLoader& other) {
    map = other.map ? new Map(*other.map) : new Map();
}

// Assignment operator: deep copy, clean previous
MapLoader& MapLoader::operator=(const MapLoader& other) {
    if (this != &other) {
        delete map;
        map = other.map ? new Map(*other.map) : new Map();
    }
    return *this;
}

MapLoader::~MapLoader() {
    delete map;
}

Map* MapLoader::getMap() const {
    return map;
}

// --------------------------------------------------------------------------
// loadMap: parse the file into [continents], [territories], [borders]
// Territory lines expected format (based on your working tests):
//   <id> <name> <continentId> <owner> <armies>
//
// Example:
// [continents]
// NorthAmerica 1
// Europe 2
//
// [territories]
// 1 Alaska 1 Neutral 5
// 2 Canada 1 Neutral 3
// 3 UK 2 Neutral 4
//
// [borders]
// 1 2
// 2 1 3
// 3 2
// --------------------------------------------------------------------------
bool MapLoader::loadMap(const std::string& filename) {
    // reset map each load to avoid stale state
    delete map;
    map = new Map();

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Failed to open file: " << filename << "\n";
        return false;
    }

    std::string line;
    enum Section { NONE, CONTINENTS, TERRITORIES, BORDERS };
    Section section = NONE;

    // Lookup tables so we can wire borders after reading all territories
    std::unordered_map<int, Continent*> continentLookup; // by continent ID
    std::unordered_map<int, Territory*> territoryLookup; // by territory ID
    std::vector<std::pair<int, std::vector<int>>> borders; // raw neighbor data

    while (std::getline(file, line)) {
        // Ignore empty lines and comments
        if (line.empty() || line[0] == ';') continue;

        // Section headers
        if (line == "[continents]") { section = CONTINENTS; continue; }
        if (line == "[territories]") { section = TERRITORIES; continue; }
        if (line == "[borders]") { section = BORDERS; continue; }

        std::istringstream iss(line);

        // -------------------- CONTINENTS --------------------
        if (section == CONTINENTS) {
            std::string name;
            int id;
            if (!(iss >> name >> id)) {
                std::cout << "Failed to parse continent: " << line << "\n";
                return false;
            }
            // Create and register the continent (map owns it)
            Continent* c = new Continent(name, id, new std::vector<Territory*>());
            map->addContinent(c);
            continentLookup[id] = c;
            std::cout << "Added continent: " << name << " (ID: " << id << ")\n";
        }
        // -------------------- TERRITORIES --------------------
        else if (section == TERRITORIES) {
            int id, contId, armies;
            std::string name, owner;
            if (!(iss >> id >> name >> contId >> owner >> armies)) {
                std::cout << "Failed to parse territory: " << line << "\n";
                return false;
            }
            // Create the territory (map owns it)
            Territory* t = new Territory(name, "TEMP", owner, armies, id, new std::vector<Territory*>());
            map->addTerritory(t);
            territoryLookup[id] = t;

            // Attach territory to its continent by ID
            if (continentLookup.count(contId)) {
                t->setContinent(continentLookup[contId]->getName());
                continentLookup[contId]->addTerritory(t);
                std::cout << "Added territory: " << name << " to continent ID: " << contId << "\n";
            } else {
                std::cout << "Invalid continent ID: " << contId << " for territory: " << name << "\n";
                return false;
            }
        }
        // -------------------- BORDERS --------------------
        else if (section == BORDERS) {
            int id;
            if (!(iss >> id)) {
                std::cout << "Failed to parse border: " << line << "\n";
                return false;
            }
            std::vector<int> neighbors;
            int neighborId;
            while (iss >> neighborId) {
                neighbors.push_back(neighborId);
            }
            borders.push_back({id, neighbors});
            std::cout << "Border for territory " << id << " has " << neighbors.size() << " neighbors\n";
        }
    }
    file.close();

    // After we've created all territories, wire up adjacency using ID lookups
    for (auto& border : borders) {
        int territoryId = border.first;
        if (!territoryLookup.count(territoryId)) {
            std::cout << "Invalid territory ID in borders: " << territoryId << "\n";
            return false;
        }

        Territory* territory = territoryLookup[territoryId];
        for (int neighborId : border.second) {
            if (!territoryLookup.count(neighborId)) {
                std::cout << "Invalid neighbor ID: " << neighborId
                          << " for territory: " << territoryId << "\n";
                return false;
            }
            territory->addAdjacentTerritory(territoryLookup[neighborId]);
        }
    }

    std::cout << "Map loading completed. Validating...\n";
    return map->validate();
}

