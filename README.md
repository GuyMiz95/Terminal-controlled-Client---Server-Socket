# Terminal controlled Client - Server Socket
  A basic and efficient 2-way socket between clients and a server, capable of handling and executing data sent by    clients, via terminal.

### =      File description     =

  socket.cpp - Only file, containing both server and client functionality (both devices can decide their roll upon     initiation of process.

### =  Design & Implementation  =

  It was decided to implement all functionality under a single file, that's to be able to unify all shared code      between both processes (client and server entries).
  The file contains 2 main processes, the server - side process which receives orders continuously and operates them.
  and the client - side process, which upon connection gives the user the ability to send orders to the server.

### =           Tests           =

The following are tests which were made to proove the validity of our code:

  Manual run-time test to check continuous connectivity.
  Manual run-time test to check commands sent by client being operated at server.
  Manual run-time test to check connectivity between server and 2 different clients using 2 different ports, with orders being operated     at server simultaniously.
