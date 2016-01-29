/*
    A simple HTTP Web Server.
    Copyright (C) 04/07/15  José Luis Valencia Herrera

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "httpParser.h"

static int setRequestMethod(HttpRequestData_t *requestData, const char *requestMethod);
static int setHttpVersion(HttpRequestData_t *requestData, char *requestVersion);

void httpParser_Init() {

}

int httpParser_GetRequestLines(char *message, char linesBuffer[][MAX_HTTP_REQUEST_SIZE], const size_t linesBufferLen, char **saveState) {
    char *result = NULL;
    int linesParsed = 0;

    result = strtok_r(message, "\n", saveState);

    if (result) {
        do {
            if (linesParsed >= linesBufferLen) {
                break;
            }

            strncpy(linesBuffer[linesParsed], result, MAX_HTTP_REQUEST_SIZE);
            ++linesParsed;
            result = strtok_r(NULL, "\n", saveState);
        }
        while (result);
    }

    return linesParsed;
}

int httpParser_ParseRequestLine(char *requestLine, HttpRequestData_t *requestData, char **saveState) {
    char *result = NULL;

    result = strtok_r(requestLine, " ", saveState);
    if (!result) return 0;
    if (!setRequestMethod(requestData, result)) return 0;

    result = strtok_r(NULL, " ", saveState);
    if (!result) return 0;
    strncpy(requestData->requestURI, result, MAX_REQUEST_URI_LEN);

    result = strtok_r(NULL, " ", saveState);
    if (!result) return 0;
    if (!setHttpVersion(requestData, result)) return 0;

    return 1;
}

static int setRequestMethod(HttpRequestData_t *requestData, const char *requestMethod) {
    if (!requestMethod || !requestData) {
        return 0;
    }

    if (0 == strcmp(requestMethod, "GET")) {
        requestData->requestMethod = HTTP_REQM_GET;
    }
    else if (0 == strcmp(requestMethod, "HEAD")) {
        requestData->requestMethod = HTTP_REQM_HEAD;
    }
    else if (0 == strcmp(requestMethod, "POST")) {
        requestData->requestMethod = HTTP_REQM_POST;
    }
    else if (0 == strcmp(requestMethod, "PUT")) {
        requestData->requestMethod = HTTP_REQM_PUT;
    }
    else if (0 == strcmp(requestMethod, "DELETE")) {
        requestData->requestMethod = HTTP_REQM_DELETE;
    }
    else if (0 == strcmp(requestMethod, "CONNECT")) {
        requestData->requestMethod = HTTP_REQM_CONNECT;
    }
    else if (0 == strcmp(requestMethod, "OPTIONS")) {
        requestData->requestMethod = HTTP_REQM_OPTIONS;
    }
    else if (0 == strcmp(requestMethod, "TRACE")) {
        requestData->requestMethod = HTTP_REQM_TRACE;
    }
    else {
        return 0;
    }

    return 1;
}

static int setHttpVersion(HttpRequestData_t *requestData, char *requestVersion) {
    char *result = NULL;
    char *saveState = NULL;
    
    if (!requestVersion || !requestData) {
        return 0;
    }
    
    result = strtok_r(requestVersion, "/", &saveState);
    
    if (!result) return 0;
    
    if (0 != strcmp(result, "HTTP")) return 0;
    
    result = strtok_r(NULL, "/", &saveState);

    if (!result) return 0;
    
    requestData->httpVersion = atof(result);
    
    return 1;
}

int httpParser_GenerateHttpResponse(char *replyBuffer, size_t bufferLen, const HttpStatusCode_t statusCode, const HttpContentType_t contentType, const char *content, const int contentLength) {
    return snprintf(replyBuffer, bufferLen, "HTTP/1.1 %d OK\r\nContent-Length: %d\r\nContent-Type: %s\r\nConnection: Closed\r\n\r\n%s\r\n", statusCode, contentLength, httpParser_GetContentTypeAsString(contentType), content);
}

HttpContentType_t httpParser_DetermineContentTypeFromFileExtension(const char *filePath) {
    char *token = NULL;
    char *saveState = NULL;
    char tmpFileBuffer[BUFSIZ];
    char auxBuffer[BUFSIZ];
    HttpContentType_t contentType;

    memset(auxBuffer, 0, BUFSIZ);
    memcpy(tmpFileBuffer, filePath, BUFSIZ);

    token = strtok_r(tmpFileBuffer, ".", &saveState);

    while(token) {
	memcpy(auxBuffer, token, BUFSIZ);

        token = strtok_r(NULL, ".", &saveState);
    }

    if (strcmp(auxBuffer, "html") == 0) {
	contentType = HTTP_CONTENT_TEXT_HTML;
    }
    else if (strcmp(auxBuffer, "xml") == 0) {
	contentType = HTTP_CONTENT_TEXT_XML;
    }
    else {
	contentType = HTTP_CONTENT_TEXT_PLAIN;
    }

    return contentType;
}

char *httpParser_GetContentTypeAsString(const HttpContentType_t contentType) {
    switch(contentType) {
	case HTTP_CONTENT_TEXT_HTML:
	    return "text/html";

	case HTTP_CONTENT_TEXT_XML:
	    return "text/xml";

	case HTTP_CONTENT_TEXT_PLAIN:
	default:
	    return "text/plain";
    }
}
