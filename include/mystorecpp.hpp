#ifndef MYSTORE_CPP_H
#define MYSTORE_CPP_H

#include <string>
#include <unordered_map>
#include <map>
#include <initializer_list>
#include <vector>

#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransport.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>

#include "replicaservice_types.h"
#include "ReplicaService.h"

namespace mystore {
    class PutFailureException {};

    class GetFailureException {};

    class DelFailureException {};

    class Client {
        private:
            std::vector<std::string> endpoints;

            std::vector<std::string> getHelper(std::string, std::string, int, int);
            bool putHelper(std::string, std::string, std::string, int);
            bool delKeyHelper(std::string, std::string, int);
            std::pair<std::string, int> getLeaderInfo();
            std::pair<std::string, int> getRandomReplica();
            int getNextRequestIdentifier();
            void setNextRequestIdentifier(int);

            static bool isNullID(const ID&);

            static const char* REQUEST_IDENTIFIER_ENV_VAR_NAME;
            static const char* MOST_RECENT_LEADER_ADDR_ENV_VAR_NAME;
            static const char* RPC_TIMEOUT_ENV_VAR_NAME;
            static const char* NUM_REQUEST_RETRIES_ENV_VAR_NAME;
            static const char* REST_PERIOD_BETWEEN_CALLS_ENV_VAR_NAME;
            static const char* NUM_RETRY_CYCLES_BEFORE_QUITTING_ENV_VAR_NAME;
            static const char* CLUSTER_MEMBERSHIP_FILE_NAME_ENV_VAR_NAME;

        public:
            Client(std::initializer_list<std::string>);
            bool put(std::string, std::string);
            std::vector<std::string> get(std::string, int=0);
            void killReplica(std::string);
            std::unordered_map<std::string, std::map<std::string, std::string>> getInformation();
            bool delKey(std::string);
    };
}

#endif
