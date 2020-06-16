#include <string>
#include <iostream>
#include <vector>

#include "mystorecpp.hpp"

std::ostream&
operator<<(std::ostream& os, const std::vector<std::string>& values) {
    os << "[";
    for(unsigned int i = 0; i < values.size(); ++i) {
        os << values.at(i);

        if(i < values.size()-1) {
            os << ", ";
        }
    }
    os << "]";

    return os;
}

int main() {
    mystore::Client client({"127.0.1.1:5000", "127.0.1.1:5001", "127.0.1.1:5002"});

    bool res = client.put("akey", "aval");
    if(res) {
        std::cout << "Put request successful\n";
    }

    std::vector<std::string> val = client.get("akey", 0);
    std::cout << "Retrieved value: " << val << "\n";

    auto info = client.getInformation();

    return 0;
}
