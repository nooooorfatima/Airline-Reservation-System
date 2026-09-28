#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

class FlightFullException : public std::runtime_error {
public:
    explicit FlightFullException(const std::string& message)
        : std::runtime_error(message) {}
};

class InvalidCancellationException : public std::runtime_error {
public:
    explicit InvalidCancellationException(const std::string& message)
        : std::runtime_error(message) {}
};

class DuplicateBookingException : public std::runtime_error {
public:
    explicit DuplicateBookingException(const std::string& message)
        : std::runtime_error(message) {}
};

class NotFoundException : public std::runtime_error {
public:
    explicit NotFoundException(const std::string& message)
        : std::runtime_error(message) {}
};

#endif
