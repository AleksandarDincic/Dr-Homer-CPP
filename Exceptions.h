#pragma once
#include <iostream>
#include <string>

class Exception {
private:
	std::string message;
public:
	Exception(const std::string& message) : message(message) {}
	const std::string& getMessage() const { return message; }
	friend std::ostream& operator<<(std::ostream& os, Exception& e) {
		return os << "ERROR: "<< e.message << std::endl;
	}
};

class BadInputException : public Exception {
public:
	BadInputException(const std::string& message) : Exception(message) {}
};

class BadPathException : public Exception {
public:
	BadPathException(const std::string& message) : Exception(message) {}
};

class BadFormatException : public Exception {
public:
	BadFormatException(const std::string& message) : Exception(message) {}
};