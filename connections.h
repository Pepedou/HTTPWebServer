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


#ifndef HTTPWEBSERVER_CONNECTIONS_H
#define HTTPWEBSERVER_CONNECTIONS_H

#define DEFAULT_CONNECTIONS_SIZE (5)

typedef struct CONNECTION_T {
    int socketFd;
    struct sockaddr_in socketAddress;
    socklen_t socketLength;
} Connection_t;

typedef struct CONNECTION_LIST_T {
    Connection_t *connections;
    size_t numConnections;
} ConnectionList_t;

int connections_Init();

int connections_AddNewConnection(int socketFd, struct sockaddr_in socketAddress);

void connections_Destroy();

#endif //HTTPWEBSERVER_CONNECTIONS_H
