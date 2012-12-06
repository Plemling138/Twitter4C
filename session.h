/* Last Modified: March 13, 2012. by Plemling138 */
/* session.h   ---   SSL Sessions
   Copyright (C) 2012. by Plemling138 Plemling138 

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  
*/
   
#ifndef _SESSION_H_
#define _SESSION_H_

//SSL受信時の関数
int recvf(char* buf, int sz, void* vp);
//SSL送信時の関数
int sendf(char* buf, int sz, void* vp);
//送受信関数
int SSL_send_and_recv(char *send_buf, char *recv_buf);

#endif
