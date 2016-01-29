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

#ifndef INCLUDE_CROSS_SIGNALS_H_
#define INCLUDE_CROSS_SIGNALS_H_

extern volatile char signals_finishProc;

int signals_init(void);
void signals_stop(void);

#endif /* INCLUDE_CROSS_SIGNALS_H_ */
