#include <initializer_list>
#include <string>
#include <utility>
#include <cstdlib>
#include <time.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <stdexcept>

#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransport.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>

#include "replicaservice_types.h"
#include "ReplicaService.h"
#include "dotenv.h"

#include "mystorecpp.hpp"

const char* mystore::Client::REQUEST_IDENTIFIER_ENV_VAR_NAME = "REQUEST_ID_FILE";
const char* mystore::Client::MOST_RECENT_LEADER_ADDR_ENV_VAR_NAME = "CURRENT_LEADER_INFO_FILE";
const char* mystore::Client::RPC_TIMEOUT_ENV_VAR_NAME = "CMD_LINE_TOOL_TIMEOUT_MS";
const char* mystore::Client::NUM_REQUEST_RETRIES_ENV_VAR_NAME = "CMD_LINE_TOTAL_NUM_RETRIES_PER_REQUEST";
const char* mystore::Client::REST_PERIOD_BETWEEN_CALLS_ENV_VAR_NAME = "CMD_LINE_TOOL_REST_PERIOD_BETWEEN_CALLS_MS";
const char* mystore::Client::NUM_RETRY_CYCLES_BEFORE_QUITTING_ENV_VAR_NAME = "CMD_LINE_TOOL_NUM_REQUEST_RETRY_CYCLES_BEFORE_QUITTING";
const char* mystore::Client::CLUSTER_MEMBERSHIP_FILE_NAME_ENV_VAR_NAME = "CLUSTER_MEMBERSHIP_FILE";

mystore::Client::Client(std::initializer_list<std::string> endpoints) {
    for(const std::string& endpoint : endpoints) {
        if(endpoint.find(':') == std::string::npos) {
            throw std::invalid_argument("Each endpoint must contain a colon");
        }

        this->endpoints.push_back(endpoint);
    }
}

bool
mystore::Client::put(std::string key, std::string value) {
    return false;
}

std::string
mystore::Client::get(std::string key) {
    return "";
}

void
mystore::Client::killReplica(std::string endpoint) {
}

std::unordered_map<std::string, std::unordered_map<std::string, std::string>>
mystore::Client::getInformation() {
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> info;

    return info;
}

bool
mystore::Client::getHelper(std::string key, std::string clientIdentifier, int requestIdentifier) {
    return false;
}

bool
mystore::Client::putHelper(std::string key, std::string value, std::string clientIdentifier, int requestIdentifier) {
    int numCyclesBeforeQuitting =
            atoi(dotenv::env[mystore::Client::NUM_RETRY_CYCLES_BEFORE_QUITTING_ENV_VAR_NAME].c_str());
    int numRetriesBeforeChangingRequestID =
            atoi(dotenv::env[mystore::Client::NUM_REQUEST_RETRIES_ENV_VAR_NAME].c_str());
    int pauseDurationAfterRetryMS =
            atoi(dotenv::env[mystore::Client::REST_PERIOD_BETWEEN_CALLS_ENV_VAR_NAME].c_str());

    std::pair<std::string, int> replicaInfo = this->getLeaderInfo();

    int currentRequestNumber = requestIdentifier;
    std::string host = replicaInfo.first;
    int port = replicaInfo.second;

    for(int cycleNum = 0; cycleNum < numCyclesBeforeQuitting; ++cycleNum) {
        for(int retryNum = 0; retryNum < numRetriesBeforeChangingRequestID; ++retryNum) {
        }
    }

    return false;
}

std::pair<std::string, int>
mystore::Client::getLeaderInfo() {
    std::ifstream leaderInfoFileObj(dotenv::env[mystore::Client::MOST_RECENT_LEADER_ADDR_ENV_VAR_NAME].c_str());
    if(leaderInfoFileObj.fail()) {
        std::ofstream leaderInfoFileObj(dotenv::env[mystore::Client::MOST_RECENT_LEADER_ADDR_ENV_VAR_NAME].c_str());
        std::pair<std::string, int> randomReplicaInfo = this->getRandomReplica();
        leaderInfoFileObj << randomReplicaInfo.first << randomReplicaInfo.second;

        return randomReplicaInfo;
    }
    else {
        std::string endpoint;
        leaderInfoFileObj >> endpoint;

        std::replace(endpoint.begin(), endpoint.end(), ':', ' ');

        std::stringstream endpointStream(endpoint);

        std::string hostname;
        int port;

        endpointStream >> hostname >> port;

        return std::make_pair(hostname, port);
    }
}

std::pair<std::string, int>
mystore::Client::getRandomReplica() {
    srand(time(NULL));

    unsigned int randIndex = rand() % this->endpoints.size();

    std::string& randomEndpoint = this->endpoints.at(randIndex);
    std::replace(randomEndpoint.begin(), randomEndpoint.end(), ':', ' ');

    std::stringstream endpointStream(randomEndpoint);

    std::string hostname;
    int port;

    endpointStream >> hostname >> port;

    return std::make_pair(hostname, port);
}

bool
mystore::Client::isNullID(const ID& id) {
    return id.hostname == "" && id.port == 0;
}
