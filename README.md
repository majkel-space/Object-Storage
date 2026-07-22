# Object-Storage


## TO RUN PROGRAM

Program is build with CMake file system, to run it:
1. mkdir build
2. cd build
3. cmake ..
4. cmake --build .
5. ./server/server
6. .client/client (in another terminal window)
    alternatively instead runing server:
    curl http://127.0.0.1:1234/
    or:
    nc 127.0.0.1 1234
    GET / HTTP/1.1
    Host: localhost

## TO TEST

Complete steps up to 5 from TO RUN chapter
ctest --verbose

Program has implemented SIGINT interuption.
To stop hit 'Ctrl+C' in both terminal windows.

---

## Server description

Main in server creates io_service, connect it with SIGINT and in async_wait waits for potential interuption signal.

Server holds its own TCP IP acceptor.
When constructed creates a shared_ptr<tcpip::socket> with acceptor as an argument.
shared_ptr as its counter is incremented when passing it to ConnectionHandler and it has to stay alive during entire connection.
Acceptor starts async_accept with labmda trigering ConnectionHandler.
ConectionHandler in constant loop calls DoRead, DoWrite with use of async_read_some and async_write_some.
In here we are creating another incrementation of socket shared_ptr.

## Client desciption

Main in client creates io_service, connect it with SIGINT and in async_wait waits for potential interuption signal.
Construct 5 clients binded to the same io_sevice and pass them io_service to create a socket instance and connect to server (every with same port).
In send method in main, start to send msgs and wait for response from server.

## Protocol parsers

There are 2 protocols HTTP and RESP, both classes responible for parsing
