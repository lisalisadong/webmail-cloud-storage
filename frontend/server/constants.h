/*
 * constants.h
 *
 *  Created on: Dec 12, 2016
 *      Author: cis505
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#define LINE_LIMIT 1000
#define MAX_CON 128
#define DEFAULT_PORT 8080

const static char* HTTP_HEADER = "HTTP/1.0 200 OK\nDate: Fri, 31 Dec 1999 23:59:59 GMT\nContent-Type: text/html\nContent-Length: ";
const static char* HTTP_HEADER_FILE = "HTTP/1.0 200 OK\nDate: Fri, 31 Dec 1999 23:59:59 GMT\nContent-Type: multipart/form-data\nContent-Length: ";

const static char* CONTENT_LEN = "Content-Length: ";
const static char* COOKIE = "Cookie: ";



#endif /* CONSTANTS_H_ */
