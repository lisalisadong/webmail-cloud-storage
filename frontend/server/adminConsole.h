#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>
#include <map>
#include <vector>
#include <grpc++/grpc++.h>
#include "../../backend/storage_query/master_client.h"
#include "../../backend/storage_query/storage_client.h"

using namespace std;

#define HTTP_HEADER "HTTP/1.1 200 OK\nDate: Fri, 31 Dec 1999 23:59:59 GMT\nContent-Type: text/html\nContent-Length: "


void renderDataStoragePage(int fd, vector<string>& upBackendServer, int& node, int& page) {
	string HOMEPAGE_BEGIN = "<!DOCTYPE html><html><head><title>DataStorage</title></head><body>";
	string HOMEPAGE_END = "<a href=\"http://localhost:10000/prev\">prev</a>    <a href=\"http://localhost:10000/next\">next</a><br><a href=\"http://localhost:10000\">Homepage</a></body></html>";
	vector<string> result;
	map<string, map<string, string> > data;

	int size = upBackendServer.size();
	cout << "There are " + to_string(size) + " backend servers alive" << endl;

	if (node < 0) node = 0;
	if (node >= size) node = size - 1;
	string backend_server = upBackendServer.at(node);
	cout << "get data from backend node " + backend_server << endl;

	StorageClient client(grpc::CreateChannel(backend_server, grpc::InsecureChannelCredentials()));
	int returnSize = client.GetData(page*10, 10, data);
	cout << "Return size is " + to_string(returnSize) << endl;

	if (returnSize < 10) {
		node++;
		page = 0;
	}

	for (auto it1 = data.begin(); it1 != data.end(); it1++) {
		string row = it1->first;
		map<string, string> value = it1->second;
		for (auto it2 = value.begin(); it2 != value.end(); it2++) {
			result.push_back(row + ":" + it2->first + "\t" + it2->second);
		}
	}

	//generate html
	string content = HOMEPAGE_BEGIN + "<h3>Data Storage</h3><h5>node#" + backend_server + "</h5><ol>";
	for (int i = 0; i < result.size(); i++) {
		content += "<li>" + result.at(i) + "</li>";
	}
	content += "</ol>"+ HOMEPAGE_END;

	string response = HTTP_HEADER + to_string(content.length()) + "\n\n";
	response += content;
	write(fd, response.c_str(), response.length());
}


/*
 * get the info about backend servers
 */

map<string, vector<string>> getBackendServerState() {
	MasterClient master(grpc::CreateChannel("127.0.0.1:8000", grpc::InsecureChannelCredentials()));
	vector<string> upServers;
	vector<string> downServers;
	map<string, vector<string>> result;

	master.GetAllNodes(upServers, downServers);
	result["up"] = upServers;
	result["down"] = downServers;

	return result;
}


/*
 * get the info about frontend servers
 */
map<string, vector<string> > getFrontendServerState() {
	size_t pos = 0;
	struct sockaddr_in servaddr;
	map<string, vector<string> > frontend;
	vector<string> up;
	vector<string> down;
	string up_server, down_server;

	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(9001);
	inet_aton("127.0.0.1", &(servaddr.sin_addr));

	int ret_val = connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
	if (ret_val == 0) printf("Connect complete!\n");
	else printf("Connect failed!\n");

	char response[1024];
	read(sockfd, (char*) &response, sizeof(response));
	printf("response: %s\n", response);
	close(sockfd);

	string server_state = string(response);
	int index = server_state.find("|");
	if (index == 0) up_server = "";
	else up_server = server_state.substr(0, index);
	if (index == server_state.length() - 1) down_server = "";
	else down_server = server_state.substr(index + 1);

	cout << "up front servers: " + up_server << endl;
	cout << "down front servers: " + down_server << endl;

	while ((pos = up_server.find(";")) != string::npos) {
		up.push_back(up_server.substr(0, pos));
		up_server.erase(0, pos + 1);
	}
	frontend["up"] = up;

	while ((pos = down_server.find(";")) != string::npos) {
		down.push_back(down_server.substr(0, pos));
		down_server.erase(0, pos + 1);
	}
	frontend["down"] = down;

	return frontend;
}

void renderAdminConsoleHomepage(int fd) {
	string HOMEPAGE_BEGIN = "<!DOCTYPE html><html><head><title>AdminConsole</title></head><body>";
	string HOMEPAGE_END = "<a href=\"http://localhost:10000/data\">Data</a></body></html>";

	map<string, vector<string> > frontendServerState = getFrontendServerState();
	vector<string> front_up = frontendServerState["up"];
	vector<string> front_down = frontendServerState["down"];

	map<string, vector<string> > backendServerState = getBackendServerState();
	vector<string> back_up = backendServerState["up"];
	vector<string> back_down = backendServerState["down"];

	//generate html file
	string content = HOMEPAGE_BEGIN + "<h3>Frontend Server</h3><h5>Alive:</h5><ul>";
	for (int i = 0; i < front_up.size(); i++) {
		content += "<li>" + front_up.at(i) + "</li>";
	}
	content += "</ul><h5>Down:</h5><ul>";
	for (int i = 0; i < front_down.size(); i++) {
		content += "<li>" + front_down.at(i) + "</li>";
	}
	content += "</ul>";

	content += "<h3>Backend Server</h3><h5>Alive:</h5><ul>";
	for (int i = 0; i < back_up.size(); i++) {
		content += "<li>" + back_up.at(i) + "</li>";
	}
	content += "</ul><h5>Down:</h5><ul>";
	for (int i = 0; i < back_down.size(); i++) {
		content += "<li>" + back_down.at(i) + "</li>";
	}
	content += "</ul>" + HOMEPAGE_END;

	string response = HTTP_HEADER + to_string(content.length()) + "\n\n";
	response += content;
	write(fd, response.c_str(), response.length());
}


