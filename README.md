# TCP server client
A thin and simple C++ TCP client server

This code was written to run on linux machines, and to compile with C++11 and older versions of C++ (this is why I chose to use pthread instead of std::thread).

Both server and client are very easy and simple to use. You can find code examples of both server and client. 

The code is documented, so I hope you find it easy to change it to suit your needs if needed to.

The server class supports multiple clients.

## Building both server and client

This project is set to use CMake to build both client example and server example.

To build:

```bash
$ git clone https://github.com/elhayra/tcp_server_client.git
$ cd tcp_server_client
$ mkdir build
$ cmake ..
$ make
```
The commands above generate two files: `tcp_client` and `tcp_server`.

To run, open a terminal, move to the `build` folder and execute:
```
./tcp_server
```
The server will print out something like:
```bash
Server setup succeeded
```
In another terminal, move to `build` folder again and execute the client by:
```
./tcp_client
```
The client will output:
```bash
Client connected successfully
Got msg from server: server got this msg: hello server

Got msg from server: server got this msg: hello server

Got msg from server: server got this msg: hello server
```
In the server terminal you would see some messages like:
```bash
Server setup succeeded
Got client with IP: 127.0.0.1
-----------------
IP address: 127.0.0.1
Connected?: True
Socket FD: 4
Message: 
Observer1 got client msg: hello server

Observer1 got client msg: hello server

Observer1 got client msg: hello server
```

Press control+c in both terminals to stop the server and client apps.

## Building only server or client

By default, the CMake configuration builds both server and client. However, ou can use flags if you want to build only one of the apps as follows:

### Disabling the server build

To build only the client, disable the server build by replace the `cmake ..` call by:

```bash
cmake -DSERVER_EXAMPLE=OFF ..
```
And then run the make command as usual.

### Disabling the client build

To build only the server, disable the client build by replace the `cmake ..` call by:

```bash
cmake -DCLIENT_EXAMPLE=OFF ..
```
And then run the make command as usual.
