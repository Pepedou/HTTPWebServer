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

#include "connections.h"
#include "worker.h"

static ssize_t worker_ReceiveRequest(Connection_t *connection, uint8_t *buffer, size_t bufferLen);

void *worker_ServeRequest(void *params) {
    Connection_t *connection = (Connection_t *) params;
    uint8_t buffer[MAX_HTTP_REQUEST_SIZE];
    ssize_t receivedBytes;

    if (connection == NULL) pthread_exit(NULL);

    receivedBytes = worker_ReceiveRequest(connection, buffer, MAX_HTTP_REQUEST_SIZE);

    if (close(connection->socketFd) != 0) printf("Unable to close worker connection socket. Error: %d\n.", errno);

    pthread_exit(NULL);
}

static ssize_t worker_ReceiveRequest(Connection_t *connection, uint8_t *buffer, size_t bufferLen) {
    ssize_t receivedBytes;

    if ((receivedBytes = recv(connection->socketFd, buffer, bufferLen, 0)) == -1) {
        printf("Error receiving data from connection %d. Error: %d.\n", connection->socketFd, errno);
        return -1;
    }

    printf("%s\n", (char *) buffer);

    return receivedBytes;
}