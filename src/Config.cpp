// Filename: Config.cpp

#include "Config.h"


std::ostream& operator<<(std::ostream& os, Peer& peer)
{
    os << peer.represent();
    return os;
}

pid_t gettid()
{
    return syscall(SYS_gettid);
}

void Config::add_peer(std::string& peer)
{
    std::stringstream tin (peer);
    std::string host;
    auto port {0u};
    char delimiter {':'};

    host.resize(100);
    tin.getline(host.data(), host.size(), delimiter);
    tin >> port;

    if (tin.fail())
    {
        error_return(this, "Invalid peer specification: ", peer, "; state=",
                name_statebits(tin.rdstate()));
    }

    this->peers.push_back(Peer{host, port});

    debug_print(this, "Added a peer: ", this->peers[this->peers.size() - 1]);
}
