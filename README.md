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

Program has implemented SIGINT interruption.
To stop hit 'Ctrl+C' in both terminal windows.


## TO TEST

Complete steps up to 5 from TO RUN chapter
ctest --verbose

Test tries to cover difference scenarios:
1. Test_HttpPut
    covers PUT method from HTTP, data is send as a one stream, or as a chunks
2. Test_RespChunkPutAndPutX
    covers Resp SET and SETXN, if file with SETXN was not created receive an error msg with information about
    not created object
3. Test_HttpAndRespGet_NoObject
    covers GET when object does not exist in Storage, receive msg object do not exist.
4. Recieve list of objects in storage from HTTP List or KEYS * from RESP
5. Test_HttpAndRespCrossGet
    covers HTTP and RESP PUT/SET and after creation GET trhough other protocol
6. Test_HttpAndRespPutBigObjectToStorage
    covers 2 functionalities, PUT/SET of two big object to the storage with async send mechanism
    !! important notice to make it work follow TODO instruction from test.

---

## Server description

Main in server creates io_service, connect it with SIGINT and in async_wait waits for potential interruption signal.

Server holds its own TCP IP acceptor.
When constructed creates a shared_ptr<tcpip::socket> with acceptor as an argument.
shared_ptr as its counter is incremented when passing it to ConnectionHandler and it has to stay alive during entire connection.
Acceptor starts async_accept with lambda triggering ConnectionHandler.
ConectionHandler in constant loop calls DoRead, DoWrite with use of async_read_some and async_write_some.
In here we are creating another incrementation of socket shared_ptr.


## ConnectionHandler

One ConnectionHandler serve one connection. So they can keep information about one request trough it entire handle of the request.
This approach prevent mixing read of header or transfer content, while handling concurrent multiple cals.
Handle every each separate client, gets a shared_ptr to the storage manager from server.
Hold pointer to the IParser.
ConnectionHandler pass header of msg to Parser to retrieve basic information how to handle each connection.
Base on the MessageStatus decide if should continue reading transfer from socket, or send data to client.
Hold read and write buffer as array<char, 1024>
When it ends it sends
boost::asio::ip::tcp::socket::shutdown_receive
boost::asio::ip::tcp::socket::shutdown_send
base on the type of operation.
I decided to not keep connection still alive, but just wait for oncoming transmission, then CH is recreated, with clear buffer.


## Protocol parsers

Protocol parsers are inheriting from IParser, so approach is extensible for another protocols.
There are 2 protocols HTTP and RESP, both classes responsible for parsing headers of incoming msgs.
Responsibility of parsers is to catch method/type of the request and path of the object, content length of the HTTP msg, content length of Resp argument,
and base on that read data creates a request.

Protocols are inheriting from IParser, which have common methods, called from ConnectionHandler.
This exact part was generated with help of GPT 5.3 Codex


## Request
Hold information parsed by protocol parser.
Contain:
Statuses with information about header parse, type of the request and status of content handling.
File handle: ifstream and ofstream
String Final response (it can be string as final response is short - just status, success, error with file path)


## StorageManager

One instace shared through multiple ConnectionHandlers. That is why it does not hold any status of msg parsing,
it just transfer request reference so it can be updated while content is parsed inside Storage.
Detect operation to perform by Storage, detect it base on the Request, created out of msg header.
Base on detected operation and StorageStatusdirect the traffic between ConnectionHandler and Storage.
Transfer of data chunks are transferred by span<char> to not allocate data into memory.


## Storage

Called by StorageManager saves data chunks to files with providen path or read data from disk.
While reading each data chunk it recalculate request.content_size.
Base on actions like errors, end of operation save data or read data set request.msg_status,
base on which CH take the decision what should do, continue read/write, or send final response to the client.
When new object is being created in storage it is saved with ".tmp" suffix, so no new request can read from it.
After transfer is finalized sufix is being removed.
All Storage operations works on span<char> to not allocate additional memory.


## Client desciption

Client contains Connect method to connect with Server.
Send method (predefined streams only) which is using boost::asio::write
or SendObject which is responsible for sending object with use of its path
Read method with use of boost::asio::read to read data, it is waiting for EOF msg from client (generated by shutdown)


## Error handling

Errors when occur are catched and saved to request final_response.
When error catched ConnectionHandler generate response to client and send EOF with shutdown receive or write.
After that connection is closed


## Class diagram
<img width="1460" height="1582" alt="ObjectStorage_Server_v3" src="https://github.com/user-attachments/assets/240f1ba2-8dda-4d82-934b-629c9ece8c39" />

