#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

using std::map;
using std::set;
using std::string;
using std::ostream;
using std::vector;


namespace rules {
    extern map<string, vector<string>> default_operators;

    extern set<string> default_types;
    extern set<string> global_reserved_names;

}