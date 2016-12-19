HTTP_SERVER_DIR = ./frontend/server/
STORAGE_DIR = ./backend/storage_query/
CXXFLAGS += -std=c++11
LDFLAGS += -L/usr/local/lib `pkg-config --libs grpc++ grpc`       \
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed \
           -lprotobuf -lpthread -ldl -lcrypto -lresolv

all: storage_backend $(HTTP_SERVER_DIR)server $(HTTP_SERVER_DIR)forward

storage_backend:
	$(MAKE) -C $(STORAGE_DIR)

$(HTTP_SERVER_DIR)server: $(STORAGE_DIR)storage_query.pb.o $(STORAGE_DIR)storage_query.grpc.pb.o $(STORAGE_DIR)logger.o $(STORAGE_DIR)utils.o $(STORAGE_DIR)file_system.o $(STORAGE_DIR)cache.o $(STORAGE_DIR)storage_client.o $(STORAGE_DIR)master_client.o $(HTTP_SERVER_DIR)server.cc 
	g++ $(CXXFLAGS) $^ $(LDFLAGS) -o $@ -g -O0

$(HTTP_SERVER_DIR)forward: $(STORAGE_DIR)storage_query.pb.o $(STORAGE_DIR)storage_query.grpc.pb.o $(STORAGE_DIR)logger.o $(STORAGE_DIR)utils.o $(STORAGE_DIR)file_system.o $(STORAGE_DIR)cache.o $(STORAGE_DIR)storage_client.o $(STORAGE_DIR)master_client.o $(HTTP_SERVER_DIR)forward.cc
	g++ $(CXXFLAGS) $^ $(LDFLAGS) -o $@ -g -O0

clean:
	rm -fv $(HTTP_SERVER_DIR)server
	rm -fv $(HTTP_SERVER_DIR)forward
	$(MAKE) -C $(STORAGE_DIR) clean
