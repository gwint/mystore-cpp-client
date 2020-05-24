## mystorecpp - A C++ client for mystore

mystorecpp is the official C++ client for mystore v1.0.

### Usage
```cpp
#include "mystorecpp"
#include <iostream>

int main() {

    // create client
    mystore::client client("127.0.1.1:5000",
                           "127.0.1.1:5001",
                           "127.0.1.1:5002");

    // create mapping
    bool result = client.put("akey", "aval");
    if(result) {
        std::cout << "put successful\n";
    }

    // find value associated with a key
    std::string val = client.get("akey");
    if(val == "aval") {
        std::cout << "get successful\n";
    }

    return 0;
}
```


