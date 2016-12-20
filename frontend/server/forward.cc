#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <string.h>
#include <iostream>
#include <pthread.h>
#include <signal.h>
#include <set>
#include <errno.h>
#include <vector>
#include <dirent.h>
#include <map>
#include <ctime>
#include <resolv.h>
#include <openssl/md5.h>
#include <sstream>
#include <grpc++/grpc++.h>

#include "helper.h"
#include "../../backend/storage_query/master_client.h"
#include "../../backend/storage_query/storage_client.h"
#include "constants.h"

#define BUF_SIZE 4096
#define DEFAULT_PORT_F 11000
#define LINE_LIMIT 1000
#define MAX_CON 128
static const char* FROM_ = "FROM ";
static const char* MQUEUE = "mqueue";
static const char* LOCALHOST = "t04.com";
static const char* HELOCRLF = "HELO localhost\r\n";
static const char* MAIL_FROM_ = "MAIL FROM: ";
static const char* RCPT_TO_ = "RCPT TO: ";
static const char* DATACRLF = "DATA\r\n";
static const char* QUITCRLF = "QUIT\r\n";
static const char* DOT_CRLF = ".\r\n";
static bool isDebug_f = false;
const char* mqueue;
char* buf = new char[LINE_LIMIT]();

struct Email {
//	vector<char*>* lines;
	char* firstLine;
	char* text;
	int bytes;
//	char* md5Digests;
	bool isDeleted = false;
};
using namespace std;

string* generateMX(const char* domain) {
	u_char buf[BUF_SIZE];
	char bufRet[BUF_SIZE];
	ns_msg msg;
	ns_rr rr;
	string* ret = NULL;
	int a = res_query(domain, ns_c_any, ns_t_mx, buf, BUF_SIZE);
	if (a < 0) {
		fputs("Cannot generate MX\n", stderr);
	} else {
		ns_initparse(buf, a, &msg);
		a = ns_msg_count(msg, ns_s_an);
		ns_parserr(&msg, ns_s_an, 0, &rr);
		ns_sprintrr(&msg, &rr, NULL, NULL, bufRet, BUF_SIZE);
		if (isDebug_f) cerr << bufRet << endl;
		ret = new string(bufRet);
		int lastSpace = (*ret).find_last_of(" ") + 1;
		int lastDot = ret->find_last_of(".");
		(*ret) = ret->substr(lastSpace, lastDot - lastSpace);
	}
	return ret;
}

string* generateA(const char* domain) {
	u_char buf[BUF_SIZE];
	char bufRet[BUF_SIZE];
	ns_msg msg;
	ns_rr rr;
	string* ret = NULL;
	int a = res_query(domain, ns_c_any, ns_t_a, buf, sizeof(buf));
	if (a < 0) {
		fputs("Cannot generate A\n", stderr);
	} else {
		ns_initparse(buf, a, &msg);
		a = ns_msg_count(msg, ns_s_an);
		ns_parserr(&msg, ns_s_an, 0, &rr);
		ns_sprintrr(&msg, &rr, NULL, NULL, bufRet, sizeof(bufRet));
		if (isDebug_f) cerr << bufRet << endl;
		ret = new string(bufRet);
		int last = ret->find_last_of(" \t") + 1;
		(*ret) = ret->substr(last);
	}
	return ret;
}

string* getMXIP(const char* domain) {
	string* MX = generateMX(domain);
	return generateA(MX->c_str());
}

int computeTextLen(vector<const char*>* lines) {
	int len = 0;
	for (int i = 1; i < lines->size(); i++) {
		int lineLen = strlen((*lines)[i]);
		len += lineLen;
	}
	return len;
}

void generateText(vector<const char*>* lines, char* text, char* firstLine) {
	strncpy(firstLine, (*lines)[0], strlen((*lines)[0]));
	int len = 0;
	for (int i = 1; i < lines->size(); i++) {
		int lineLen = strlen((*lines)[i]);
		strncpy(text + len, (*lines)[i], lineLen);
		len += lineLen;
	}
}

void saveEmail(vector<const char*>*& lines, vector<Email*>* emails) {
	Email* email = new Email;
	email->bytes = computeTextLen(lines);
	char* text = new char[email->bytes + 1];
	bzero(text, email->bytes + 1);
	char* firstLine = new char[strlen((*lines)[0]) + 1];
	bzero(firstLine, strlen((*lines)[0]) + 1);
	generateText(lines, text, firstLine);
//	email->lines = lines;
	email->text = text;
	email->firstLine = firstLine;
	emails->push_back(email);
}

char* getHostnameByEmailAddr(const char* receiver) {
	char* ret = new char[strlen(receiver)]();
	int firstChar = 0;
	while (receiver[firstChar] != '@')
		firstChar++;
	firstChar++;
	for (int i = firstChar; i < strlen(receiver) && receiver[i] != '\r' && receiver[i] != '\n' && receiver[i] != '>'; i++)
		ret[i - firstChar] = receiver[i];
	return ret;
}

vector<string*>* generateOtherEmails(string* text) {
	int lastIndex = text->find_first_of("\r\n");
	string* TO = new string;
	(*TO) = text->substr(0, lastIndex);
	int firstSpace = 0;
	vector<string*>* retVec = new vector<string*>;
	while ((firstSpace = TO->find(' ')) != string::npos) {
		(*TO) = TO->substr(firstSpace + 1);
		int secondSpace = TO->find(' ');
		string* ret = new string;
		if (secondSpace == string::npos) {
			ret = TO;
			char* hostname = getHostnameByEmailAddr(ret->c_str());
			if (strncmp(hostname, LOCALHOST, strlen(LOCALHOST))) {
				retVec->push_back(ret);
				break;
			}
		} else {
			(*ret) = TO->substr(0, secondSpace - 1);
			char* hostname = getHostnameByEmailAddr(ret->c_str());
			if (strncmp(hostname, LOCALHOST, strlen(LOCALHOST))) retVec->push_back(ret);
		}
	}
	return retVec;
}

int openSocket(int& sockfd, struct sockaddr_in serverAddr, const char* ip, int port) {
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "Cannot open a socket. \n");
		exit(5);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &serverAddr.sin_addr);
	serverAddr.sin_port = htons(port);

	if (connect(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1) {
		if (isDebug_f) cerr << "Cannot connect the host" << endl;
		return -1;
	}
	return 0;
}

string* generateFromEmailAddr(string* text) {
	int first = text->find("From: ") + 6;
	int second = text->find("\r\n", first);
	string* ret = new string;
	(*ret) = text->substr(first, second - first);
//	(*ret) = text->substr(loc);
//	loc = ret->find_first_of('<') + 1;
//	int secondLoc = ret->find_first_of('>');
//	(*ret) = ret->substr(loc, secondLoc - loc);
	return ret;
}

void run() {
	//read mbox files' names
	//	char* folderName = argv[optind];
	//	string folderStr = folderName;
	//	if (folderStr.find_last_of('/', folderStr.length() - 1)
	//			!= (folderStr.length() - 1))
	//		folderStr += "/";
	//	string mqueueStr(MQUEUE);
	//	mqueueStr = folderStr + mqueueStr;
	//	mqueue = mqueueStr.c_str();
	//	FILE* f = fopen(mqueue, "r");
	StorageClient client = getClient("mqueue");
	string mqueue;
	client.Get("mqueue", "mqueue", mqueue);
	if (mqueue.length() == 0) return;
	vector<Email*>* emails = new vector<Email*>;
	vector<const char*>* lines = NULL;
	int first = 0;
	int end;
	//	while (fgets(buf, LINE_LIMIT, f) != NULL) {
	while ((end = mqueue.find("\n", first)) != string::npos) {
		string* lineStr = new string;
		(*lineStr) = mqueue.substr(first, end - first + 1);
		first = end + 1;
		const char* line = lineStr->c_str();
		if (strncmp(line, FROM_, strlen(FROM_)) == 0) {
			if (lines != NULL) {
				saveEmail(lines, emails);
				delete lines;
			}
			lines = new vector<const char*>;
		}
		lines->push_back(line);
	}
	saveEmail(lines, emails);
	for (int i = 0; i < emails->size(); i++) {
		Email* email = (*emails)[i];
		string* pEmailTextStr = new string(email->text);
		vector<string*>* emailAddrs = generateOtherEmails(pEmailTextStr);
		for (int j = 0; j < emailAddrs->size(); j++) {
			string* emailAddr = (*emailAddrs)[j];
			if (isDebug_f) cerr << "Send email to: " << emailAddr->c_str() << endl;
			int firstAt = emailAddr->find('@');
			string* hostname = new string;
			(*hostname) = emailAddr->substr(firstAt + 1);
			string* ip = getMXIP(hostname->c_str());
			if (isDebug_f) cerr << "Hostname: " << ip->c_str() << endl;
			struct sockaddr_in serverAddr;
			int sockfd;
			if (openSocket(sockfd, serverAddr, ip->c_str(), 25) == -1) {
				if (isDebug_f) cerr << "Cannot connect to the SMTP server" << endl;
				continue;
			}

			char* junk = new char[LINE_LIMIT]();
			read(sockfd, junk, LINE_LIMIT);
			if (isDebug_f) cerr << "Message from SMTP: " << junk << endl;
			bzero(junk, LINE_LIMIT);
			//HELO
			write(sockfd, HELOCRLF, strlen(HELOCRLF));
			read(sockfd, junk, LINE_LIMIT);
			if (isDebug_f) cerr << "Message from SMTP: " << junk << endl;
			bzero(junk, LINE_LIMIT);
			//MAIL
			string mailFrom(MAIL_FROM_);
			string* mailFromAddr = generateFromEmailAddr(pEmailTextStr);
			mailFrom = mailFrom + (*mailFromAddr) + "\r\n";
			write(sockfd, mailFrom.c_str(), strlen(mailFrom.c_str()));
			read(sockfd, junk, LINE_LIMIT);
			if (isDebug_f) cerr << "Message from SMTP: " << junk << endl;
			bzero(junk, LINE_LIMIT);
			//RCPT
			string rcptTo(RCPT_TO_);
			rcptTo = rcptTo + emailAddr->c_str() + "\r\n";
			write(sockfd, rcptTo.c_str(), strlen(rcptTo.c_str()));
			read(sockfd, junk, LINE_LIMIT);
			if (isDebug_f) cerr << "Message from SMTP: " << junk << endl;
			bzero(junk, LINE_LIMIT);
			//DATA
			write(sockfd, DATACRLF, strlen(DATACRLF));
			write(sockfd, email->text, email->bytes);
			write(sockfd, DOT_CRLF, strlen(DOT_CRLF));
			read(sockfd, junk, LINE_LIMIT);
			if (isDebug_f) cerr << "Message from SMTP: " << junk << endl;
			bzero(junk, LINE_LIMIT);
			//QUIT
			write(sockfd, QUITCRLF, strlen(QUITCRLF));
			read(sockfd, junk, LINE_LIMIT);
			if (isDebug_f) cerr << "Message from SMTP: " << junk << endl;
			free(junk);
			close(sockfd);
			email->isDeleted = true;
		}
	}
	//	FILE* fw = fopen(mqueue, "w");
	string output;
	for (int i = 0; i < (*emails).size(); i++) {
		Email* email = (*emails)[i];
		if (email->isDeleted) continue;

		output += email->firstLine;
		output += email->text;
		//		fputs(email->firstLine, fw);
		//		fputs(email->text, fw);
	}
	client.Put("mqueue", "mqueue", output);
}

int main(int argc, char* argv[]) {
	// parse arguments
	char arg_char;
	int port = DEFAULT_PORT_F;
	while ((arg_char = getopt(argc, argv, "av")) != -1)
		switch (arg_char) {
		case 'a':
			fprintf(stderr, "*** Author: Zhi Xu (xuzhi)\n");
			exit(1);
			break;
		case 'v':
			isDebug_f = true;
			break;
		case '?':
			fprintf(stderr, "Not supported option.\n");
			exit(3);
		}

	while (true) {
		if (isDebug_f) cout << "Forward emails" << endl;
		try {
			run();
		} catch (int e) {
			cout << "Error: " << e << endl;
		}
		sleep(5);
	}
	return 0;
}
