# Multithreaded distributed HTTP/1.1 server

This project is a prototype of a said technology, implementing the following features: 

1. Connection to the servers with IPs listed in `serverIpList.txt`.
2. Load balancing with the help of [nftables](https://wiki.archlinux.org/title/nftables).
3. Resourse sharing (If a requested resource isn't available on one subserver, it's fetched from another, if present at all).

# Build 

CMake was used as a building system. To build, use the following commands: 

```
cmake -DCMAKE_BUILD_TYPE=Release -S ./ -B ./build
cmake --build ./build
```

# Future improvements

The project is yet to be improved, introducing the complete HTTP/1.1 functionality, better code base and error handling. Working on it. 