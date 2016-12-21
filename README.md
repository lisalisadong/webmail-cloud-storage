# How to run:

## Compile
$ cd /T04
$ make

## Run backend master
$ cd /T04/backend/storage_query
$ ./master

## Run backend slaves
$ cd /T04/backend/storage_query
$ ./storage_server 50051

## Run frontend loadbalancer
$ cd /T04/frontend/server
$ ./load_balancer

## Run frontend servers
$ cd /T04
$ ./frontend/server/server -p 8101

## Run admin console
$ cd /T04/frontend/server
$ ./admin

# Visit website
- visit localhost:9000 for PennCloud
- visit localhost:10000 for admin console
