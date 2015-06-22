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
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#include "connections.h"
#include "listener.h"
#include "worker.h"

#define HTTP_PORT (8080)

static size_t numWorkers = 0;
static pthread_t threads[MAX_NUM_WORKERS];
static Connection_t connections[MAX_NUM_WORKERS];
static char connectionsStarted = 0,
        listenerStarted = 0,
        running = 1;

static int spawnNewWorker(int newConnectionSocket, struct sockaddr_in *newConnection, socklen_t newConnectionLength);

static void signalHandler(int signal);

static void cleanup();

static void exitOnError();


int main(int argc, char **argv) {
    int newConnectionFd;
    socklen_t newConnectionLength;
    struct sockaddr_in newConnectionSocket;

    memset(threads, 0, MAX_NUM_WORKERS);

    signal(SIGINT, signalHandler);

    if (connections_Init() != 0) exitOnError();
    else connectionsStarted = 1;

    if (listener_Init(INADDR_ANY, HTTP_PORT) != 0) exitOnError();
    else listenerStarted = 1;

    if (listener_BindToPort() != 0) exitOnError();

    if (listener_ListenToPort() != 0) exitOnError();

    while (running) {
        if (listener_AcceptNewConnections(&newConnectionFd, &newConnectionSocket, &newConnectionLength) == 0) {
            spawnNewWorker(newConnectionFd, &newConnectionSocket, newConnectionLength);
        }
    }

    cleanup();
    exit(EXIT_SUCCESS);
}

static int spawnNewWorker(int newConnectionSocket, struct sockaddr_in *newConnection, socklen_t newConnectionLength) {
    pthread_attr_t threadAttrs;
    size_t newWorkerIndex = numWorkers + 1;

    if (newWorkerIndex <= MAX_NUM_WORKERS) {
        numWorkers = newWorkerIndex;

        connections[newWorkerIndex].socketFd = newConnectionSocket;
        memcpy(&(connections[newWorkerIndex].socketAddress), &newConnection, sizeof(struct sockaddr_in));
        connections[newWorkerIndex].socketLength = newConnectionLength;

        pthread_attr_init(&threadAttrs);

        if (pthread_create(&threads[numWorkers + 1],
                           &threadAttrs, worker_ServeRequest, &connections[newWorkerIndex]) != 0) {
            printf("Failed to create new worker thread. Error: %d.\n", errno);
            return -1;
        }

        return 0;
    }
    else {
        printf("Unable to spawn new worker: Reached max number of worker threads [%d/%d].\n", (int) numWorkers,
               MAX_NUM_WORKERS);
        return -1;
    }
}

static void signalHandler(int signal) {
    switch (signal) {
        case SIGINT:
            printf("Interrupt received. Signaling process to stop...\n");
            running = 0;
            break;
        default:
            break;
    }
}

static void cleanup() {
    int i;

    for (i = 0; i < numWorkers; ++i) {
        if (threads[i] != 0) pthread_join(threads[i], NULL);
    }

    if (connectionsStarted) connections_Destroy();
    if (listenerStarted) listener_Destroy();
}

static void exitOnError() {
    cleanup();
    exit(EXIT_FAILURE);
}

