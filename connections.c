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
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include "connections.h"

static ConnectionList_t connectionsList;

static int increaseConnectionsArraySize();

static inline char moreMemoryIsRequired();

int connections_Init() {
    connectionsList.numConnections = 0;

    if (increaseConnectionsArraySize() == -1) {
        printf("Failed to initialize connections module. Insufficient memory.\n");
        return -1;
    }

    return 0;
}

int connections_AddNewConnection(int socketFd, struct sockaddr_in socketAddress) {
    if (moreMemoryIsRequired()) {
        if (increaseConnectionsArraySize() == -1) {
            printf("Failed to create new connection. Insufficient memory.\n");
            return -1;
        }
    }
    ++(connectionsList.numConnections);
    connectionsList.connections[connectionsList.numConnections].socketFd = socketFd;
    memcpy(&(connectionsList.connections[connectionsList.numConnections].socketAddress), &socketAddress,
           sizeof(socketAddress));

    return 0;

}

static inline char moreMemoryIsRequired() {
    return (connectionsList.numConnections + 1) % DEFAULT_CONNECTIONS_SIZE == 0;
}

static int increaseConnectionsArraySize() {
    Connection_t *aux = NULL;

    aux = (Connection_t *) realloc(connectionsList.connections,
                                   sizeof(Connection_t) * (connectionsList.numConnections + DEFAULT_CONNECTIONS_SIZE));

    if (aux == NULL) {
        printf("Failed to assign memory for connections. Error: %d.\n", errno);
        return -1;
    }

    connectionsList.connections = aux;

    return 0;
}

void connections_Destroy() {
    if (connectionsList.connections != NULL) {
        free(connectionsList.connections);
    }
}