#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <iomanip>

using namespace std;

class Vehicle
{
private:
    int id;
    string model;
    double rate;
    bool available;

public:
    Vehicle(int i, string m, double r)
    {
        id = i;
        model = m;
        rate = r;
        available = true;
    }

    virtual ~Vehicle() {}

    int getId() const { return id; }
    string getModel() const { return model; }
    double getRate() const { return rate; }
    bool isAvailable() const { return available; }

    void setAvailable(bool value)
    {
        available = value;
    }

    virtual double calculateCost(int days) const = 0;
    virtual string getType() const = 0;

    virtual void showInfo() const
    {
        cout << id << " - " << getType()
             << " - " << model
             << " - $" << rate << " per day - "
             << (available ? "Available" : "Rented") << endl;
    }
};

class Car : public Vehicle
{
private:
    int seats;

public:
    Car(int i, string m, double r, int s)
        : Vehicle(i, m, r)
    {
        seats = s;
    }

    double calculateCost(int days) const override
    {
        return getRate() * days;
    }

    string getType() const override
    {
        return "Car";
    }

    void showInfo() const override
    {
        Vehicle::showInfo();
        cout << "   Seats: " << seats << endl;
    }
};

class Motorbike : public Vehicle
{
private:
    int engine;

public:
    Motorbike(int i, string m, double r, int e)
        : Vehicle(i, m, r)
    {
        engine = e;
    }

    double calculateCost(int days) const override
    {
        double cost = getRate() * days;

        if (days > 7)
            cost = cost * 0.90;

        return cost;
    }

    string getType() const override
    {
        return "Motorbike";
    }

    void showInfo() const override
    {
        Vehicle::showInfo();
        cout << "   Engine: " << engine << " CC" << endl;
    }
};

class Truck : public Vehicle
{
private:
    double payload;

public:
    Truck(int i, string m, double r, double p)
        : Vehicle(i, m, r)
    {
        payload = p;
    }

    double calculateCost(int days) const override
    {
        return getRate() * days * 1.20;
    }

    string getType() const override
    {
        return "Truck";
    }

    void showInfo() const override
    {
        Vehicle::showInfo();
        cout << "   Payload: " << payload << " tonnes" << endl;
    }
};

class Customer
{
private:
    int id;
    string name;
    bool renting;

public:
    Customer(int i, string n)
    {
        id = i;
        name = n;
        renting = false;
    }

    int getId() const { return id; }
    string getName() const { return name; }
    bool hasRental() const { return renting; }

    void setRental(bool value)
    {
        renting = value;
    }
};

class Rental
{
private:
    int id;
    Customer* customer;
    Vehicle* vehicle;
    int days;
    double cost;
    bool active;

public:
    Rental(int i, Customer* c, Vehicle* v, int d)
    {
        id = i;
        customer = c;
        vehicle = v;
        days = d;
        cost = vehicle->calculateCost(days);
        active = true;
    }

    bool isActive() const { return active; }
    Customer* getCustomer() const { return customer; }
    Vehicle* getVehicle() const { return vehicle; }

    void close()
    {
        active = false;
    }

    void show() const
    {
        cout << "Rental " << id
             << " | Customer: " << customer->getName()
             << " | Vehicle: " << vehicle->getModel()
             << " | Days: " << days
             << " | Cost: $" << fixed << setprecision(2) << cost
             << " | " << (active ? "Active" : "Closed")
             << endl;
    }
};

class RentalSystem
{
private:
    vector<unique_ptr<Vehicle>> vehicles;
    vector<Customer> customers;
    vector<Rental> rentals;
    int nextRentalId = 1;

    Vehicle* findVehicle(int id)
    {
        for (auto& v : vehicles)
        {
            if (v->getId() == id)
                return v.get();
        }
        return nullptr;
    }

    Customer* findCustomer(int id)
    {
        for (auto& c : customers)
        {
            if (c.getId() == id)
                return &c;
        }
        return nullptr;
    }

public:
    void addVehicle(unique_ptr<Vehicle> vehicle)
    {
        vehicles.push_back(move(vehicle));
    }

    void addCustomer(int id, string name)
    {
        if (findCustomer(id) != nullptr)
        {
            cout << "Customer ID already exists." << endl;
            return;
        }

        customers.emplace_back(id, name);
    }

    void showVehicles()
    {
        cout << "\n--- Vehicle List ---\n";

        for (auto& v : vehicles)
            v->showInfo();
    }

    void rentVehicle(int customerId, int vehicleId, int days)
    {
        Customer* customer = findCustomer(customerId);
        Vehicle* vehicle = findVehicle(vehicleId);

        if (customer == nullptr)
        {
            cout << "Customer not found." << endl;
            return;
        }

        if (vehicle == nullptr)
        {
            cout << "Vehicle not found." << endl;
            return;
        }

        if (days <= 0)
        {
            cout << "Number of days must be greater than zero." << endl;
            return;
        }

        if (customer->hasRental())
        {
            cout << "Customer already has an active rental." << endl;
            return;
        }

        if (!vehicle->isAvailable())
        {
            cout << "Vehicle is already rented." << endl;
            return;
        }

        rentals.emplace_back(nextRentalId++, customer, vehicle, days);

        vehicle->setAvailable(false);
        customer->setRental(true);

        cout << "Rental successful for "
             << customer->getName() << "." << endl;
    }

    void returnVehicle(int customerId)
    {
        Customer* customer = findCustomer(customerId);

        if (customer == nullptr)
        {
            cout << "Customer not found." << endl;
            return;
        }

        for (auto& rental : rentals)
        {
            if (rental.getCustomer()->getId() == customerId &&
                rental.isActive())
            {
                rental.getVehicle()->setAvailable(true);
                rental.close();
                customer->setRental(false);

                cout << "Vehicle returned successfully." << endl;
                return;
            }
        }

        cout << "No active rental found for this customer." << endl;
    }

    void showSummary()
    {
        int available = 0;
        int rented = 0;

        for (auto& v : vehicles)
        {
            if (v->isAvailable())
                available++;
            else
                rented++;
        }

        cout << "\n==============================\n";
        cout << "       RENTAL SUMMARY\n";
        cout << "==============================\n";

        cout << "Available vehicles: " << available << endl;
        cout << "Rented vehicles: " << rented << endl;

        cout << "\nActive Rentals:\n";

        bool found = false;

        for (auto& r : rentals)
        {
            if (r.isActive())
            {
                r.show();
                found = true;
            }
        }

        if (!found)
            cout << "No active rentals." << endl;

        cout << "==============================\n";
    }
};

int main()
{
    RentalSystem system;

    cout << "Vehicle Rental System\n";

    system.addVehicle(
        make_unique<Car>(101, "Toyota Corolla", 50, 5)
    );

    system.addVehicle(
        make_unique<Motorbike>(102, "Honda 150", 25, 150)
    );

    system.addVehicle(
        make_unique<Truck>(103, "Isuzu Truck", 100, 5)
    );

    system.showVehicles();

    cout << "\n--- Customers ---\n";

    system.addCustomer(1, "Ali");
    system.addCustomer(2, "Ahmed");

    cout << "Ali and Ahmed registered successfully.\n";

    cout << "\n--- First Rental ---\n";
    system.rentVehicle(1, 101, 3);

    cout << "\n--- Second Rental ---\n";
    system.rentVehicle(2, 102, 10);

    cout << "\n--- Testing Rented Vehicle ---\n";
    system.rentVehicle(2, 101, 2);

    cout << "\n--- Returning Vehicle ---\n";
    system.returnVehicle(1);

    system.showSummary();

    return 0;
}
