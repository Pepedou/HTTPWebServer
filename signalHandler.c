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

#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>

#include "signalHandler.h"

volatile char signals_finishProc = 0;

static pthread_t sigHandTID;
static sigset_t threadsSignalMask, originalSignalMask;

static void *signalHandlerThread(void *params);

int signals_init(void)
{
	sigemptyset(&threadsSignalMask);
	sigemptyset(&originalSignalMask);

	sigaddset(&threadsSignalMask, SIGINT);
	sigaddset(&threadsSignalMask, SIGTERM);

	/*Block signals in main thread and all the threads that are spawned from it*/
	if(0 != pthread_sigmask(SIG_BLOCK, &threadsSignalMask, &originalSignalMask))
	{
		printf("Failed to set the threads signal mask.");
		return -1;
	}

	if(0 != pthread_create(&sigHandTID, NULL, signalHandlerThread, NULL))
	{
		printf("Failed to create signal handler thread.");
		return -1;
	}

	return 0;
}

void signals_stop(void)
{
	pthread_join(sigHandTID, NULL);
	pthread_sigmask(SIG_SETMASK, &originalSignalMask, NULL);
}

static void *signalHandlerThread(void *params)
{
	int result, sigRecv;
	sigset_t signalSet;

	(void) params;

	sigemptyset(&signalSet);
	sigaddset(&signalSet, SIGINT);
	sigaddset(&signalSet, SIGTERM);

	do
	{
		result = sigwait(&signalSet, &sigRecv);
	}
	while(result == EINTR);

	switch(sigRecv)
	{
		case SIGINT:
		case SIGTERM:
			signals_finishProc = 1;
			break;
	}

	pthread_exit (NULL);
}
