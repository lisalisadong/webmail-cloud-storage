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

//	StorageClient client(grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()));
	StorageClient client = getClient(user);
	string email;
	client.Get(user, emailName, email);
	int crlf;
	while ((crlf = email.find("\r\n")) != string::npos)
		email.replace(crlf, 2, "<br/>");

	string email_begin = readHTMLFile("frontend/sites/emailDisplay_begin.html");
	string email_end = readHTMLFile("frontend/sites/emailDisplay_end.html");
	email = email_begin + email + email_end;

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

//	StorageClient client(grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()));
	StorageClient client = getClient(user);
	string emails;
	client.Get(user, url, emails);
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

void addReceiver(vector<string>* receivers, string to) {
	int space;
	while ((space = to.find(" ")) != string::npos)
		to.replace(space, 1, "");
//	to = "To: " + to + "\r\n";
	receivers->push_back(to);
}

void sendEmail(vector<string>& receivers, string& header, string& email) {
	for (int i = 0; i < receivers.size(); i++) {
		string receiver = receivers[i];
		int at2 = receiver.find("@");
		if (!receiver.substr(at2 + 1).compare(DOMAIN_NAME)) {
			string receiverPre = receiver.substr(0, at2);
			StorageClient client = getClient(receiverPre);
			string emails;
			if (!client.Get(receiverPre, "emails", emails)) continue;

			int numLF = emails.find_last_of("\n", emails.length() - 2);
			int numPreLF = emails.find_last_of("\n", numLF - 1);
			string numStr = emails.substr(numPreLF + 1, numLF - numPreLF - 1);
			int num = atoi(numStr.c_str());
			numStr = to_string(num + 1);
			string emailName = "email-" + numStr;
			client.Put(receiverPre, "emails", emails + numStr + header);
			client.Put(receiverPre, emailName, email);
		} else {
			string outEmail = "FROM xxx\n" + email;

			int toStart = outEmail.find("To: ") + 4;
			int toEnd = outEmail.find("\r\n", toStart);
			outEmail.replace(toStart, toEnd - toStart, receiver);

			StorageClient client = getClient("mqueue");
			string mqueue;
			client.Get("mqueue", "mqueue", mqueue);
			mqueue += outEmail;
			client.Put("mqueue", "mqueue", mqueue);
		}
	}

}

vector<string> getReceivers(string& toLine) {
	vector<string> receivers;
	int at;
	while ((at = toLine.find("%40")) != string::npos)
		toLine.replace(at, 3, "@");
	int toFirst = 0;
	int comma = toLine.find(',');
	while (comma != string::npos) {
		string to = toLine.substr(toFirst, comma - toFirst);
		addReceiver(&receivers, to);
		toFirst = comma + 1;
		comma = toLine.find(',', comma + 1);
	}
	string to = toLine.substr(toFirst);
	addReceiver(&receivers, to);
	return receivers;
}

void createEmail(string user, string lastLine) {
	string fromLine = "From: " + user + "@" + DOMAIN_NAME + "\r\n";

	int andToEqualIndex = lastLine.find("&to=") + 4;
	int andContentEqualIndex = lastLine.find("&content=");
	int first = strlen(SUBJECT) + 1;
	string subject = lastLine.substr(first, andToEqualIndex - first - 4);
	string subjectLine = "Subject: " + subject + "\r\n";

	string toLine = lastLine.substr(andToEqualIndex, andContentEqualIndex - andToEqualIndex); //zhixu%40localhost, xx%40localhost

	vector<string> receivers = getReceivers(toLine);
	toLine = "To: " + toLine + "\r\n";

	time_t now = time(0);
	string date = ctime(&now);
	date.replace(date.find('\n'), 1, "");
	string dateLine = "Date: " + date + "\r\n";
	string content = lastLine.substr(andContentEqualIndex + 9);
	string contentLine = content + "\r\n";
	filterHex(contentLine);
	string header = "\n" + subject + ", " + date + "\n";

	string email = toLine + fromLine + subjectLine + dateLine + "\r\n" + contentLine;

	sendEmail(receivers, header, email);
}

void forwardEmail(string user, string referer, string lastLine) {
	int start = referer.find_last_of('/') + 1;
	string emailName = referer.substr(start, referer.length() - 2 - start);

	StorageClient client = getClient(user);
	string email;
	if (!client.Get(user, emailName, email)) return;
	string toLine = lastLine.substr(lastLine.find('=') + 1);
	vector<string> receivers = getReceivers(toLine);

	int toStart = email.find("To: ") + 4;
	int toEnd = email.find("\r\n", toStart);
	email.replace(toStart, toEnd - toStart, toLine);

	int subjectIndex = email.find("Subject: ");
	int subjectEnd = email.find("\r\n", subjectIndex);
	string subject = email.substr(subjectIndex, subjectEnd - subjectIndex);
	subject.replace(0, 9, "");
	time_t now = time(0);
	string date = ctime(&now);
	string header = "\nFW: " + subject + ", " + date;

	sendEmail(receivers, header, email);
}

void replayEmail(string user, string referer, string lastLine) {
	string fromLine = "From: " + user + "@" + DOMAIN_NAME + "\r\n";

	int start = referer.find_last_of('/') + 1;
	string emailName = referer.substr(start, referer.length() - 2 - start);

	StorageClient client = getClient(user);
	string email;
	if (!client.Get(user, emailName, email)) return;

	int fromIndex = email.find("From: ");
	int fromEnd = email.find("\r\n", fromIndex + 1);
	string toLine = email.substr(fromIndex, fromEnd - fromIndex);
	toLine.replace(0, 6, "");
	vector<string> receivers = getReceivers(toLine);
	toLine = "To: " + toLine + "\r\n";

	int subjectIndex = email.find("Subject: ");
	int subjectEnd = email.find("\r\n", subjectIndex);
	string subject = email.substr(subjectIndex, subjectEnd - subjectIndex);
	subject.replace(0, 9, "");
	subject = "Re: " + subject;
	time_t now = time(0);
	string date = ctime(&now);
	string header = "\n" + subject + ", " + date;

	string content = lastLine.substr(lastLine.find("=") + 1);
	filterHex(content);
	string contentLine = content + "\r\n";

	string subjectLine = subject;

	string dateLine = "Date: " + date + "\r\n";

	string finalEmail = toLine + fromLine + subjectLine + dateLine + "\r\n" + contentLine;

	sendEmail(receivers, header, finalEmail);
}

void deleteEmail(string user, string referer) {
	StorageClient client = getClient(user);
	string emails;
	client.Get(user, "emails", emails);

	string emailName = referer.substr(referer.find_last_of('/') + 1);
	emailName.replace(emailName.length() - 2, 2, "");
	client.Delete(user, emailName);

	string preNumStr = emailName.substr(emailName.find_last_of('-') + 1);

	int first = 0;
	int second;
	int third;
	while ((second = emails.find('\n', first)) != string::npos) {
		third = emails.find('\n', second + 1);
		string numStr = emails.substr(first, second - first);
		if (!numStr.compare(preNumStr)) break;
		first = third + 1;
	}
	emails.replace(first, third - first + 1, "");
	client.Put(user, "emails", emails);
}
#endif /* HANDLEEMAILS_H_ */
