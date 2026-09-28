#ifndef TICKET_H
#define TICKET_H

#include "Flight.h"
#include "Passenger.h"
#include <iostream>
#include <string>

enum class BookingStatus { Confirmed, Cancelled };

class Ticket {
    std::string ticketId;
    std::string passengerId;
    std::string flightNumber;
    int seatNumber;
    double farePaid;
    BookingStatus status;
public:
    Ticket(const std::string& id, const std::string& passenger,
           const std::string& flight, int seat, double fare);

    const std::string& getId() const;
    const std::string& getPassengerId() const;
    const std::string& getFlightNumber() const;
    int getSeatNumber() const;
    double getFarePaid() const;
    BookingStatus getStatus() const;

    void cancel();
    bool isActive() const;
    std::string serialize() const;
    void display(std::ostream& os) const;

    friend bool operator==(const Ticket& lhs, const Ticket& rhs);
    friend std::ostream& operator<<(std::ostream& os, const Ticket& ticket);
};

#endif
