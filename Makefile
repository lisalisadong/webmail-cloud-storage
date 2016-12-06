HTTP_SERVER_DIR = ./frontend/server/
STORAGE_DIR = ./backend/storage_query/
CXXFLAGS += -std=c++11
LDFLAGS += -L/usr/local/lib `pkg-config --libs grpc++ grpc`       \
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed \
           -lprotobuf -lpthread -ldl

<<<<<<< HEAD
all: storage_backend $(HTTP_SERVER_DIR)server
=======
all:  storage_backend $(HTTP_SERVER_DIR)server 
>>>>>>> 8bbd426201f6e6836da4f34b7fe49d324cc3c245


storage_backend:
	$(MAKE) -C $(STORAGE_DIR)

$(HTTP_SERVER_DIR)server: $(HTTP_SERVER_DIR)server.cc $(STORAGE_DIR)storage_client.h $(STORAGE_DIR)storage_query.pb.o $(STORAGE_DIR)storage_query.grpc.pb.o
	g++ $(CXXFLAGS) $^ $(LDFLAGS) -o $@ -g -O0 

pack:
	rm -f submit-hw2.zip
	zip -r submit-hw2.zip *.cc README Makefile

clean:
	rm -fv $(HTTP_SERVER_DIR)server
	$(MAKE) -C $(STORAGE_DIR) clean
