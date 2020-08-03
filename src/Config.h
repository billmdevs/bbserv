// Config.h

#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <vector>
#include "BBServException.h"
#include "BBServTimeout.h"

#ifdef SYS_gettid
pid_t gettid();
#else
#error "SYS_gettid unavailable on this system"
#endif

/**
 *Contains a peer's host-name and the port number.
 */
struct Peer
{
    std::string host;
    unsigned int port {0u};

    std::string represent() { return host + ":" + std::to_string(port); }
};

std::ostream& operator<<(std::ostream& os, Peer& peer);

/**
 *The singleton class providing configuration data for the other parts of the program.
 */
class Config
{
    protected:
        std::string bbfile { "bbfile.txt" };
        size_t Tmax { 20 };
        int bport { 9000 };
        int sport { 10000 };
        bool isDaemon { true };
        bool isDebug { false };
        std::vector<Peer> peers;

    public:
        static Config& singleton()
        {
            static Config object;
            return object;
        }

    public:
        /**
         *Returns the path to the bulletin board file.
         */
        const std::string& get_bbfile()         const { return bbfile; }
        /**
         *Returns the number of used threads.
         */
        size_t get_Tmax()                       const { return Tmax; }
        /**
         *Returns the port number of client-server communication.
         */
        int get_bport()                         const { return bport;}
        /**
         *Returns the port number used for inter-server communication.
         */
        int get_sport()                         const { return sport;}
        /**
         *Returns if the bbserv shall act as a daemon.
         */
        bool is_daemon()                        const { return isDaemon; }
        /**
         *Returns if the debug messages shall be displayed.
         */
        bool is_debug()                         const { return isDebug; }
        /**
         *Returns a reference to the registered peers.
         */
        const std::vector<Peer>& get_peers()    const { return peers; }

        /**
         *Set the path to the bulletin board file.
         */
        void set_bbfile(const std::string& bbfile)  { this->bbfile = bbfile; }
        /**
         *Set the number of used threads.
         */
        void set_Tmax(size_t Tmax)                  { this->Tmax = Tmax; }
        /**
         *Set the port number of client-server communication.
         */
        void set_bport(int bport)                   { this->bport = bport; }
        /**
         *Set the port number used for inter-server communication.
         */
        void set_sport(int sport)                   { this->sport = sport;}
        /**
         *Set if bbserv shall act as a daemon.
         */
        void set_daemon(bool on)                    { this->isDaemon = on; }
        /**
         *Set of debug messages shall be displayed.
         */
        void set_debug(bool on)                     { this->isDebug = on; }
        /**
         * Set a new peer definition including host-name and port number.
         *
         * Upon parsing error, this method may throw a BBServException.
         */
        void add_peer(std::string& peer);
        /**
         *Get the default timeout in ms for non-blocking network operations.
         */
        int get_network_timeout_ms()                { return 5000; }
};

/**
 *Print error messages.
 */
template<typename OriginT, typename... ArgsT>
void error_return(OriginT origin, ArgsT... args)
{
    std::stringstream sout;
    sout << "ERROR - [" << gettid() << "] " << typeid(origin).name() << ": ";
    (sout << ... << args) << ' ' << std::endl;
    throw BBServException(sout.str());
}

/**
 *Print timeout messages.
 */
template<typename OriginT, typename... ArgsT>
void timeout_return(OriginT origin, ArgsT... args)
{
    std::stringstream sout;
    sout << "ERROR - [" << gettid() << "] " << typeid(origin).name() << ": ";
    (sout << ... << args) << ' ' << std::endl;
    throw BBServTimeout(sout.str());
}

/**
 *Print debug messages.
 */
template<typename OriginT, typename... ArgsT>
void debug_print(OriginT origin, ArgsT... args)
{
    if (Config::singleton().is_debug())
    {
        std::stringstream sout;
        std::cout << "DBG   - [" << gettid() << "] " << typeid(origin).name() << ": ";
        (std::cout << ... << args) << ' ' << std::endl;
    }
}

/**
 *Helper function that converts ios_base state bits into names.
 */
inline std::string name_statebits(long bit)
{
    auto &goodbit =  std::ios_base::goodbit;
    auto &badbit =  std::ios_base::badbit;
    auto &eofbit =  std::ios_base::eofbit;
    auto &failbit =  std::ios_base::failbit;

    std::string result;

    if (goodbit == bit)
    {
        result = "good";
    }
    if (badbit & bit)
    {
        result += "|bad";
    }
    if (eofbit & bit)
    {
        result += "|eof";
    }
    if (failbit & bit)
    {
        result += "|fail";
    }

    return result;
}

