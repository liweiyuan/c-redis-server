# C Redis Server

A simple Redis-like server implemented in C.

## Features

- Handles `SET` and `GET` commands.
- Multi-threaded to handle multiple client connections.
- Uses function pointers for command dispatching.

## Build Instructions

To build the project, navigate to the root directory and run:

```bash
mkdir build
cd build
cmake ..
make
```

## Run Instructions

After building, you can run the server from the `build` directory:

```bash
./c_redis_server
```

## Usage

You can connect to the server using `netcat` or any Redis client. For example:

```bash
netcat localhost 6379
```

Then, you can send Redis commands:

```
SET mykey myvalue
+OK
GET mykey
$7
myvalue
```
