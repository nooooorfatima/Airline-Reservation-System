#include "Airline.h"
#include <iostream>

int main() {
    try {
        Airline airline;
        airline.load("data/airline_data.txt");
        airline.seedSampleData();
        airline.menu();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
