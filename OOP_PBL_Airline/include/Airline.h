#ifndef AIRLINE_H
#define AIRLINE_H

#include "Exceptions.h"
#include "Flight.h"
#include "Passenger.h"
#include "Ticket.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

template <typename T, typename Predicate>
std::vector<T*> genericSearch(const std::vector<std::unique_ptr<T>>& items, Predicate predicate) {
    std::vector<T*> results;
    for (const auto& item : items) {
        if (predicate(*item)) results.push_back(item.get());
    }
    return results;
}

class Airline {
    std::map<std::string, std::unique_ptr<Flight>> flights;
    std::map<std::string, std::unique_ptr<Passenger>> passengers;
    std::vector<Ticket> tickets;
    int nextTicketNumber;

    Flight& requireFlight(const std::string& flightNumber);
    Passenger& requirePassenger(const std::string& passengerId);
    const Flight& requireFlight(const std::string& flightNumber) const;
    const Passenger& requirePassenger(const std::string& passengerId) const;

public:
    Airline();

    void addFlight(std::unique_ptr<Flight> flight);
    void removeFlight(const std::string& flightNumber);
    void listFlights() const;
    void searchFlightsByNumber(const std::string& number) const;
    void searchFlightsByRoute(const std::string& origin, const std::string& destination) const;
    void searchFlightsByDate(const std::string& date) const;

    void registerPassenger(std::unique_ptr<Passenger> passenger);
    void removePassenger(const std::string& passengerId);
    void listPassengers() const;
    void bookingHistory(const std::string& passengerId) const;

    Ticket& bookTicket(const std::string& passengerId, const std::string& flightNumber);
    double cancelTicket(const std::string& ticketId);

    void todaysDepartures(const std::string& date) const;
    void occupancyReport() const;
    void topRevenueFlights() const;

    void save(const std::string& filename) const;
    void load(const std::string& filename);

    void seedSampleData();
    void menu();
};

#endif
