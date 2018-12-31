#include "Utility.h"
using namespace std;
std::string_view::iterator skip_past(std::string_view::iterator iter, std::string_view::iterator end, char delimiter) {
    while(iter != end && *iter++ != delimiter)
        ;
    return iter;
}
