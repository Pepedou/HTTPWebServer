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
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "listener.h"

static int socketFd;
static uint32_t ipAddress;
static uint16_t port;
static struct sockaddr_in sockAddrSelf, sockAddrOther;
static socklen_t sockLength;

int listener_Init(uint32_t ipAddressToUse, uint16_t portToUse) {
    ipAddress = ipAddressToUse;
    port = portToUse;
    sockLength = sizeof(sockAddrSelf);
    memset(&sockAddrSelf, 0, sockLength);
    memset(&sockAddrOther, 0, sockLength);

    if ((socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        printf("Unable to get TCP/IP socket file descriptor. Error: %d.\n", errno);
        return -1;
    }

    return 0;
}

int listener_BindToPort() {
    sockAddrSelf.sin_family = AF_INET;
    sockAddrSelf.sin_port = htons(port);
    sockAddrSelf.sin_addr.s_addr = htonl(ipAddress);

    if (bind(socketFd, (struct sockaddr *) &sockAddrSelf, sockLength) == -1) {
        printf("Unable to bind to port %d. Error: %d", port, errno);
        return -1;
    }

    return 0;
}

int listener_ListenToPort() {
    if (listen(socketFd, MAX_PENDING_CONNECTIONS) == -1) {
        printf("Unable to listen at %d:%d. Error: %d.", ipAddress, port, errno);
        return -1;
    }

    return 0;
}

int listener_AcceptNewConnections(int *newConnectionSocket, struct sockaddr_in *newConnection,
                                  socklen_t *newConnectionLength) {
    if ((*newConnectionSocket = accept(socketFd, (struct sockaddr *) newConnection, newConnectionLength)) == -1) {
        printf("Unable to accept new connection. Error: %d.\n", errno);
        return -1;
    }

    return 0;
}

int listener_Destroy() {
    if (close(socketFd) == -1) {
        printf("Unable to release the listener socket %d. Error: %d.\n", socketFd, errno);
        return -1;
    }

    return 0;
}
