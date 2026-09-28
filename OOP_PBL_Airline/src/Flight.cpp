#include "Flight.h"
#include "Exceptions.h"
#include <iomanip>
#include <sstream>

Flight::Flight(const std::string& number, const std::string& from,
               const std::string& to, const std::string& departure,
               int seats)
    : flightNumber(number), origin(from), destination(to),
      departureDateTime(departure), totalSeats(seats), availableSeats(seats) {
    if (seats <= 0) throw std::invalid_argument("Total seats must be positive.");
}

const std::string& Flight::getFlightNumber() const { return flightNumber; }
const std::string& Flight::getOrigin() const { return origin; }
const std::string& Flight::getDestination() const { return destination; }
const std::string& Flight::getDepartureDateTime() const { return departureDateTime; }
int Flight::getTotalSeats() const { return totalSeats; }
int Flight::getAvailableSeats() const { return availableSeats; }
int Flight::getBookedSeats() const { return totalSeats - availableSeats; }

void Flight::reserveSeat() {
    if (availableSeats <= 0)
        throw FlightFullException("Flight " + flightNumber + " is full.");
    --availableSeats;
}

void Flight::releaseSeat() {
    if (availableSeats < totalSeats) ++availableSeats;
}

void Flight::displayDetails(std::ostream& os) const {
    os << flightNumber << " | " << getType() << " | "
       << origin << " -> " << destination
       << " | Departure: " << departureDateTime
       << " | Seats: " << getBookedSeats() << "/" << totalSeats
       << " | Base fare: PKR " << std::fixed << std::setprecision(2)
       << calculateBaseFare();
}

std::string Flight::serialize() const {
    std::ostringstream out;
    out << getType() << '|' << flightNumber << '|' << origin << '|'
        << destination << '|' << departureDateTime << '|' << totalSeats;
    return out.str();
}

std::ostream& operator<<(std::ostream& os, const Flight& flight) {
    flight.displayDetails(os);
    return os;
}

DomesticFlight::DomesticFlight(const std::string& number, const std::string& from,
                               const std::string& to, const std::string& departure,
                               int seats, double factor)
    : Flight(number, from, to, departure, seats), routeFactor(factor) {}

double DomesticFlight::calculateBaseFare() const { return 8500.0 * routeFactor; }
std::string DomesticFlight::getType() const { return "Domestic"; }
void DomesticFlight::displayDetails(std::ostream& os) const {
    Flight::displayDetails(os);
    os << " | Route factor: " << routeFactor;
}
std::string DomesticFlight::serialize() const {
    std::ostringstream out;
    out << Flight::serialize() << '|' << routeFactor;
    return out.str();
}

InternationalFlight::InternationalFlight(const std::string& number,
                                         const std::string& from,
                                         const std::string& to,
                                         const std::string& departure,
                                         int seats, bool visa)
    : Flight(number, from, to, departure, seats), visaRequired(visa) {}

double InternationalFlight::calculateBaseFare() const { return 45000.0 + (visaRequired ? 5000.0 : 0.0); }
std::string InternationalFlight::getType() const { return "International"; }
void InternationalFlight::displayDetails(std::ostream& os) const {
    Flight::displayDetails(os);
    os << " | Visa: " << (visaRequired ? "Required" : "Not required");
}
std::string InternationalFlight::serialize() const {
    std::ostringstream out;
    out << Flight::serialize() << '|' << (visaRequired ? 1 : 0);
    return out.str();
}

CharterFlight::CharterFlight(const std::string& number, const std::string& from,
                             const std::string& to, const std::string& departure,
                             int seats, const std::string& holder)
    : Flight(number, from, to, departure, seats), contractHolder(holder) {}

double CharterFlight::calculateBaseFare() const { return 75000.0; }
std::string CharterFlight::getType() const { return "Charter"; }
void CharterFlight::displayDetails(std::ostream& os) const {
    Flight::displayDetails(os);
    os << " | Contract holder: " << contractHolder;
}
std::string CharterFlight::serialize() const {
    std::ostringstream out;
    out << Flight::serialize() << '|' << contractHolder;
    return out.str();
}
const std::string& CharterFlight::getContractHolder() const { return contractHolder; }
