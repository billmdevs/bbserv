#include <iomanip>
#include <iostream>
#include <memory>
#include <unistd.h>
#include <pthread.h>
#include <string>
#include "Config.h"
#include "InConnection.h"
#include "ThreadPool.h"
#include "ConnectionQueue.h"
#include "AutoLock.h"

InConnection replicationConnection;
InConnection inConnection;

/**
 *Forward the given peer definition to the configuration.
 */
void add_peer(std::string peer)
{
    Config::singleton().add_peer(peer);
}

/**
 *Print the program's how-to-use to stdout.
 */
void print_usage() {
    std::cout << "bbserv - Bulletin Board Server" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: bbserv [arguments] [peer..]" << std::endl;
    std::cout << "  Peers must have the format 'host:port'. 0 or more are allowed after the" << std::endl;
    std::cout << "  arguments." << std::endl;
    std::cout << std::endl;
    std::cout << "Arguments:" << std::endl;
    std::cout << "  -h Print this usage information." << std::endl;
    std::cout << "  -b overrides (or sets) the file name bbfile according to its argument." << std::endl;
    std::cout << "  -T overrides THMAX according to its argument." << std::endl;
    std::cout << "  -p overrides the port number bp according to its argument." << std::endl;
    std::cout << "  -s overrides the port number sp according to its argument." << std::endl;
    std::cout << "  -f (with no argument) forces daemon behavior to false." << std::endl;
    std::cout << "  -d (with no argument) forces debugging facilities to true." << std::endl;
    std::cout << "  -q (with no argument) disables debug-prolonged I/O." << std::endl;
}

/**
 *The program's main entry point.
 */
int main(int argc, char *argv[])
{
    Config::singleton().read_config();

    const char* optionString { "hb:T:p:s:fdqc:" };
    char option { '\0' };
    auto rescan {false};

    while (-1 != (option = getopt(argc, argv, optionString))) {
        switch (option) {
            case 'h':
            case '?':
                print_usage();
                return 0;
            case 'b':
                Config::singleton().set_bbfile(optarg);
                break;
            case 'T':
                Config::singleton().set_Tmax(atoi(optarg));
                break;
            case 'p':
                Config::singleton().set_bport(atoi(optarg));
                break;
            case 's':
                Config::singleton().set_sport(atoi(optarg));
                break;
            case 'f':
                Config::singleton().set_daemon(false);
                break;
            case 'd':
                Config::singleton().set_debug(true);
                break;
            case 'q':
                Config::singleton().set_quick(true);
                break;
            case 'c':
                if (rescan)
                {
                    break;
                }
                Config::singleton().set_bbconf(optarg);
                Config::singleton().read_config();
                optind = 1;
                rescan = true;
                break;
            default:
                break;
        }
    }

    if (0 == Config::singleton().get_bbfile().size())
    {
        std::cout << "ERROR - bbfile is not set" << std::endl;
        std::cout << std::endl;
        print_usage();
        return 1;
    }
    debug_print(Config::singleton(), "bbfile is set to ",
            std::quoted(Config::singleton().get_bbfile()));

    try
    {
        for (; optind < argc; ++optind)
        {
            add_peer(argv[optind]);
        }

        // Startup of threadpool operating on 's-port'
        auto replicationQueue = std::make_shared<ConnectionQueue>();
        ThreadPool replicationAgents(1);
        replicationAgents.operate(replicationQueue);
        replicationConnection.operate(Config::singleton().get_sport(),
                replicationQueue, true);

        // Startup of threadpool operating on 'b-port'
        auto connectionQueue = std::make_shared<ConnectionQueue>();
        ThreadPool agents(Config::singleton().get_Tmax());
        agents.operate(connectionQueue);
        inConnection.operate(Config::singleton().get_bport(), connectionQueue,
                false);

    }
    catch (const BBServException& error)
    {
        std::cout << error.what() << std::endl;
    }

    std::cout << "Press ENTER to quit" << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    return 0;
}
