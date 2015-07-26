/*
 A simple HTTP Web Server.
 Copyright (C) 21/06/15  José Luis Valencia Herrera
 
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
#include <pthread.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "connections.h"
#include "worker.h"
#include "httpParser.h"

#define MAX_FILE_LEN (4096)
#define REPLY_MAX_LEN (MAX_FILE_LEN + 200)

#define NOT_FOUND_MSG "<html><head><title>Not found</title></head><body><h1>404 Not Found</h1></body></html>"
#define NOT_FOUND_MSG_LEN (sizeof (NOT_FOUND_MSG))

#define BAD_REQUEST_ERROR_MSG "<html><head><title>Bad request</title></head><body><h1>400 Bad Request</h1></body></html>"
#define BAD_REQUEST_ERROR_MSG_LEN (sizeof (BAD_REQUEST_ERROR_MSG))

#define INTERNAL_ERROR_MSG "<html><head><title>Server Error</title></head><body><h1>500 Internal Server Error</h1></body></html>"
#define INTERNAL_ERROR_MSG_LEN (sizeof (INTERNAL_ERROR_MSG))

#define NOT_IMPLEMENTED_ERROR_MSG "<html><head><title>Not implemented</title></head><body><h1>501 Action Not Implemented</h1></body></html>"
#define NOT_IMPLEMENTED_ERROR_MSG_LEN (sizeof (NOT_IMPLEMENTED_ERROR_MSG))

char worker_BaseDir[BASE_DIR_PATH_MAX_LEN] = ".";

static ssize_t receiveRequest(Connection_t *connection, uint8_t *buffer, size_t bufferLen);
static void executeActionForGETRequest(const HttpRequestData_t *requestData, char *replyBuffer, const size_t bufferLen);
static long fetchFile(char *fileBuffer, const size_t bufferLen, const char *path);

void *worker_ServeRequest(void *params) {
    Connection_t *connection = (Connection_t *) params;
    HttpRequestData_t requestData;
    uint8_t requestBuffer[MAX_HTTP_REQUEST_SIZE];
    char linesBuffer[MAX_REQUEST_LINES][MAX_HTTP_REQUEST_SIZE],
    replyBuffer[REPLY_MAX_LEN],
    *saveState = NULL;
    ssize_t receivedBytes;
    int linesParsed = 0;
    
    memset(requestBuffer, 0, sizeof (requestBuffer));
    memset(linesBuffer, 0, sizeof (linesBuffer));
    memset(replyBuffer, 0, sizeof (replyBuffer));
    
    if (connection == NULL) pthread_exit(NULL);
    
    receivedBytes = receiveRequest(connection, requestBuffer, MAX_HTTP_REQUEST_SIZE);
    
    if (!receivedBytes) {
        httpParser_GenerateHttpResponse(replyBuffer, REPLY_MAX_LEN, HTTP_ERROR_BAD_REQUEST, BAD_REQUEST_ERROR_MSG, BAD_REQUEST_ERROR_MSG_LEN);
    }
    else {
        linesParsed = httpParser_GetRequestLines((char *) requestBuffer, linesBuffer, MAX_REQUEST_LINES, &saveState);
        
        if (linesParsed) {
            if (!httpParser_ParseRequestLine(linesBuffer[0], &requestData, &saveState)) {
                httpParser_GenerateHttpResponse(replyBuffer, REPLY_MAX_LEN, HTTP_ERROR_BAD_REQUEST, BAD_REQUEST_ERROR_MSG, BAD_REQUEST_ERROR_MSG_LEN);
            }
            else {
                switch (requestData.requestMethod)
                {
                    case HTTP_REQM_GET:
                        executeActionForGETRequest(&requestData, replyBuffer, REPLY_MAX_LEN);
                        break;
                    default:
                        httpParser_GenerateHttpResponse(replyBuffer, REPLY_MAX_LEN, HTTP_ERROR_NOT_IMPLEMENTED, NOT_IMPLEMENTED_ERROR_MSG, NOT_IMPLEMENTED_ERROR_MSG_LEN);
                        break;
                }
            }
        }
    }
    
    if (write(connection->socketFd, replyBuffer, REPLY_MAX_LEN) != REPLY_MAX_LEN) {
        printf("Failed to write reply to socket. Error: %d.\n", errno);
    }
    
    if (close(connection->socketFd) != 0) printf("Unable to close worker connection socket. Error: %d\n.", errno);
    pthread_exit(NULL);
}

static ssize_t receiveRequest(Connection_t *connection, uint8_t *buffer, size_t bufferLen) {
    ssize_t receivedBytes;
    
    if ((receivedBytes = recv(connection->socketFd, buffer, bufferLen, 0)) == -1) {
        printf("Error receiving data from connection %d. Error: %d.\n", connection->socketFd, errno);
        return -1;
    }
    
    return receivedBytes;
}

static void executeActionForGETRequest(const HttpRequestData_t *requestData, char *replyBuffer, const size_t bufferLen) {
    long bytesRead = 0L;
    char fileBuffer[MAX_FILE_LEN];
    
    bytesRead = fetchFile(fileBuffer, MAX_FILE_LEN, requestData->requestURI);
    
    if (bytesRead == 0L) {
        httpParser_GenerateHttpResponse(replyBuffer, bufferLen, HTTP_ERROR_NOT_FOUND, NOT_FOUND_MSG, NOT_FOUND_MSG_LEN);
    }
    else if (bytesRead == -1L) {
        httpParser_GenerateHttpResponse(replyBuffer, bufferLen, HTTP_ERROR_SERVER_INTERNAL, INTERNAL_ERROR_MSG, INTERNAL_ERROR_MSG_LEN);
    }
    else {
        httpParser_GenerateHttpResponse(replyBuffer, bufferLen, HTTP_SUCCESS_OK, fileBuffer, bytesRead);
    }
    
    printf("%s\n", replyBuffer);
}

static long fetchFile(char *fileBuffer, const size_t bufferLen, const char *path) {
    long fileSize = 0L;
    size_t bytesRead = 0;
    FILE *file;
    char pathBuffer[BASE_DIR_PATH_MAX_LEN + MAX_REQUEST_URI_LEN];
    
    snprintf(pathBuffer, sizeof(pathBuffer), "%s/%s", worker_BaseDir, path);
    
    file = fopen(pathBuffer, "r");
    
    if (!file) return 0L;
    
    if (0 != fseek(file, 0L, SEEK_END)) {
        fclose(file);
        return 0L;
    }
    
    fileSize = ftell(file);
    
    if (fileSize == -1L) {
        fclose(file);
        return -1L;
    }
    
    rewind(file);
    
    if (bufferLen < fileSize) {
        fclose(file);
        return -1L;
    }
    
    bytesRead = fread(fileBuffer, fileSize, 1, file);
    
    fclose(file);
    
    if (!bytesRead) {
        return -1L;
    }
    
    return fileSize;
}