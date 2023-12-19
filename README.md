# Serveur Redis

This project is a Red-is server implemented in C. It provides a set of basic Redis commands such as PING, SET, GET, DEL, and others. The purpose of this server is to serve as an educational example for understanding the internal workings of a server communicating with Redis.

## Installation

1.Clone this repository to your local system using the following command :

```shell
git clone https://github.com/RoKaTG/redis-server.git
```
Navigate to the project directory:

```shell

cd ~/redis-server
```
Compile the server using the make command:

```shell
make
```
Usage

   Start the server by specifying a port number (e.g., 8080):

```shell

./server 8080
```
The Red-is server is now running on the specified port.

To use the Redis command-line client, open a new terminal window and use the redis-cli command, specifying the same port number as the server:

```shell

redis-cli -p 8080
```
You can now use the supported Redis commands. For example, to get help on available commands, type:

```shell

HELPER
```
   This will display a list of supported commands with their descriptions (unfortunately, the display is buggy in the client but appears perfectly on the server prompt).

Supported Commands

    PING: Tests server connectivity.
    SET key value: Sets the value of a key.
    GET key: Retrieves the value of a key.
    DEL key [key ...]: Deletes one or more keys.
    EXISTS key [key ...]: Checks if one or more keys exist.
    APPEND key value: Appends a value to the end of a key.
    RANDOMKEY: Returns a random key.
    EXPIRE key seconds: Sets an expiration time on a key (in seconds).
    PEXPIRE key milliseconds: Sets an expiration time on a key (in milliseconds).
    PERSIST key: Removes the expiration time of a key.
    TTL key: Gets the remaining time until a key's expiration (in seconds).
    PTTL key: Gets the remaining time until a key's expiration (in milliseconds).
    KEYS pattern: Finds all keys matching the given pattern.
    RENAME key newkey: Renames a key.
    COPY source destination: Copies the value of a key to another key.
    INCR key: Increments the numeric value of a key.
    DECR key: Decrements the numeric value of a key.
    INCRBY key increment: Increments the numeric value of a key by a given amount.
    DECRBY key decrement: Decrements the numeric value of a key by a given amount.

In terms of functionality, I have completed the BASE and INTERMEDIATE parts of the project (except for concurrent requests where I did not have time to integrate mutexes and deadlocks in each implemented functionality), and I have not been able to work on the ADVANCED part of the project.

© MSILINI Yassine
