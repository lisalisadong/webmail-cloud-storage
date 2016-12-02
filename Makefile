HTTP_SERVER_DIR = ./frontend/server/
STORAGE_DIR = ./backend/storage_query


all: $(HTTP_SERVER_DIR)server
	$(MAKE) -C $(STORAGE_DIR)

$(HTTP_SERVER_DIR)server: $(HTTP_SERVER_DIR)/server.cc
	g++ $^ -lpthread -o $@ -g -O0 

pack:
	rm -f submit-hw2.zip
	zip -r submit-hw2.zip *.cc README Makefile

clean:
	rm -fv $(HTTP_SERVER_DIR)server
	$(MAKE) -C $(STORAGE_DIR) clean
