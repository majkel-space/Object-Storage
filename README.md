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

Program has implemented SIGINT interuption.
To stop hit 'Ctrl+C' in both terminal windows.


## TO TEST

Complete steps up to 5 from TO RUN chapter
ctest --verbose

Test tries to cover diffrenc scenarios:
1. Test_HttpPut
    covers PUT method from HTTP, data is send as a one stream, or as a chunks
2. Test_RespChunkPutAndPutX
    covers Resp SET and SETXN, if file with SETXN was not created recieve an error msg with information about
    not created object
3. Test_HttpAndRespGet_NoObject
    covers GET when object does not exist in Storage, recieve msg object do not exist.
4. Recieve list of objects in storage from HTTP List or KEYS * from RESP
5. Test_HttpAndRespCrossGet
    covers HTTP and RESP PUT/SET and after creation GET trhough other protocol
6. Test_HttpAndRespPutBigObjectToStorage
    covers 2 functionalities, PUT/SET of two big object to the storage with async send mechanism
    !! importent notice to make it work follow TODO instruction from test.

---

## Server description

Main in server creates io_service, connect it with SIGINT and in async_wait waits for potential interuption signal.

Server holds its own TCP IP acceptor.
When constructed creates a shared_ptr<tcpip::socket> with acceptor as an argument.
shared_ptr as its counter is incremented when passing it to ConnectionHandler and it has to stay alive during entire connection.
Acceptor starts async_accept with labmda trigering ConnectionHandler.
ConectionHandler in constant loop calls DoRead, DoWrite with use of async_read_some and async_write_some.
In here we are creating another incrementation of socket shared_ptr.


## ConnectionHandler

One ConnectionHandler serve one connection. So they can keep information about one request trough it entire handle of the request.
This approach prevent mixing read of header or transfer content, while handling concurent multiple cals.
Handle every each separate client, gets a shared_ptr to the storage manager from server.
Hold pointer to the IParser.
ConnectionHandler pass header of msg to Parser to retrieve basic information how to handle each connection.
Base on the MessageStatus decide if should continue reading transfer from socket, or send data to client.
Hold read and write buffer as array<char, 1024>
When it ends it sends
boost::asio::ip::tcp::socket::shutdown_receive
boost::asio::ip::tcp::socket::shutdown_send
base on the type of operation.
I decided to not keep connection still alive, but just wait for ncoming transmision, then CH is recreated, with clear buffer.


## Protocol parsers

Protocol parsers are inheriting from IParser, so approach is extensible for another protocols.
There are 2 protocols HTTP and RESP, both classes responible for parsing headers of incoming msgs.
Responsibility of parsers is to catch method/type of the request and path of the object, content lenght of the HTTP msg, content length of Resp argument,
and base on that read data creates a request.

Protocols are inheriting from IParser, which have common methods, called from ConnectionHandler.
This exact part was generated with help of GPT 5.3 Codex


## Request
Hold information parsed by protocol parser.
Contain:
Statuses with information about header parse, type of the request and status of content handling.
File handle: ifstream and ofstream
String Final response (it can be strin as final response is short - just status, succes, error with file path)


## StorageManager

One instace shared through multiple ConnectionHandlers. That is why it deos not hold any status of msg parsing,
it just transfer request reference so it can be updated while content is parsed inside Storage.
Detect operation to perfomr by Storage, detect it base on the Request, created out of msg header.
Base on detected operation and StorageStatusdirect the traffic between ConnectionHandler and Storage.
Transfer of data chunks are transfered by span<char> to not allocate data into memory.


## Storage

Called by StorageManager saves data chunks to files with providen path or read data from disk.
While reading each data chunk it recalculate request.content_size.
Base on actions like errors, end of operation save data or read data set request.msg_status,
base on which CH take the decision what should do, continue read/write, or send final response to the client.
When new object is being created in storage it is saved with ".tmp" suffix, so no new request can read from it.
After transfer is finalized sufix is being removed.
All Storage operations works on span<char> to not alocate aditional memory.


## Client desciption

Client contains Connect method to connect with Server.
Send method (predefined streams only) which is using boost::asio::write
or SendObject which is responsible for sending object with use of its path
Read method with use of boost::asio::read to read data, it is waiting for EOF msg from client (generated by shutdown)


## Error handling

Errors when occure are catched and saved to request final_response.
When error catched ConnectionHandler generate response to client and send EOF with shutdown recieve or write.
After that connection is closed


## Implementation

1. Client Server connection and msg transfer
2. Http Parser to read header
3. Resp parser to read bulks and common IParser.
4. StorageManager to detect method type
5. Storate method by method: PUT/SET, SETNX, GET / and KEYS *, GET.
6. Integration test (Uncle Bob wouldn't be happy with that approach)

With every step I tried to keep client server connection working, but at the end when I've started UT then errors where catched.
Mainly they were bad file descriptor, caused by connection close whem there were stilldata to send, or read in buffer, or incoming transfer.
That is why I've implemented shutdown_send or write first and after that when final response was send to client, than


## What I would do next

Small upgread of storage, like retray while fail of open file
Storage limitation - StorageManager would need to keep Storage size
    recalculate its size base on method PUT/SET and if implemented DELETE and request contetn size
    then there would be 2 options, reject new income, or keep queue with paths and delete oldest (till there would be enough space for new object)
Standarisation of errors with enum of error types
DELETE command
other bonus points described in objstore-task-v2.md


<image src = "../ObjectStorage_Server_v3.png" alt="Class diagram">
