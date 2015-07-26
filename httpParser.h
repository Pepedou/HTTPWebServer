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

#ifndef HTTPWEBSERVER_HTTPPARSER_H
#define HTTPWEBSERVER_HTTPPARSER_H

#define MAX_REQUEST_URI_LEN (250)
#define MAX_HTTP_REQUEST_SIZE (8192)
#define MAX_REQUEST_LINES   (10)

typedef enum HTTP_REQUEST_METHODS_TAG
{
    HTTP_REQM_GET,
    HTTP_REQM_HEAD,
    HTTP_REQM_POST,
    HTTP_REQM_PUT,
    HTTP_REQM_DELETE,
    HTTP_REQM_CONNECT,
    HTTP_REQM_OPTIONS,
    HTTP_REQM_TRACE
} HttpRequestMethods;

typedef enum HTTP_STATUS_CODES_TAG
{
    HTTP_SUCCESS_OK = 200,
    HTTP_ERROR_BAD_REQUEST = 400,
    HTTP_ERROR_NOT_FOUND = 404,
    HTTP_ERROR_SERVER_INTERNAL = 500,
    HTTP_ERROR_NOT_IMPLEMENTED = 501
} HttpStatusCode_t;

typedef struct HTTP_REQUEST_DATA_TAG
{
    HttpRequestMethods requestMethod;
    char requestURI[MAX_REQUEST_URI_LEN];
    float httpVersion;
} HttpRequestData_t;

void httpParser_Init();
int httpParser_ParseRequestMethod(char *requestLine, char *requestMethod);
int httpParser_GetRequestLines(char *message, char linesBuffer[][MAX_HTTP_REQUEST_SIZE], const size_t linesBufferLen,
                               char **saveState);
int httpParser_ParseRequestLine(char *requestLine, HttpRequestData_t *requestData, char **saveState);
int httpParser_GenerateHttpResponse(char *replyBuffer, size_t bufferLen, const HttpStatusCode_t statusCode, const char *content, const int contentLength);

#endif //HTTPWEBSERVER_HTTPPARSER_H
