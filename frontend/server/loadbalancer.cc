#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <vector>

#define PORT_NUM 10000
#define HTTP_HEADER "HTTP/1.1 200 OK\nDate: Fri, 31 Dec 2016 23:59:59 GMT\nContent-Type: text/html\nContent-Length: "

using namespace std;


/*
 * Read input file and store server addresses
 */
vector<string> readInputFile() {
	string line;
	vector<string> servers;
	ifstream inputfile("servers.txt");

	if (!inputfile.is_open()) return servers;

	while (getline(inputfile, line)) {
		servers.push_back(line);
		cout << line << endl;
	}

	return servers;
}


/*
 * Write response to client
 */
bool doWrite(int fd, const char* buf, int len) {
	int sent = 0;
	while (sent < len) {
		int n = write(fd, &buf[sent], len - sent);
		if (n < 0) return false;
		sent += n;
	}
	return true;
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

	vector<string> servers = readInputFile();
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
		cout << "Accept a new connection, redirect to server#" << to_string(index) << endl;

		read(fd, buf, sizeof(buf));
		string s(buf);
		cout << s << endl; //request

		string content = html_part1 + servers.at(index++%size) + html_part2;

		string response = HTTP_HEADER + to_string(content.length()) + "\n\n";
		response += content;

		cout << response << endl; //respinse

		doWrite(fd, response.c_str(), response.length());

		close(fd);
	}

	close(listen_fd);
	return 0;
}
