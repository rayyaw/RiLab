#include <string>

#include "utils.h"

using std::string;

// Taken from https://linuxhint.com/check-integer-input-cpp/. Checks if a string is an integer.
bool isInt(string v) {
    for (size_t i = 0; i < v.length(); i++) {
        if (!isdigit(v[i])) return false;
    }
        
    return true;
}

// Modified from the code above. Checks if the string is a valid float.
bool isFloat(string v) {
    bool no_dot = true;
    for (size_t i = 0; i < v.length(); i++) {
        if (!isdigit(v[i])) {
            if (v[i] == '.' && no_dot) no_dot = false;
            else return false;
        }

    }

    return true;
}