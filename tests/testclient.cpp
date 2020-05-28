#include <string>
#include <iostream>

#include "mystorecpp.hpp"

int main() {
    mystore::Client client({"127.0.1.1:5000", "127.0.1.1:5001", "127.0.1.1:5002"});

    bool res = client.put("akey", "aval");
    if(res) {
        std::cout << "Put request successful\n";
    }

    std::string val = client.get("akey");
    std::cout << "Retrieved value: " << val << "\n";

    auto info = client.getInformation();

    return 0;
}
