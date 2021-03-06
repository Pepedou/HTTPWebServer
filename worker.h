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

#ifndef HTTPWEBSERVER_WORKER_H
#define HTTPWEBSERVER_WORKER_H

#define MAX_NUM_WORKERS (1000)
#define BASE_DIR_PATH_MAX_LEN (200)

extern char worker_BaseDir[BASE_DIR_PATH_MAX_LEN];

void *worker_ServeRequest(void *params);

#endif //HTTPWEBSERVER_WORKER_H
