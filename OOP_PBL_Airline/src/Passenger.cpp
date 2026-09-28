#include "Passenger.h"
#include <sstream>

Passenger::Passenger(const std::string& id, const std::string& passengerName,
                     const std::string& passengerPhone)
    : passengerId(id), name(passengerName), phone(passengerPhone) {}

const std::string& Passenger::getId() const { return passengerId; }
const std::string& Passenger::getName() const { return name; }
const std::string& Passenger::getPhone() const { return phone; }

std::string Passenger::serialize() const {
    std::ostringstream out;
    out << getType() << '|' << passengerId << '|' << name << '|' << phone;
    return out.str();
}

void Passenger::display(std::ostream& os) const {
    os << passengerId << " | " << name << " | " << getType()
       << " | Phone: " << phone
       << " | Baggage: " << baggageAllowance() << " kg";
}

int EconomyPassenger::baggageAllowance() const { return 20; }
double EconomyPassenger::loyaltyMultiplier() const { return 1.0; }
double EconomyPassenger::cancellationPercentage() const { return 0.60; }
std::string EconomyPassenger::getType() const { return "Economy"; }

int BusinessPassenger::baggageAllowance() const { return 30; }
double BusinessPassenger::loyaltyMultiplier() const { return 1.5; }
double BusinessPassenger::cancellationPercentage() const { return 0.80; }
std::string BusinessPassenger::getType() const { return "Business"; }

int FirstClassPassenger::baggageAllowance() const { return 40; }
double FirstClassPassenger::loyaltyMultiplier() const { return 2.0; }
double FirstClassPassenger::cancellationPercentage() const { return 0.90; }
std::string FirstClassPassenger::getType() const { return "FirstClass"; }
