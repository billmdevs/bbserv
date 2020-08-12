// Filename: Config.cpp

#include "Config.h"
#include <cstring>
#include <fstream>
#include <iomanip>
#include <istream>
#include <sstream>
#include <string>
#include <vector>


std::ostream& operator<<(std::ostream& os, Peer& peer)
{
    os << peer.represent();
    return os;
}

pid_t gettid() noexcept
{
    return syscall(SYS_gettid);
}

void Config::add_peer(std::string& peer)
{
    std::stringstream tin (peer);
    std::string host;
    std::array<char, 100> section;
    auto port {0u};
    char delimiter {':'};

    tin.getline(section.data(), section.size(), delimiter);
    host = section.data();
    tin >> port;

    if (tin.fail())
    {
        error_return(this, "Invalid peer specification: ", peer, "; state=",
                name_statebits(tin.rdstate()));
    }

    this->peers.push_back(Peer{host, port});

    debug_print(this, "Added a peer: ", this->peers[this->peers.size() - 1]);
}

void Config::clear_peers()
{
    this->peers.clear();
}

size_t Config::read_config_uint(std::string& value)
{
    std::istringstream sin (value);
    auto number {0ul};

    sin >> number;

    if (sin.fail())
    {
        error_return(this, "Failed to read a number from ", value);
    }

    return number;
}


bool Config::read_config_bool(std::string& value)
{
    std::istringstream sin (value);
    auto isTrue {false};

    sin >> isTrue;

    if (sin.fail())
    {
        std::string bvalue;
        std::istringstream sin2 (value);
        sin2 >> bvalue;

        if (sin2.fail())
        {
            error_return(this, "Failed to read a boolean from ", value);
        }

        isTrue = (0 == bvalue.find("true"));
    }

    return isTrue;
}

void Config::read_config_peers(std::string& line)
{
    std::istringstream sin (line);
    std::array<char, 1024> section;

    for (;sin.getline(section.data(), section.size(), ' ');)
    {
        std::string peer (section.data());
        Config::singleton().add_peer(peer);
    }
}

void Config::read_config_line(std::string line)
{
    std::istringstream sin (line);
    std::array<char, 100> keySection;
    std::array<char, 1024> valueSection;

    sin.getline(keySection.data(), keySection.size(), '=');
    sin.getline(valueSection.data(), valueSection.size());
    std::string key (keySection.data());
    std::string value (valueSection.data());

    if (0 == key.find("THMAX"))
    {
        Config::singleton().set_Tmax(read_config_uint(value));
    }
    else if (0 == key.find("BBFILE"))
    {
        Config::singleton().set_bbfile(value);
    }
    else if (0 == key.find("BBPORT"))
    {
        Config::singleton().set_bport(read_config_uint(value));
    }
    else if (0 == key.find("SYNCPORT"))
    {
        Config::singleton().set_sport(read_config_uint(value));
    }
    else if (0 == key.find("DAEMON"))
    {
        Config::singleton().set_daemon(read_config_bool(value));
    }
    else if (0 == key.find("DEBUG"))
    {
        Config::singleton().set_debug(read_config_bool(value));
    }
    else if (0 == key.find("PEERS"))
    {
        read_config_peers(value);
    }

}

void Config::read_config()
{
    std::ifstream fin (this->bbconf);
    std::array<char, 1024> line;

    if (fin.fail())
    {
        debug_print(this, "No config file ", std::quoted(this->bbconf),
                " available");

    }

    line.fill('\0');

    for (;fin.getline(line.data(), line.size());)
    {
        read_config_line(line.data());
    }
}
