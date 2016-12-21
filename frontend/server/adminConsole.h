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


void renderDataStoragePage(int fd, int& node, int& page) {
	string HOMEPAGE_BEGIN = "<!DOCTYPE html><html><head><title>DataStorage</title></head><link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\"><body><div style=\"margin-left: 20px; margin-right: 20px\">";
	string HOMEPAGE_END = "<a href=\"http://localhost:10000/prev\" class=\"btn btn-success\" role=\"button\" style=\"margin-right: 50px\">prev</a><a href=\"http://localhost:10000/next\" class=\"btn btn-success\" role=\"button\">next</a><br><br><a href=\"http://localhost:10000\" class=\"btn btn-default\" role=\"button\">Homepage</a></div></body></html>";
	vector<string> result;
	map<string, map<string, string> > data;
	string content, response;

	map<string, vector<string> > backendServers = getBackendServerState();
	vector<string> upBackendServer = backendServers["up"];

	//if there is no backend server up, return no data
	int size = upBackendServer.size();
	cout << "There are " + to_string(size) + " backend servers alive" << endl;
	if (size == 0) {
		content = HOMEPAGE_BEGIN + "<h3>Data Storage</h3><h4>no data!</h4>" + HOMEPAGE_END;
		response = HTTP_HEADER + to_string(content.length()) + "\n\n";
		response += content;
		write(fd, response.c_str(), response.length());
		return;
	}

	if (node < 0) node = 0;
	if (node >= size) node = size - 1;
	string backend_server = upBackendServer.at(node);
	cout << "get data from backend node " + backend_server << endl;

	StorageClient client(grpc::CreateChannel(backend_server, grpc::InsecureChannelCredentials()));
	int returnSize = client.GetData(page*10, 10, data);
	cout << "Return size is " + to_string(returnSize) << endl;

	//if there is no data in current node, move to next node and get data again
	if (returnSize == 0) {
		node++;
		if (node >= size) {
			node = size - 1;
		} else page = 0;
		cout << "backend node " + backend_server + "has no data" << endl;
		backend_server = upBackendServer.at(node);
		cout << "get data from backend node " + backend_server << endl;

		StorageClient clientNew(grpc::CreateChannel(backend_server, grpc::InsecureChannelCredentials()));
		returnSize = clientNew.GetData(page*10, 10, data);
		cout << "Return size is " + to_string(returnSize) << endl;
	}

	for (auto it1 = data.begin(); it1 != data.end(); it1++) {
		string row = it1->first;
		map<string, string> value = it1->second;
		for (auto it2 = value.begin(); it2 != value.end(); it2++) {
			result.push_back("<td class=\"col-md-2\">" + row + "</td><td class=\"col-md-2\">" + it2->first + "</td><td>" + it2->second + "</td>");
		}
	}

	//generate html
	content = HOMEPAGE_BEGIN + "<h3>Data Storage</h3><h4>node#" + backend_server + "</h4><table class=\"table table-sm table-bordered\"><thead><tr><th class=\"col-md-2\">Row</th><th class=\"col-md-2\">Col</th><th>Value</th></tr></thead><tbody>";
	for (int i = 0; i < result.size(); i++) {
		content += "<tr>" + result.at(i) + "</tr>";
	}
	content += "</tbody></table>" + HOMEPAGE_END;

	response = HTTP_HEADER + to_string(content.length()) + "\n\n";
	response += content;
	write(fd, response.c_str(), response.length());
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
	string HOMEPAGE_BEGIN = "<!DOCTYPE html><html><head><title>AdminConsole</title><link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\"></head><body><div style=\"margin-left: 20px; margin-right: 20px\">";
	string HOMEPAGE_END = "<br><br><a href=\"http://localhost:10000/data\" class=\"btn btn-primary\" role=\"button\">Data</a></div></body></html>";

	map<string, vector<string> > frontendServerState = getFrontendServerState();
	vector<string> front_up = frontendServerState["up"];
	vector<string> front_down = frontendServerState["down"];

	map<string, vector<string> > backendServerState = getBackendServerState();
	vector<string> back_up = backendServerState["up"];
	vector<string> back_down = backendServerState["down"];

	//generate html file
	string content = HOMEPAGE_BEGIN + "<h3>Frontend Server</h3><h4>Alive:</h4><ul class=\"list-group\">";
	for (int i = 0; i < front_up.size(); i++) {
		content += "<li class=\"list-group-item list-group-item-success\">" + front_up.at(i) + "</li>";
	}
	content += "</ul><h4>Down:</h4><ul class=\"list-group\">";
	for (int i = 0; i < front_down.size(); i++) {
		content += "<li class=\"list-group-item list-group-item-danger\">" + front_down.at(i) + "</li>";
	}
	content += "</ul>";

	content += "<h3>Backend Server</h3><h4>Alive:</h4><ul class=\"list-group\">";
	for (int i = 0; i < back_up.size(); i++) {
		content += "<li class=\"list-group-item list-group-item-success\">" + back_up.at(i) + "</li>";
	}
	content += "</ul><h4>Down:</h4><ul class=\"list-group\">";
	for (int i = 0; i < back_down.size(); i++) {
		content += "<li class=\"list-group-item list-group-item-danger\">" + back_down.at(i) + "</li>";
	}
	content += "</ul>" + HOMEPAGE_END;

	string response = HTTP_HEADER + to_string(content.length()) + "\n\n";
	response += content;
	write(fd, response.c_str(), response.length());
}


