#include "Airline.h"
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace {
std::vector<std::string> split(const std::string& line) {
    std::vector<std::string> parts;
    std::stringstream ss(line);
    std::string item;
    while (std::getline(ss, item, '|')) parts.push_back(item);
    return parts;
}

std::string datePart(const std::string& dateTime) {
    return dateTime.substr(0, 10);
}
}

Airline::Airline() : nextTicketNumber(1001) {}

Flight& Airline::requireFlight(const std::string& number) {
    auto it = flights.find(number);
    if (it == flights.end()) throw NotFoundException("Flight not found: " + number);
    return *it->second;
}
const Flight& Airline::requireFlight(const std::string& number) const {
    auto it = flights.find(number);
    if (it == flights.end()) throw NotFoundException("Flight not found: " + number);
    return *it->second;
}
Passenger& Airline::requirePassenger(const std::string& id) {
    auto it = passengers.find(id);
    if (it == passengers.end()) throw NotFoundException("Passenger not found: " + id);
    return *it->second;
}
const Passenger& Airline::requirePassenger(const std::string& id) const {
    auto it = passengers.find(id);
    if (it == passengers.end()) throw NotFoundException("Passenger not found: " + id);
    return *it->second;
}

void Airline::addFlight(std::unique_ptr<Flight> flight) {
    if (!flight) throw std::invalid_argument("Null flight.");
    const auto number = flight->getFlightNumber();
    if (flights.count(number)) throw std::invalid_argument("Flight already exists.");
    flights[number] = std::move(flight);
}

void Airline::removeFlight(const std::string& number) {
    if (flights.find(number) == flights.end()) throw NotFoundException("Flight not found.");
    auto hasTicket = std::any_of(tickets.begin(), tickets.end(),
        [&](const Ticket& t) { return t.getFlightNumber() == number && t.isActive(); });
    if (hasTicket) throw std::runtime_error("Cannot remove a flight with active bookings.");
    flights.erase(number);
}

void Airline::listFlights() const {
    if (flights.empty()) { std::cout << "No flights.\n"; return; }
    for (const auto& pair : flights) std::cout << *pair.second << '\n';
}

void Airline::searchFlightsByNumber(const std::string& number) const {
    auto it = flights.find(number);
    if (it == flights.end()) { std::cout << "No matching flight.\n"; return; }
    std::cout << *it->second << '\n';
}

void Airline::searchFlightsByRoute(const std::string& from, const std::string& to) const {
    for (const auto& pair : flights) {
        const auto& f = *pair.second;
        if (f.getOrigin() == from && f.getDestination() == to) std::cout << f << '\n';
    }
}

void Airline::searchFlightsByDate(const std::string& date) const {
    for (const auto& pair : flights) {
        if (datePart(pair.second->getDepartureDateTime()) == date) std::cout << *pair.second << '\n';
    }
}

void Airline::registerPassenger(std::unique_ptr<Passenger> passenger) {
    if (!passenger) throw std::invalid_argument("Null passenger.");
    const auto id = passenger->getId();
    if (passengers.count(id)) throw std::invalid_argument("Passenger already exists.");
    passengers[id] = std::move(passenger);
}

void Airline::removePassenger(const std::string& id) {
    requirePassenger(id);
    auto active = std::any_of(tickets.begin(), tickets.end(),
        [&](const Ticket& t) { return t.getPassengerId() == id && t.isActive(); });
    if (active) throw std::runtime_error("Cannot remove passenger with active bookings.");
    passengers.erase(id);
}

void Airline::listPassengers() const {
    for (const auto& pair : passengers) {
        pair.second->display(std::cout);
        std::cout << '\n';
    }
}

void Airline::bookingHistory(const std::string& id) const {
    requirePassenger(id);
    bool found = false;
    for (const auto& ticket : tickets) {
        if (ticket.getPassengerId() == id) {
            std::cout << ticket << '\n';
            found = true;
        }
    }
    if (!found) std::cout << "No booking history.\n";
}

Ticket& Airline::bookTicket(const std::string& passengerId, const std::string& flightNumber) {
    Passenger& passenger = requirePassenger(passengerId);
    Flight& flight = requireFlight(flightNumber);

    for (const auto& ticket : tickets) {
        if (ticket.getPassengerId() == passengerId &&
            ticket.getFlightNumber() == flightNumber && ticket.isActive()) {
            throw DuplicateBookingException("Passenger already has a ticket on this flight.");
        }
    }

    flight.reserveSeat();
    int seat = flight.getBookedSeats();
    double fare = flight.calculateBaseFare();
    if (passenger.getType() == "Business") fare *= 1.50;
    else if (passenger.getType() == "FirstClass") fare *= 2.00;

    std::ostringstream id;
    id << 'T' << nextTicketNumber++;
    tickets.emplace_back(id.str(), passengerId, flightNumber, seat, fare);
    return tickets.back();
}

double Airline::cancelTicket(const std::string& ticketId) {
    auto it = std::find_if(tickets.begin(), tickets.end(),
        [&](const Ticket& t) { return t.getId() == ticketId; });
    if (it == tickets.end()) throw NotFoundException("Ticket not found.");
    if (!it->isActive()) throw InvalidCancellationException("Ticket is already cancelled.");

    Passenger& passenger = requirePassenger(it->getPassengerId());
    Flight& flight = requireFlight(it->getFlightNumber());

    // The assignment requires refund to depend on passenger class and proximity to departure.
    // This implementation uses three simple time bands.
    std::string dep = flight.getDepartureDateTime();
    double percentage = passenger.cancellationPercentage();
    std::time_t now = std::time(nullptr);
    std::tm tm{};
    std::istringstream ss(dep);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M");
    double hours = 48.0;
    if (!ss.fail()) {
        std::time_t departure = std::mktime(&tm);
        if (departure != static_cast<std::time_t>(-1))
            hours = std::difftime(departure, now) / 3600.0;
    }
    if (hours < 6) percentage *= 0.25;
    else if (hours < 24) percentage *= 0.60;

    double refund = it->getFarePaid() * percentage;
    it->cancel();
    flight.releaseSeat();
    return refund;
}

void Airline::todaysDepartures(const std::string& date) const {
    std::cout << "Departures on " << date << ":\n";
    searchFlightsByDate(date);
}

void Airline::occupancyReport() const {
    std::cout << "\nOccupancy Report\n";
    for (const auto& pair : flights) {
        const auto& f = *pair.second;
        double occupancy = f.getTotalSeats() == 0 ? 0.0 :
            100.0 * f.getBookedSeats() / f.getTotalSeats();
        std::cout << f.getFlightNumber() << " | "
                  << std::fixed << std::setprecision(1)
                  << occupancy << "%\n";
    }
}

void Airline::topRevenueFlights() const {
    std::map<std::string, double> revenue;
    for (const auto& ticket : tickets)
        if (ticket.isActive()) revenue[ticket.getFlightNumber()] += ticket.getFarePaid();

    std::vector<std::pair<std::string, double>> ranking(revenue.begin(), revenue.end());
    std::sort(ranking.begin(), ranking.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    std::cout << "\nTop Revenue Flights\n";
    int count = 0;
    for (const auto& item : ranking) {
        if (count++ == 5) break;
        std::cout << item.first << " | PKR "
                  << std::fixed << std::setprecision(2) << item.second << '\n';
    }
}

void Airline::save(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out) throw std::runtime_error("Unable to open save file.");

    out << "[FLIGHTS]\n";
    for (const auto& pair : flights) out << pair.second->serialize() << '\n';

    out << "[PASSENGERS]\n";
    for (const auto& pair : passengers) out << pair.second->serialize() << '\n';

    out << "[TICKETS]\n";
    for (const auto& ticket : tickets) out << ticket.serialize() << '\n';

    out << "[META]\n" << nextTicketNumber << '\n';
}

void Airline::load(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) return; // First run: no save file is normal.

    flights.clear();
    passengers.clear();
    tickets.clear();

    std::string section, line;
    while (std::getline(in, line)) {
        if (line == "[FLIGHTS]" || line == "[PASSENGERS]" ||
            line == "[TICKETS]" || line == "[META]") {
            section = line;
            continue;
        }
        if (line.empty()) continue;

        auto p = split(line);
        try {
            if (section == "[FLIGHTS]" && p.size() >= 7) {
                std::string type = p[0];
                if (type == "Domestic") {
                    addFlight(std::make_unique<DomesticFlight>(
                        p[1], p[2], p[3], p[4], std::stoi(p[5]), std::stod(p[6])));
                } else if (type == "International") {
                    addFlight(std::make_unique<InternationalFlight>(
                        p[1], p[2], p[3], p[4], std::stoi(p[5]), std::stoi(p[6]) != 0));
                } else if (type == "Charter" && p.size() >= 7) {
                    addFlight(std::make_unique<CharterFlight>(
                        p[1], p[2], p[3], p[4], std::stoi(p[5]), p[6]));
                }
            } else if (section == "[PASSENGERS]" && p.size() >= 4) {
                if (p[0] == "Economy") registerPassenger(
                    std::make_unique<EconomyPassenger>(p[1], p[2], p[3]));
                else if (p[0] == "Business") registerPassenger(
                    std::make_unique<BusinessPassenger>(p[1], p[2], p[3]));
                else if (p[0] == "FirstClass") registerPassenger(
                    std::make_unique<FirstClassPassenger>(p[1], p[2], p[3]));
            } else if (section == "[TICKETS]" && p.size() >= 6) {
                tickets.emplace_back(p[0], p[1], p[2], std::stoi(p[3]), std::stod(p[4]));
                if (p[5] == "Cancelled") tickets.back().cancel();
                else {
                    auto fit = flights.find(p[2]);
                    if (fit != flights.end()) fit->second->reserveSeat();
                }
            } else if (section == "[META]") {
                nextTicketNumber = std::stoi(p[0]);
            }
        } catch (...) {
            // Ignore malformed individual records and continue loading.
        }
    }
}

void Airline::seedSampleData() {
    if (!flights.empty() || !passengers.empty()) return;

    addFlight(std::make_unique<DomesticFlight>("SK101","Islamabad","Karachi","2026-09-21 09:00",120,1.0));
    addFlight(std::make_unique<DomesticFlight>("SK102","Lahore","Islamabad","2026-09-21 13:30",100,0.9));
    addFlight(std::make_unique<DomesticFlight>("SK103","Karachi","Lahore","2026-09-22 10:00",110,1.1));
    addFlight(std::make_unique<DomesticFlight>("SK104","Islamabad","Gilgit","2026-09-22 08:00",70,1.3));
    addFlight(std::make_unique<InternationalFlight>("SK201","Islamabad","Dubai","2026-09-23 18:00",180,true));
    addFlight(std::make_unique<InternationalFlight>("SK202","Lahore","Doha","2026-09-24 16:00",170,true));
    addFlight(std::make_unique<InternationalFlight>("SK203","Karachi","Istanbul","2026-09-25 20:00",190,false));
    addFlight(std::make_unique<CharterFlight>("SK301","Islamabad","Skardu","2026-09-26 07:30",80,"Mountain Tours"));
    addFlight(std::make_unique<CharterFlight>("SK302","Lahore","Hunza","2026-09-27 06:30",60,"Northern Adventures"));
    addFlight(std::make_unique<DomesticFlight>("SK105","Peshawar","Islamabad","2026-09-28 11:00",90,0.95));

    registerPassenger(std::make_unique<EconomyPassenger>("P001","Ali Khan","0300-1111111"));
    registerPassenger(std::make_unique<EconomyPassenger>("P002","Sara Ahmed","0301-2222222"));
    registerPassenger(std::make_unique<EconomyPassenger>("P003","Hamza Malik","0302-3333333"));
    registerPassenger(std::make_unique<BusinessPassenger>("P004","Ayesha Noor","0303-4444444"));
    registerPassenger(std::make_unique<BusinessPassenger>("P005","Bilal Shah","0304-5555555"));
    registerPassenger(std::make_unique<FirstClassPassenger>("P006","Zainab Ali","0305-6666666"));
    registerPassenger(std::make_unique<FirstClassPassenger>("P007","Usman Tariq","0306-7777777"));
    registerPassenger(std::make_unique<EconomyPassenger>("P008","Mariam Iqbal","0307-8888888"));

    bookTicket("P001","SK101");
    bookTicket("P004","SK201");
    bookTicket("P006","SK301");
}

void Airline::menu() {
    while (true) {
        std::cout << "\n=== SkyLink Airways ===\n"
                  << "1. List flights\n2. Search flight by number\n3. Search by route\n"
                  << "4. Search by date\n5. List passengers\n6. Register passenger\n"
                  << "7. Book ticket\n8. Passenger booking history\n9. Cancel ticket\n"
                  << "10. Today's departures\n11. Occupancy report\n12. Top 5 revenue flights\n"
                  << "13. Add domestic flight\n14. Save\n0. Exit\nChoice: ";
        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear(); std::cin.ignore(10000, '\n');
            std::cout << "Invalid input.\n"; continue;
        }
        try {
            if (choice == 0) { save("data/airline_data.txt"); std::cout << "Saved. Goodbye.\n"; return; }
            if (choice == 1) listFlights();
            else if (choice == 2) { std::string n; std::cout << "Flight number: "; std::cin >> n; searchFlightsByNumber(n); }
            else if (choice == 3) { std::string a,b; std::cout<<"Origin: "; std::cin>>a; std::cout<<"Destination: "; std::cin>>b; searchFlightsByRoute(a,b); }
            else if (choice == 4) { std::string d; std::cout<<"Date (YYYY-MM-DD): "; std::cin>>d; searchFlightsByDate(d); }
            else if (choice == 5) listPassengers();
            else if (choice == 6) {
                std::string id,n,p; int type;
                std::cout<<"ID: "; std::cin>>id; std::cout<<"Name: "; std::cin.ignore(); std::getline(std::cin,n);
                std::cout<<"Phone: "; std::cin>>p; std::cout<<"1 Economy 2 Business 3 First: "; std::cin>>type;
                if(type==1) registerPassenger(std::make_unique<EconomyPassenger>(id,n,p));
                else if(type==2) registerPassenger(std::make_unique<BusinessPassenger>(id,n,p));
                else if(type==3) registerPassenger(std::make_unique<FirstClassPassenger>(id,n,p));
                else throw std::invalid_argument("Invalid passenger type.");
            }
            else if (choice == 7) {
                std::string p,f; std::cout<<"Passenger ID: "; std::cin>>p; std::cout<<"Flight: "; std::cin>>f;
                std::cout << "Booked: " << bookTicket(p,f) << '\n';
            }
            else if (choice == 8) { std::string p; std::cout<<"Passenger ID: "; std::cin>>p; bookingHistory(p); }
            else if (choice == 9) {
                std::string t; std::cout<<"Ticket ID: "; std::cin>>t;
                std::cout<<"Refund: PKR "<<std::fixed<<std::setprecision(2)<<cancelTicket(t)<<'\n';
            }
            else if (choice == 10) { std::string d; std::cout<<"Date: "; std::cin>>d; todaysDepartures(d); }
            else if (choice == 11) occupancyReport();
            else if (choice == 12) topRevenueFlights();
            else if (choice == 13) {
                std::string n,a,b,d; int s; double factor;
                std::cout<<"Number: "; std::cin>>n; std::cout<<"Origin: "; std::cin>>a;
                std::cout<<"Destination: "; std::cin>>b; std::cout<<"Departure (YYYY-MM-DD HH:MM): ";
                std::cin.ignore(); std::getline(std::cin,d); std::cout<<"Seats: "; std::cin>>s;
                std::cout<<"Route factor: "; std::cin>>factor;
                addFlight(std::make_unique<DomesticFlight>(n,a,b,d,s,factor));
            }
            else if (choice == 14) { save("data/airline_data.txt"); std::cout<<"Saved.\n"; }
            else std::cout<<"Unknown option.\n";
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << '\n';
        }
    }
}
