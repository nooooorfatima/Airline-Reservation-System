#include "Ticket.h"
#include <sstream>

Ticket::Ticket(const std::string& id, const std::string& passenger,
               const std::string& flight, int seat, double fare)
    : ticketId(id), passengerId(passenger), flightNumber(flight),
      seatNumber(seat), farePaid(fare), status(BookingStatus::Confirmed) {}

const std::string& Ticket::getId() const { return ticketId; }
const std::string& Ticket::getPassengerId() const { return passengerId; }
const std::string& Ticket::getFlightNumber() const { return flightNumber; }
int Ticket::getSeatNumber() const { return seatNumber; }
double Ticket::getFarePaid() const { return farePaid; }
BookingStatus Ticket::getStatus() const { return status; }

void Ticket::cancel() { status = BookingStatus::Cancelled; }
bool Ticket::isActive() const { return status == BookingStatus::Confirmed; }

std::string Ticket::serialize() const {
    std::ostringstream out;
    out << ticketId << '|' << passengerId << '|' << flightNumber << '|'
        << seatNumber << '|' << farePaid << '|'
        << (isActive() ? "Confirmed" : "Cancelled");
    return out.str();
}

void Ticket::display(std::ostream& os) const {
    os << ticketId << " | Passenger: " << passengerId
       << " | Flight: " << flightNumber
       << " | Seat: " << seatNumber
       << " | Fare: PKR " << farePaid
       << " | Status: " << (isActive() ? "Confirmed" : "Cancelled");
}

bool operator==(const Ticket& lhs, const Ticket& rhs) {
    return lhs.ticketId == rhs.ticketId;
}

std::ostream& operator<<(std::ostream& os, const Ticket& ticket) {
    ticket.display(os);
    return os;
}
