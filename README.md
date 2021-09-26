![cmake workflow](https://github.com/elhayra/tcp_server_client/actions/workflows/cmake.yml/badge.svg)

# TCP server client
A thin and simple C++ TCP client and server library with examples.

### Platforms Support
Currently, both linux and mac are supported

### Examples
Simple tcp server-client examples. They are optimized for simplicity and ease of use/read but not for performance. However, I believe tuning this code to suite your needs should be easy in most cases.
You can find code examples of both server and client under the 'examples' directory. Both the library and the examples are well documented.

### Server
The server is thread-safe, and can handle multiple clients at the same time, and remove dead clients resources automatically. 

## Quick start
build the examples and static library file:
```bash
$ cd tcp_server_client
$ ./build
```

run the server and client examples:
Navigate into the `build` folder and run in the terminal:
```bash
$ cd build
```
In terminal #1:
```bash
$ ./tcp_server
```
In terminal #2:
```bash
$ ./tcp_client
```

## Building 

This project is set to use CMake to build both the *client example* and the *server example*. In addition, CMake builds a static *library file* to hold the common code to both server and client.

In order to build the project you can either use the `build.sh` script:
```bash
$ cd tcp_server_client
$ ./build
```

or build it manually:
```bash
$ cd tcp_server_client
$ mkdir build
$ cmake ..
$ make
```

The build process generate three files: `libtcp_client_server.a`, `tcp_client` and `tcp_server`.
The last two are the executables of the examples which can be executed in the terminal. 


#### Building Only Server or Client

By default, the CMake configuration builds both server and client. However, you can use flags to build only one of the apps as follows:

###### Disabling the server build

To build only the client, disable the server build by replace the `cmake ..` call by:

```bash
cmake -DSERVER_EXAMPLE=OFF ..
```
And then run the make command as usual.

###### Disabling the client build

To build only the server, disable the client build by replace the `cmake ..` call by:

```bash
cmake -DCLIENT_EXAMPLE=OFF ..
```
And then run the make command as usual.

## Run the Examples 
Navigate into the `build` folder and run in the terminal:
```bash
$ ./tcp_server
```

You should see a menu output on the screen, we'll get back to that.
In a different terminal, run the client:
````bash
$ ./tcp_client
````

You should see a similar menu for the client too. In addition, as mentioned, the client will try to connect to the server right away, so you should also see an output messages on both client and server terminals indicating that the connection succeeded.
Now, feel free to play with each of the client/server menus. You can exchange messages b/w client and server, print active clients etc. You can also spawn more clients in other terminals to see how the server handles multiple clients.

## Server-Client API

After playing with the examples, go into the examples source code and have a look at the `main()` function to learn how the server and client interacts. The examples are heavily documented.
In addition, you can also look at the public functions in `tcp_client.h` and `tcp_server.h` to learn what APIs are available.

### Server and Client Events 
Both server and client are using the observer design pattern to register and handle events.
When registering to an event with a callback, you should make sure that:
- The callback is fast (not doing any heavy lifting tasks) because those callbacks are called from the context of the server or client. 
- No server / client function calls are made in those callbacks to avoid possible deadlock.
