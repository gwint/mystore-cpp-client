#ifndef MYSTORE_CPP_H
#define MYSTORE_CPP_H

#include <string>
#include <unordered_map>

namespace client {
    class Client {
        private:
            bool getHelper(std::string, std::string, int);
            bool putHelper(std::string, std::string, std::string, int);

            static const char* REQUEST_IDENTIFIER_ENV_VAR_NAME;
            static const char* MOST_RECENT_LEADER_ADDR_ENV_VAR_NAME;
            static const char* RPC_TIMEOUT_ENV_VAR_NAME;
            static const char* NUM_REQUEST_RETRIES_ENV_VAR_NAME;
            static const char* REST_PERIOD_BETWEEN_CALLS_ENV_VAR_NAME;
            static const char* NUM_RETRY_CYCLES_BEFORE_QUITTING_ENV_VAR_NAME;
            static const char* CLUSTER_MEMBERSHIP_FILE_NAME_ENV_VAR_NAME;

        public:
            bool put(std::string, std::string);
            std::string get(std::string);
            void killReplica(std::string);
            std::unordered_map<std::string, std::unordered_map<std::string, std::string>> getInformation();
    };
}

#endif
