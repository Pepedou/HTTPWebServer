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

#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#include "connections.h"
#include "listener.h"
#include "worker.h"
#include "signalHandler.h"

#define DEFAULT_HTTP_PORT (8080)

static uint32_t numWorkers = 0;
static pthread_t threads[MAX_NUM_WORKERS];
static Connection_t connections[MAX_NUM_WORKERS];
static char connectionsStarted = 0, listenerStarted = 0;
static int selectedPort = DEFAULT_HTTP_PORT;

static void parseOpt(int argc, char **argv);

static int spawnNewWorker(int newConnectionSocket, struct sockaddr_in *newConnection, socklen_t newConnectionLength);

static void cleanup();

static void exitOnError();

int main(int argc, char **argv) {
    int newConnectionFd;
    socklen_t newConnectionLength;
    struct sockaddr_in newConnectionSocket;
    
    memset(threads, 0, MAX_NUM_WORKERS);
    
    parseOpt(argc, argv);

    if(signals_init() < 0) {
	printf("Unable to setup the signal handler.");
	return 1;
    }
    
    if (connections_Init() != 0) exitOnError();
    else connectionsStarted = 1;
    
    if (listener_Init(INADDR_ANY, selectedPort) != 0) exitOnError();
    else listenerStarted = 1;
    
    if (listener_BindToPort() != 0) exitOnError();
    
    if (listener_ListenToPort() != 0) exitOnError();
    
    while (!signals_finishProc) {
        if (listener_AcceptNewConnections(&newConnectionFd, &newConnectionSocket, &newConnectionLength) == 0) {
            spawnNewWorker(newConnectionFd, &newConnectionSocket, newConnectionLength);
        }
    }
    
    cleanup();
    exit(EXIT_SUCCESS);
}

static void parseOpt(int argc, char **argv) {
    DIR *dir = NULL;
    int opt, aux;
    
    while ((opt = getopt(argc, argv, "d:hp:")) != -1){
        switch (opt) {
            case 'd':
                dir = opendir(optarg);
                
                if (dir) {
                    strncpy(worker_BaseDir, optarg, BASE_DIR_PATH_MAX_LEN);
                    printf("Base directory:\t%s.\n", optarg);
                    closedir(dir);
                }
                else if (ENOENT == errno) {
                    printf("ERROR: Directory %s doesn't exist.\n", optarg);
                    exit(2);
                }
                else {
                    printf("ERROR: Unable to open directory %s. Error: %d.\n", optarg,errno);
                    exit(2);
                }
                break;
            case 'h':
                printf("Usage: %s [-d <baseDir>] [-h print this message and exit] [-p <listeningPort>]", argv[0]);
                exit(EXIT_SUCCESS);
                break;
            case 'p':
                aux = atoi(optarg);
                
                if (aux < 0 || aux > UINT16_MAX) {
                    printf("ERROR: Port range is 0 - %d.\n", UINT16_MAX);
                    exit(2);
                }
                
                selectedPort = aux;
                printf("Port:\t%d.\n", aux);
                break;
            case '?':
                printf("ERROR: Option %c not supported.\n", opt);
                break;
            case ':':
                printf("ERROR: Missing argument for option %c.\n", opt);
                exit(2);
                break;
        }
    }
}

static int spawnNewWorker(int newConnectionSocket, struct sockaddr_in *newConnection, socklen_t newConnectionLength) {
    pthread_attr_t threadAttrs;
    
    if (numWorkers < MAX_NUM_WORKERS) {
        connections[numWorkers].socketFd = newConnectionSocket;
        memcpy(&(connections[numWorkers].socketAddress), &newConnection, sizeof(struct sockaddr_in));
        connections[numWorkers].socketLength = newConnectionLength;
        
        pthread_attr_init(&threadAttrs);
        
        if (pthread_create(&threads[numWorkers],
                           &threadAttrs, worker_ServeRequest, &connections[numWorkers]) != 0) {
            printf("Failed to create new worker thread. Error: %d.\n", errno);
            return -1;
        }
        
        ++numWorkers;
        
        return 0;
    }
    else {
        printf("Unable to spawn new worker: Reached max number of worker threads [%d/%d].\n", (int) numWorkers,
               MAX_NUM_WORKERS);
        return -1;
    }
}

static void cleanup() {
    int i;
    
    for (i = 0; i < numWorkers; ++i) {
        if (threads[i] != 0) pthread_join(threads[i], NULL);
    }
    
    if (connectionsStarted) connections_Destroy();
    if (listenerStarted) listener_Destroy();
    
    printf("Cleanup finished.\n");
}

static void exitOnError() {
    cleanup();
    exit(EXIT_FAILURE);
}

