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
#include <chrono>

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

using apache::thrift::transport::TTransportException;

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
            std::shared_ptr<apache::thrift::transport::TSocket> socket(new apache::thrift::transport::TSocket(host, port));
            socket->setConnTimeout(atoi(dotenv::env[mystore::Client::RPC_TIMEOUT_ENV_VAR_NAME].c_str()));
            socket->setSendTimeout(atoi(dotenv::env[mystore::Client::RPC_TIMEOUT_ENV_VAR_NAME].c_str()));
            socket->setRecvTimeout(atoi(dotenv::env[mystore::Client::RPC_TIMEOUT_ENV_VAR_NAME].c_str()));
            std::shared_ptr<apache::thrift::transport::TTransport> transport(new apache::thrift::transport::TBufferedTransport(socket));
            std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(new apache::thrift::protocol::TBinaryProtocol(transport));
            ReplicaServiceClient client(protocol);

            try {
                transport->open();

                try {
                    PutResponse putResponse;
                    client.put(putResponse, key, value, clientIdentifier, currentRequestNumber);

                    if(putResponse.success) {
                        std::ofstream leaderFileObj(dotenv::env[mystore::Client::MOST_RECENT_LEADER_ADDR_ENV_VAR_NAME].c_str());
                        leaderFileObj << host << ":" << port << std::endl;
                        this->setNextRequestIdentifier(currentRequestNumber+1);

                        return true;
                    }
                    else if(mystore::Client::isNullID(putResponse.leaderID)) {
                        std::pair<std::string, int> leaderInfo = this->getRandomReplica();
                        host = leaderInfo.first;
                        port = leaderInfo.second;
                    }
                    else {
                        host = putResponse.leaderID.hostname;
                        port = putResponse.leaderID.port;
                    }
                }
                catch(TTransportException& e) {
                }
            }
            catch(TTransportException& e) {
                std::pair<std::string, int> leaderInfo = this->getRandomReplica();
                host = leaderInfo.first;
                port = leaderInfo.second;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(pauseDurationAfterRetryMS*2));
        }

        ++currentRequestNumber;
    }

    throw mystore::PutFailureException();
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

void
mystore::Client::setNextRequestIdentifier(int nextRequestIdentifier) {
    std::ofstream requestIdentifierFileObj(dotenv::env[mystore::Client::REQUEST_IDENTIFIER_ENV_VAR_NAME].c_str());
    requestIdentifierFileObj << nextRequestIdentifier << std::endl;
    requestIdentifierFileObj.close();
}

int
mystore::Client::getNextRequestIdentifier() {
    std::ifstream requestIdentifierFileObj(dotenv::env[mystore::Client::REQUEST_IDENTIFIER_ENV_VAR_NAME].c_str());
    if(requestIdentifierFileObj.fail()) {
        std::ofstream requestIdentifierFileObj(dotenv::env[mystore::Client::REQUEST_IDENTIFIER_ENV_VAR_NAME].c_str());
        requestIdentifierFileObj << 0 << std::endl;
        requestIdentifierFileObj.close();

        return 0;
    }

    int nextIdentifier;
    requestIdentifierFileObj >> nextIdentifier;

    requestIdentifierFileObj.close();

    return nextIdentifier;
}
