1) pre-requisite:
```sh
$ [sudo] apt-get install build-essential autoconf libtool
```

2) install grpc: 
```sh
 $ git clone -b $(curl -L http://grpc.io/release) https://github.com/grpc/grpc
 $ cd grpc
 $ git submodule update --init
 $ make
 $ [sudo] make install
```

3) install protobuf 3.0
```sh
$ cd third_party/protobuf
$ make
$ sudo make install
```

4) install pkg-config
```sh
$ sudo apt-get install pkg-config
```

5) [optional] build an example
```sh
$ cd examples/cpp/helloworld/
$ make
$ ./greeter_server
# in another terminal
$ ./greeter_cliend
# Greeter received: Hello world
$ 
```