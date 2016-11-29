TARGETS = ./frontend/server/server

all: $(TARGETS)

./frontend/server/server: ./frontend/server/server.cc
	g++ $^ -lpthread -o $@ -g -O0 

pack:
	rm -f submit-hw2.zip
	zip -r submit-hw2.zip *.cc README Makefile

clean::
	rm -fv $(TARGETS)
