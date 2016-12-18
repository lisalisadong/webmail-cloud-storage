HTTP_SERVER_DIR = ./frontend/server/
STORAGE_DIR = ./backend/storage_query/
CXXFLAGS += -std=c++11
LDFLAGS += -L/usr/local/lib `pkg-config --libs grpc++ grpc`       \
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed \
           -lprotobuf -lpthread -ldl -lcrypto

all: storage_backend $(HTTP_SERVER_DIR)server $(HTTP_SERVER_DIR)load_balancer $(HTTP_SERVER_DIR)admin

storage_backend:
	$(MAKE) -C $(STORAGE_DIR)

$(HTTP_SERVER_DIR)server: $(STORAGE_DIR)storage_query.pb.o $(STORAGE_DIR)storage_query.grpc.pb.o $(STORAGE_DIR)logger.o $(STORAGE_DIR)utils.o $(STORAGE_DIR)file_system.o $(STORAGE_DIR)cache.o $(STORAGE_DIR)storage_client.o $(STORAGE_DIR)master_client.o $(HTTP_SERVER_DIR)server.cc 
	g++ $(CXXFLAGS) $^ $(LDFLAGS) -o $@ -g -O0 

$(HTTP_SERVER_DIR)load_balancer: $(HTTP_SERVER_DIR)loadbalancer.cc 
	g++ $(CXXFLAGS) $^ $(LDFLAGS) -o $@ -g -O0 

$(HTTP_SERVER_DIR)admin: $(STORAGE_DIR)storage_query.pb.o $(STORAGE_DIR)storage_query.grpc.pb.o $(STORAGE_DIR)logger.o $(STORAGE_DIR)utils.o $(STORAGE_DIR)file_system.o $(STORAGE_DIR)cache.o $(STORAGE_DIR)storage_client.o $(STORAGE_DIR)master_client.o $(HTTP_SERVER_DIR)adminConsole.h $(HTTP_SERVER_DIR)adminConsole.cc 
	g++ $(CXXFLAGS) $^ $(LDFLAGS) -o $@ -g -O0 

pack:
	rm -f submit-hw2.zip
	zip -r submit-hw2.zip *.cc README Makefile

clean:
	rm -fv $(HTTP_SERVER_DIR)server $(HTTP_SERVER_DIR)load_balancer $(HTTP_SERVER_DIR)admin
	$(MAKE) -C $(STORAGE_DIR) clean
