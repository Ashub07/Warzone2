#include "Map.h"
#include <iostream>

// Driver function required by assignment
void testLoadMaps() {
    MapLoader loader;

    // First test: valid map
    std::string file1 = "maps/valid_map1.map";
    std::cout << "=== Testing: " << file1 << " ===\n";
    if (loader.loadMap(file1)) {
        std::cout << " Map loaded successfully!\n";
        loader.getMap()->printMapInfo();
    } else {
        std::cerr << " Failed to load or validate map.\n";
    }
    std::cout << "\n";

    // Second test: another valid map
    std::string file2 = "maps/valid_map2.map";
    std::cout << "=== Testing: " << file2 << " ===\n";
    if (loader.loadMap(file2)) {
        std::cout << " Map loaded successfully!\n";
        loader.getMap()->printMapInfo();
    } else {
        std::cerr << " Failed to load or validate map.\n";
    }
    std::cout << "\n";

    // Third test: invalid map
    std::string file3 = "maps/invalid_map1.map";
    std::cout << "=== Testing: " << file3 << " ===\n";
    if (loader.loadMap(file3)) {
        std::cout << " Map loaded successfully!\n";
        loader.getMap()->printMapInfo();
    } else {
        std::cerr << " Failed to load or validate map.\n";
    }
    std::cout << "\n";

    // Fourth test: another invalid map
    std::string file4 = "maps/invalid_map2.map";
    std::cout << "=== Testing: " << file4 << " ===\n";
    if (loader.loadMap(file4)) {
        std::cout << " Map loaded successfully!\n";
        loader.getMap()->printMapInfo();
    } else {
        std::cerr << " Failed to load or validate map.\n";
    }
    std::cout << "\n";
}


