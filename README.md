# Socket Server

Creates a socket server on the configured port that will read from the specified device and serve to any connected client.

See also: [https://github.com/mckelvym/socket-client](https://github.com/mckelvym/socket-client)

## Build

```bash
$ make
g++ -I/usr/include -L/usr/lib -o server.o -c server.C
g++ -I/usr/include -L/usr/lib -o config.o -c config.C
g++ -lnsl -o server server.o config.o 
```

## Usage

```bash
$ ./server -?

Usage: server <options>
	[-b #]		 Buffer transfer size [4096].
	[-p #]		 Port to use [8800].
	[-q #]		 Queue size [2].
	[-s dev]	 Serial device to read from (must have proper permissions) [/dev/ttyS0].
	[-v]		 Turn on verbosity.
```

## Run

```bash
$ ./server -p 8801 -v -s /dev/random 
server: info: allocating socket.
server: info: socket is bound.
server: info: setting up listen queue.
server: info: waiting for connection.
server: info: got a connection.
server: info: opening serial device for reading.
server: info: finished reading data.
server: info: writing data to client.
server: info: wrote 4096 bytes.
```
