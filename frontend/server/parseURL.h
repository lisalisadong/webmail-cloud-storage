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
#include "handleEmails.h"
#include "handleFiles.h"

using namespace std;
#ifndef PARSEURL_H_
#define PARSEURL_H_

void mqueueInitialize() {
	StorageClient client = getClient("mqueue");
	client.Put("mqueue", "mqueue", "");
//	string mqueue = "FROM xxx\nTo: zhixu93@gmail.com\r\nFrom: zhixu@t04.com\r\nSubject: AAA\r\nDate: Mon, 24 Oct 2016 01:25:53 -0400\r\n\r\nAAA\r\n";
//	client.Put("mqueue", "mqueue", mqueue);
}

void testInitialize() {
	string user("Alice");
	StorageClient client = getClient(user);

	client.Put(user, PWD, "aaa");

	string emails = "1\nAAA, Mon, 24 Oct 2016 01:25:53 -0400\n2\nAAA, Mon, 24 Oct 2016 01:25:53 -0400\n4\nBBBBBB, Mon, 17 Oct 2016 01:25:53 -0400\n";
	string email1 = "To: Alice@t04.com\r\nFrom: zhixu@t04.com\r\nSubject: AAA\r\nDate: Mon, 24 Oct 2016 01:25:53 -0400\r\n\r\nAAA\r\n";
	string email2 = "To: Alice@t04.com\r\nFrom: zhixu@t04.com\r\nSubject: AAA\r\nDate: Mon, 17 Oct 2016 01:25:53 -0400\r\n\r\nBBB\r\n";
	string email4 = "To: Alice@t04.com\r\nFrom: zhixu@t04.com\r\nSubject: BBBBBB\r\nDate: Mon, 10 Oct 2016 01:25:53 -0400\r\n\r\nDDD\r\n";
//	string emails = to_string(email1.length()) + "\r\n" + email1 +to_string(email2.length()) + "\r\n" + email2 + to_string(email3.length()) + "\r\n" + email3;
	string col("emails");
	client.Put(user, "emails", emails);
	cout << "Put Alice emails: " << endl << emails << endl;

	client.Put(user, "email-1", email1);
	client.Put(user, "email-2", email2);
	client.Put(user, "email-4", email4);

	client.Put(user, "files", "folder-aaa\nfolder-bbb\nfile-1.txt\n");
	client.Put(user, "folder-aaa", "folder-aaa/..\nfolder-aaa/homework3\nfolder-aaa/solution\nfile-aaa/a.png\n");
	client.Put(user, "folder-aaa/solution", "folder-aaa/solution/..\nfile-aaa/solution/2.txt\n");
	client.Put(user, "folder-aaa/homework3", "folder-aaa/homework3/..\n");
	client.Put(user, "folder-bbb", "folder-bbb/..\n");

	/*
	 *	files
	 *	|--aaa
	 *	|	|--homework3
	 *	|	|--solution
	 *	|	|	|--2.txt
	 *	|	|--a.png
	 *	|--bbb
	 *	|--1.txt
	 */
	string apng;
	string txt1;
	string txt2;
	readFile("a.png", apng);
	readFile("1.txt", txt1);
	readFile("2.txt", txt2);
	client.Put(user, "file-aaa/a.png", apng);
	client.Put(user, "file-1.txt", txt1);
	client.Put(user, "file-aaa/solution/2.txt", txt2);

	string response;
	client.Get(user, col, response);
	cout << "Get Alice emails: " << endl << response << endl;
	string fileResponse;
	client.Get(user, "file-aaa/solution/2.txt", fileResponse);
	cout << "Get file-1: " << endl << fileResponse << endl;
}

void testInitializezhixu() {
	string user("zhixu");
	StorageClient client = getClient(user);

	client.Put(user, PWD, "aaa");

	string emails = "1\nAAA, Mon, 24 Oct 2016 01:25:53 -0400\n2\nAAA, Mon, 24 Oct 2016 01:25:53 -0400\n4\nBBBBBB, Mon, 17 Oct 2016 01:25:53 -0400\n";
	string email1 = "To: Alice@t04.com\r\nFrom: zhixu@t04.com\r\nSubject: AAA\r\nDate: Mon, 24 Oct 2016 01:25:53 -0400\r\n\r\nAAA\r\n";
	string email2 = "To: Alice@t04.com\r\nFrom: zhixu@t04.com\r\nSubject: AAA\r\nDate: Mon, 17 Oct 2016 01:25:53 -0400\r\n\r\nBBB\r\n";
	string email4 = "To: Alice@t04.com\r\nFrom: zhixu@t04.com\r\nSubject: BBBBBB\r\nDate: Mon, 10 Oct 2016 01:25:53 -0400\r\n\r\nDDD\r\n";
//	string emails = to_string(email1.length()) + "\r\n" + email1 +to_string(email2.length()) + "\r\n" + email2 + to_string(email3.length()) + "\r\n" + email3;
	string col("emails");
	client.Put(user, "emails", emails);
	cout << "Put " + user + " emails: " << endl << emails << endl;

	client.Put(user, "email-1", email1);
	client.Put(user, "email-2", email2);
	client.Put(user, "email-4", email4);

	client.Put(user, "files", "folder-aaa\nfolder-bbb\nfile-1.txt\n");
	client.Put(user, "folder-aaa", "folder-aaa/..\nfolder-aaa/homework3\nfolder-aaa/solution\nfile-aaa/a.png\n");
	client.Put(user, "folder-aaa/solution", "folder-aaa/solution/..\nfile-aaa/solution/2.txt\n");
	client.Put(user, "folder-aaa/homework3", "folder-aaa/homework3/..\n");
	client.Put(user, "folder-bbb", "folder-bbb/..\n");

	/*
	 *	files
	 *	|--aaa
	 *	|	|--homework3
	 *	|	|--solution
	 *	|	|	|--2.txt
	 *	|	|--a.png
	 *	|--bbb
	 *	|--1.txt
	 */
	string apng;
	string txt1;
	string txt2;
	readFile("a.png", apng);
	readFile("1.txt", txt1);
	readFile("2.txt", txt2);
	client.Put(user, "file-aaa/a.png", apng);
	client.Put(user, "file-1.txt", txt1);
	client.Put(user, "file-aaa/solution/2.txt", txt2);

	string response;
	client.Get(user, col, response);
	cout << "Get Alice emails: " << endl << response << endl;
	string fileResponse;
	client.Get(user, "file-aaa/solution/2.txt", fileResponse);
	cout << "Get file-1: " << endl << fileResponse << endl;
}

string getResponse(const char* fileLoc, const char* headers) {
	string response = HTTP_HEADER;
	string fileStr = readHTMLFile(fileLoc);
	response += to_string(fileStr.length()) + "\n";
	response += headers;
	response += "\n";
	response += fileStr;
	return response;
}

void getUserPassword(string& urlStr, string& user, string& password) {
	int usernameFirst = urlStr.find("username") + 9;
	int usernameEnd = urlStr.find('&', usernameFirst);
	user = urlStr.substr(usernameFirst, usernameEnd - usernameFirst);

	int passwordFirst = urlStr.find("password") + 9;
	int passwordEnd = urlStr.find('&', passwordFirst);
	if (passwordEnd == string::npos) passwordEnd = urlStr.length();
	password = urlStr.substr(passwordFirst, passwordEnd - passwordFirst);
}

void createUser(string& user, string lastLine, string& sessionid) {
	string password;
	getUserPassword(lastLine, user, password);
	StorageClient client = getClient(user);
	client.Put(user, PWD, password);
	sessionid = generateSessionID(user);
	client.Put(user, SESSIONID, sessionid);
	client.Put(user, "emails", "");
	client.Put(user, "files", "");
}

bool checkPWD(string lastLine, string& user, string& sessionid) {
	string password;
	getUserPassword(lastLine, user, password);
	if (isDebug) cout << "Current login user is: " << user << " with password: " << password << endl;

	StorageClient client = getClient(user);
	string actualPwd;
	if (!client.Get(user, PWD, actualPwd) || actualPwd.compare(password)) return false;
	sessionid = generateSessionID(user);
	client.Put(user, SESSIONID, sessionid);
	return true;
}

bool checkCookie(string cookie, string& user) {
	if (cookie.length() <= 0) return false;
	int newLineI = cookie.find_first_of("\r\n");
	if (newLineI != string::npos) cookie.erase(newLineI);

	int sessionidI = cookie.find("sessionid=");
	int userI = cookie.find("user=");
	string sessionid = cookie.substr(sessionidI + 10, userI - sessionidI - 12);
	user = cookie.substr(userI + 5);
	StorageClient client = getClient(user);
	string actualSessionid;
	if (!client.Get(user, SESSIONID, actualSessionid)) return false;
	return !sessionid.compare(actualSessionid);
}

string putCredentials(const char*& lastLine) {
	string response;
	string sessionid;
	string user;
	createUser(user, lastLine, sessionid);
	string headers = "Set-Cookie: sessionid=" + sessionid + "\nSet-Cookie: user=" + user + "\n";
	response = getResponse("frontend/sites/getIntoRoot.html", headers.c_str());
	return response;
}

bool checkPWD(string lastLine) {
	int usernameFirst = lastLine.find("username") + 9;
	int usernameEnd = lastLine.find('&', usernameFirst);
	string username = lastLine.substr(usernameFirst, usernameEnd - usernameFirst);

	int prepwdFirst = lastLine.find("prepwd") + 7;
	int prepwdEnd = lastLine.find('&', prepwdFirst);
	string prepwd = lastLine.substr(prepwdFirst, prepwdEnd - prepwdFirst);

	int passwordFirst = lastLine.find("password") + 9;
	int passwordEnd = lastLine.find('&', passwordFirst);
	if (passwordEnd == string::npos) passwordEnd = lastLine.length();
	string password = lastLine.substr(passwordFirst, passwordEnd - passwordFirst);

	StorageClient client = getClient(username);
	string actualPWD;
	client.Get(username, PWD, actualPWD);
	if (!actualPWD.compare(prepwd)) {
		client.CPut(username, PWD, prepwd, password);
		return true;
	} else return false;
}

int generateHTML(struct Message* pM, const char* url, const char* lastLine, string cookie, string referer, int contentLen) {
//	get();
	string urlStr = url;
	string response = "";
	if (!strncmp(url, SIGNUP, strlen(SIGNUP))) {
		response = getResponse("frontend/sites/signup.html", "");
	} else if (!strncmp(url, CREDENTIALS, strlen(CREDENTIALS))) {
		response = putCredentials(lastLine);
	} else if (!strncmp(url, CHANGEPWD, strlen(CHANGEPWD))) {
		response = getResponse("frontend/sites/changepwd.html", "");
	} else if (!strncmp(url, CHECKPWD, strlen(CHECKPWD))) {
		if (checkPWD(lastLine)) {
			response = getResponse("frontend/sites/getIntoRoot.html", "");
		} else {
			response = getResponse("frontend/sites/changepwdError.html", "");
		}
	} else {
		string user;
		if (checkCookie(cookie, user)) {
			if (!strncmp(url + 1, " ", 1)) {
				response = getResponse("frontend/sites/dashboard.html", "");
			} else if (!strncmp(url, EMAILS, strlen(EMAILS))) {
				response = getListResponse(user, string("emails"), "frontend/sites/emails_begin.html", "frontend/sites/emails_end.html", string("email"));
			} else if (!strncmp(url, FILES, strlen(FILES)) || !strncmp(url, FOLDER_, strlen(FOLDER_))) {
				string dir = url + 1;
				dir = dir.substr(0, dir.find(' '));
				response = getFileListResponse(user, "frontend/sites/files_begin.html", "frontend/sites/files_end.html", dir);
			} else if (!strncmp(url, EMAIL_, strlen(EMAIL_))) {
				response = getEmailResponse(user, url);
			} else if (!strncmp(url, FILE_, strlen(FILE_))) {
				getFileResponse(user, url, pM);
				return 0;
			} else if (urlStr.find(NEW_FOLDER) != string::npos) {
				createNewFolder(user, referer, lastLine);
				response = getResponse("frontend/sites/getBack.html", "");
			} else if (!strncmp(url, UPLOAD_FILE, strlen(UPLOAD_FILE))) {
				createNewFile(user, referer, lastLine, contentLen);
				response = getResponse("frontend/sites/getBack.html", "");
			} else if (!strncmp(url, WRITE_EMAIL, strlen(WRITE_EMAIL))) {
				response = getResponse("frontend/sites/writeEmail.html", "");
			} else if (!strncmp(url, SEND_EMAIL, strlen(SEND_EMAIL))) {
				createEmail(user, lastLine);
				response = getResponse("frontend/sites/getIntoEmails.html", "");
			} else if (!strncmp(url, FORWARD, strlen(FORWARD))) {
				forwardEmail(user, referer, lastLine);
				response = getResponse("frontend/sites/getIntoEmails.html", "");
			} else if (!strncmp(url, REPLY, strlen(REPLY))) {
				replyEmail(user, referer, lastLine);
				response = getResponse("frontend/sites/getIntoEmails.html", "");
			} else if (!strncmp(url, DELETE_EMAIL, strlen(DELETE_EMAIL))) {
				deleteEmail(user, referer);
				response = getResponse("frontend/sites/getIntoEmails.html", "");
			} else if (!strncmp(url, MOVE_FILE, strlen(MOVE_FILE))) {
				moveFile(user, lastLine);
				response = getResponse("frontend/sites/getBack.html", "");
			} else if (!strncmp(url, RENAME_FILE, strlen(RENAME_FILE))) {
				renameFile(user, lastLine, referer);
				response = getResponse("frontend/sites/getBack.html", "");
			} else if (!strncmp(url, DELETE_FILE, strlen(DELETE_FILE))) {
				deleteFileOrFolder(user, lastLine, referer);
				response = getResponse("frontend/sites/getBack.html", "");
			} else response = getResponse("frontend/sites/notfound.html", "");
		} else {
			if (!strncmp(url, LOGINSUBMIT, strlen(LOGINSUBMIT))) {
				string sessionid;
				if (checkPWD(lastLine, user, sessionid)) {
					string headers = "Set-Cookie: sessionid=" + sessionid + "\nSet-Cookie: user=" + user + "\n";
					response = getResponse("frontend/sites/dashboard.html", headers.c_str());
				} else response = getResponse("frontend/sites/loginError.html", "");
			} else if (!strncmp(url + 1, " ", 1))
				response = getResponse("frontend/sites/login.html", "");
			else response = getResponse("frontend/sites/notfound.html", "");
		}
	}
	write(pM->confd, response.c_str(), response.length());
	return 0;
}

#endif /* PARSEURL_H_ */
