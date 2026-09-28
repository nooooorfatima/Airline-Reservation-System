#ifndef FLIGHT_H
#define FLIGHT_H

#include <iostream>
#include <string>

class Flight {
protected:
    std::string flightNumber;
    std::string origin;
    std::string destination;
    std::string departureDateTime;
    int totalSeats;
    int availableSeats;

public:
    Flight(const std::string& number, const std::string& from,
           const std::string& to, const std::string& departure,
           int seats);
    virtual ~Flight() = default;

    virtual double calculateBaseFare() const = 0;
    virtual std::string getType() const = 0;
    virtual void displayDetails(std::ostream& os) const;

    const std::string& getFlightNumber() const;
    const std::string& getOrigin() const;
    const std::string& getDestination() const;
    const std::string& getDepartureDateTime() const;
    int getTotalSeats() const;
    int getAvailableSeats() const;
    int getBookedSeats() const;

    void reserveSeat();
    void releaseSeat();

    virtual std::string serialize() const;
    friend std::ostream& operator<<(std::ostream& os, const Flight& flight);
};

class DomesticFlight : public Flight {
    double routeFactor;
public:
    DomesticFlight(const std::string& number, const std::string& from,
                    const std::string& to, const std::string& departure,
                    int seats, double factor = 1.0);
    double calculateBaseFare() const override;
    std::string getType() const override;
    void displayDetails(std::ostream& os) const override;
    std::string serialize() const override;
};

class InternationalFlight : public Flight {
    bool visaRequired;
public:
    InternationalFlight(const std::string& number, const std::string& from,
                        const std::string& to, const std::string& departure,
                        int seats, bool visa);
    double calculateBaseFare() const override;
    std::string getType() const override;
    void displayDetails(std::ostream& os) const override;
    std::string serialize() const override;
};

class CharterFlight : public Flight {
    std::string contractHolder;
public:
    CharterFlight(const std::string& number, const std::string& from,
                  const std::string& to, const std::string& departure,
                  int seats, const std::string& holder);
    double calculateBaseFare() const override;
    std::string getType() const override;
    void displayDetails(std::ostream& os) const override;
    std::string serialize() const override;
    const std::string& getContractHolder() const;
};

#endif
