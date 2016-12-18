#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <pthread.h>
#include <vector>

#define PORT_NUM 9000
#define HTTP_HEADER "HTTP/1.1 200 OK\nDate: Fri, 31 Dec 1999 23:59:59 GMT\nContent-Type: text/html\nContent-Length: "

using namespace std;

struct server {
	struct sockaddr_in address;
	bool running;
};

vector<string> servers;
pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER;


/*
 * Read input file and store server addresses
 */
void readInputFile() {
	string line;
	ifstream inputfile("servers.txt");

	if (!inputfile.is_open()) return;

	while (getline(inputfile, line)) {
		servers.push_back(line);
		//cout << line << endl;
	}

}

vector<server*> getServerAddress() {
	vector<server*> server_state;

	for (int i = 0; i < servers.size(); i++) {
		string ip_port = servers.at(i);
		int delimiter_pos = ip_port.find(":");
		string ip = ip_port.substr(0, delimiter_pos);
		string port = ip_port.substr(delimiter_pos + 1, ip_port.length());

		cout << ip << ":" << port << endl;

		server* s = (server*) malloc(sizeof(server));
		inet_aton(ip.c_str(), &(s->address.sin_addr));
		s->address.sin_port = htons(atoi(port.c_str()));
		s->address.sin_family = AF_INET;
		s->running = false;
		
		server_state.push_back(s);
	}

	return server_state;
}


void* responseToAdminConsole(void* arg) {
	vector<server*>* server_state = (vector<server*>*) arg;
	int size = servers.size();

	// create socket for listening
	int sockfd = socket(PF_INET, SOCK_STREAM, 0);

	// config server socket
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htons(INADDR_ANY);
	servaddr.sin_port = htons(9001);

	bind(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
	listen(sockfd, 5);

	while (true) {
		// accept a connection from client
		struct sockaddr_in clientaddr;
		socklen_t clientaddrlen = sizeof(clientaddr);
		string up, down;

		int fd = accept(sockfd, (struct sockaddr*) &clientaddr, &clientaddrlen);
		if (fd < 0) continue;
		
		for (int i = 0; i < size; i++) {
			if (server_state->at(i)->running) up += servers.at(i) + ";";
			else down += servers.at(i) + ";";
		}

		cout << "alive servers: " << up << endl;
		cout << "down servers: " << down << endl;

		string response = up + "|" + down;
		write(fd, response.c_str(), response.length());
		close(fd);
	}

	return NULL;
}


void* checkServerState(void* arg) {
	vector<server*>* server_state = (vector<server*>*) arg;
	int size = server_state->size();
	int sockfd;
	struct sockaddr_in servaddr;

	while (true) {
		for (int i = 0; i < size; i++) {
			sockfd = socket(PF_INET, SOCK_STREAM, 0);
			servaddr = server_state->at(i)->address;
			if (connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) == 0) {
				pthread_mutex_lock(&mutex_lock);
				server_state->at(i)->running = true;
				pthread_mutex_unlock(&mutex_lock);

				cout << "server#" << i << " is alive" << endl;
			} else {
				pthread_mutex_lock(&mutex_lock);
				server_state->at(i)->running = false;
				pthread_mutex_unlock(&mutex_lock);

				cout << "server#" << i << " is down" << endl;
			}
			close(sockfd);
		}

		sleep(5);
	}

	return NULL;
}


/*
 * Main
 * Round Robin
 */
int main(int argc, char *argv[]) {
	int port_number = PORT_NUM;
	string html_part1 = "<!DOCTYPE html><html><head><title>Redirect</title><link rel=\"icon\" href=\"data:;base64,=\"><meta http-equiv=\"refresh\" content=\"0; URL='http://";
	string html_part2 = "'\"/></head></html>";

	// create socket for listening
	int listen_fd = socket(PF_INET, SOCK_STREAM, 0);

	// config server socket
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htons(INADDR_ANY);
	servaddr.sin_port = htons(port_number);

	// bind
	int ret_val = bind(listen_fd, (struct sockaddr*) &servaddr, sizeof(servaddr)); // bind
	ret_val = listen(listen_fd, 100); // start listening
	cout << "listening on " << to_string(PORT_NUM) << endl;

	readInputFile();
	vector<server*> server_state = getServerAddress();

	pthread_t thread1, thread2;
	pthread_create(&thread1, NULL, checkServerState, &server_state); //keep checking each server's state
	pthread_create(&thread2, NULL, responseToAdminConsole, &server_state);

	int size = servers.size();
	int index = 0;
	char buf[2048];
	// listening
	while (true) {
		// accept a connection from client
		struct sockaddr_in clientaddr;
		socklen_t clientaddrlen = sizeof(clientaddr);

		int fd = accept(listen_fd, (struct sockaddr*) &clientaddr, &clientaddrlen);
		if (fd < 0) continue;
		/*
		char* temp_ip = inet_ntoa(clientaddr.sin_addr);
		int temp_port = ntohs(clientaddr.sin_port);
		printf("incoming connection is from %s:%d\n", temp_ip, temp_port);
		//read(fd, buf, sizeof(buf));

		if (strcmp("127.0.0.1", temp_ip) == 0 && temp_port == 10000) {
			responseToAdminConsole(servers, server_state, fd);
			close(fd);
			continue;
		}
		*/
		pthread_mutex_lock(&mutex_lock);
		while (!server_state.at(index)->running) index = (index + 1) % size;
		pthread_mutex_unlock(&mutex_lock);

		cout << "Accept a new connection, redirect to server#" << to_string(index) << endl;

		//read(fd, buf, sizeof(buf));
		//string s(buf);
		//cout << s << endl;

		string content = html_part1 + servers.at(index) + html_part2;

		string response = HTTP_HEADER + to_string(content.length()) + "\n\n";
		response += content;
		//cout << response << endl;
		write(fd, response.c_str(), response.length());

		index = (index + 1) % size;
		close(fd);
	}

	close(listen_fd);

	return 0;
}
