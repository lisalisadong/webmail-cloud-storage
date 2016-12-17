#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <pthread.h>
#include <sstream>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <set>
#include <vector>
#include <dirent.h>
#include <map>
#include <ctime>
#include <sys/time.h>
#include <algorithm>
#include <fstream>
#include <grpc++/grpc++.h>
#include <iomanip>

#include "constants.h"
#include "helper.h"

using namespace std;

#ifndef HANDLEEMAILS_H_
#define HANDLEEMAILS_H_

string getEmailResponse(string user, const char* url) {
	string response = HTTP_HEADER;
	string urlStr = url + 1;
	int emailNameI = urlStr.find(' ');
	string emailName = urlStr.substr(0, emailNameI);

	StorageClient client(grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()));
	string email = client.Get(user, emailName);
	response += to_string(email.length()) + "\n";
	response += "\n";
	response += email;
	return response;
}


string generateLI(string num, string title, string url) {
	string res;
//	<li><a href="emails/0">aaaa</a></li>
	res = "<li><a href=\"" + url + "-" + num + "\">" + title + "</a></li>";
	return res;
}

string getEmailMid(string emails, string url) {
	string email_mid;
	int first = 0;
	while (first != string::npos) {
		int second = emails.find('\n', first);
		if (second == string::npos)
			break;
		else second++;
		string num = emails.substr(first, second - first - 1);
		first = emails.find('\n', second);
		string title;
		if (first == string::npos)
			title = emails.substr(second);
		else {
			first++;
			title = emails.substr(second, first - second - 1);
		}
		email_mid += generateLI(num, title, url);
	}

	return email_mid;
}

string getListResponse(string user, string url, const char* begin, const char* end, string liurl) {
	string response = HTTP_HEADER;

	StorageClient client(grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()));
	string emails = client.Get(user, url);
	string email_begin = readHTMLFile(begin);
	string email_end = readHTMLFile(end);
	string email_mid;
	email_mid = getEmailMid(emails, liurl);
	email_begin += email_mid + email_end;
	response += to_string(email_begin.length()) + "\n";
	response += "\n";
	response += email_begin;
	return response;
}
#endif /* HANDLEEMAILS_H_ */
