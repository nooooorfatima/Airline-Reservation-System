# SkyLink Airways — Airline Reservation & Flight Management System

## Overview
A console-based C++17 airline reservation prototype implementing the required PBL concepts:
- Abstract classes and pure virtual functions
- Encapsulation, inheritance, abstraction, polymorphism
- Flight and passenger hierarchies
- Ticket booking, seat allocation and cancellation/refunds
- STL `vector`, `map`, `sort`, `find_if`
- Generic function template
- Custom exception classes
- Text-file persistence
- Operator overloading

## Folder Structure
```text
OOP_PBL_Airline/
├── include/
├── src/
├── data/
├── docs/
├── Makefile
└── README.md
```

## Requirements
- g++ with C++17 support
- GNU Make

## Build
```bash
make
```

## Run
```bash
./airline
```
or:
```bash
make run
```

## Clean
```bash
make clean
```

## Persistence
The program reads and writes:
```text
data/airline_data.txt
```
The file is created/updated when the program saves or exits.

## Main Classes
`Flight` is abstract and has three derived classes:
- `DomesticFlight`
- `InternationalFlight`
- `CharterFlight`

`Passenger` is abstract and has:
- `EconomyPassenger`
- `BusinessPassenger`
- `FirstClassPassenger`

`Airline` aggregates flights, passengers and tickets.

## OOP Demonstration
- Encapsulation: data members are protected/private and accessed through methods.
- Abstraction: `Flight` and `Passenger` define abstract interfaces.
- Inheritance: specialized flight/passenger classes inherit common behavior.
- Polymorphism: virtual fare, display, passenger rules and serialization.
- Operator overloading: `<<` for Flight/Ticket and `==` for Ticket.
- Template: `genericSearch`.
- Exceptions: custom runtime exceptions for full flights, duplicate bookings and invalid cancellations.

## Notes for Viva
Be able to explain:
1. Why `Flight` is abstract.
2. Why `std::unique_ptr` is used.
3. How virtual dispatch selects the derived fare.
4. How duplicate bookings are detected.
5. How a seat is released after cancellation.
6. How save/load reconstructs objects.
7. How `std::sort` creates the revenue ranking.
8. Why the airline owns the flight/passenger objects.

## Sample Credentials/Data
No login is required. The application seeds 10 flights and 8 passengers on an empty data file.
