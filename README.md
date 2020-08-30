# BBServ

## Bulletin Board Messages and Distributed Agreement:
## A CSC 590 Challenge

Part of the requirements for the M.Sc. capstone research project(CS590)

Bulletin Board Server
---------------------

### Usage
~~~~~

Usage: bbserv [arguments] [peer..]
  Peers must have the format 'host:port'. 0 or more are allowed after the
  arguments.

Arguments:
  -h Print this usage information.
  -b overrides (or sets) the file name bbfile according to its argument.
  -T overrides THMAX according to its argument.
  -p overrides the port number bp according to its argument.
  -s overrides the port number sp according to its argument.
  -f (with no argument) forces daemon behavior to false.
  -d (with no argument) forces debugging facilities to true.
  -q (with no argument) disables debug-prolonged I/O.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### Implementation and Dependencies
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This project is written in C++17 and provides a build script based non GNU Make.

It relies only on standard POSIX libraries and is developed and tested on
GNU/Linux with GCC9. So it might be working on other POSIX compliant platforms
as well.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
### Design Principles
~~~~~~~~~~~~~~~~~

The following design principles have been applied in the design of this project:

 * Object Oriented Programming (OOP)
 * Resource Acquisition Is Initialization (RAII)
 * Builder Pattern
 * Polymorphism
 * Error handling via Exceptions

OOP is used throughout the whole code. It is especially useful in the
implementation of Command Objects (e.g. CmdRead, CmdWrite) as it improves
readability and data encapsulation.

RAII is useful when dealing with system resources like sockets and file
descriptors. This way it is ensured that the resource allocation/deallocation is
bound to an objects life time, which is particularly valuable when making this
code exception safe.

C++ Exceptions are used to handle error conditions and error reporting.

The design of this server project is built on Command Objects representing a
task to be done by this server. This includes processing the commands received
from a network client such as USER, READ, WRITE etc. as specified in phase 1 of
the project specification. A second type of Command Objects are implemented as
Broadcast Commands. These are instantiated by other Command Objects, which need
to broadcast data to sibling bbserv instances in order to fulfill the data
replication requirements as specified in phase 2.

In order to deal with all types of Command Objects and Broadcast Objects, they
are designed polymorphic and with a common interface. The pool of preallocated
threads is able to process these polymorphic data structures in a concurrent and
transparent manner. In order to do so, raw data, e.g. coming from an incoming
network connection, is passed to a builder function, which encapsulates the
knowledge of how to create Command Objects.

After implementing and testing the undo feature. I realized it is a bit tricky 
because you need to provoke some exceptional situation at the point when PRECOMMIT 
has already been acknowledged. E.g. by deleting lines from the bbfile, which would 
let REPLACE fail or kill a bbserv instance before it replies SUCCESS to COMMIT, which you can do manually if you started it with '-d'.
I figured I should mention that.
