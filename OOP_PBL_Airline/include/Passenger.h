#ifndef PASSENGER_H
#define PASSENGER_H

#include <iostream>
#include <string>

class Passenger {
protected:
    std::string passengerId;
    std::string name;
    std::string phone;
public:
    Passenger(const std::string& id, const std::string& passengerName,
              const std::string& passengerPhone);
    virtual ~Passenger() = default;

    virtual int baggageAllowance() const = 0;
    virtual double loyaltyMultiplier() const = 0;
    virtual double cancellationPercentage() const = 0;
    virtual std::string getType() const = 0;

    const std::string& getId() const;
    const std::string& getName() const;
    const std::string& getPhone() const;

    virtual std::string serialize() const;
    virtual void display(std::ostream& os) const;
};

class EconomyPassenger : public Passenger {
public:
    using Passenger::Passenger;
    int baggageAllowance() const override;
    double loyaltyMultiplier() const override;
    double cancellationPercentage() const override;
    std::string getType() const override;
};

class BusinessPassenger : public Passenger {
public:
    using Passenger::Passenger;
    int baggageAllowance() const override;
    double loyaltyMultiplier() const override;
    double cancellationPercentage() const override;
    std::string getType() const override;
};

class FirstClassPassenger : public Passenger {
public:
    using Passenger::Passenger;
    int baggageAllowance() const override;
    double loyaltyMultiplier() const override;
    double cancellationPercentage() const override;
    std::string getType() const override;
};

#endif
