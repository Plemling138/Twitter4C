/* Last Modified: March 13, 2012. by Plemling138 */
/* session.c   ---   Connect to Twitter use SSL sessions.
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

#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "ssl.h"
#include "twilib.h"
#include "session.h"

int sock = 0;

//SSL受信時の関数
int recvf(char* buf, int sz, void* vp)
{
  int got = recv(sock, buf, sz, 0);
  
  if (got == 0)
    return -2;  // IO_ERR_WANT_READ;
    
  return got;
}

//SSL送信時の関数
int sendf(char* buf, int sz, void* vp)
{
  int sent = send(sock, buf, sz, 0);
  if (sent == 0)
    return -2;  // IO_ERR_WANT_WRITE
  
  return sent;
}

int SSL_send_and_recv(char *send_buf, char *recv_buf)
{
  SSL* ssl;
  SSL_CTX* ctx;
  int ret = 0, read_size = 0;
  int i = 0;

  struct sockaddr_in addr;
  struct hostent *host = 0;

  //名前解決
  if((host = gethostbyname("api.twitter.com")) == NULL) {
    printf("ドメイン解決に失敗しました。\n");
    return -1;
  }
  printf("DNS OK.\n");

  //ポート番号などの設定
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = *((unsigned long *)host->h_addr_list[0]);
  addr.sin_port = htons(443);

  sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  //接続の確立
  if(connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    printf("接続できませんでした。\n");
    return -1;
  }

  //SSL接続の準備
  SSL_load_error_strings();
  SSL_library_init();
  
  ctx = SSL_CTX_new(SSLv3_client_method());
  if(ctx == 0) {
    return -2;
  }

  CyaSSL_SetIORecv(ctx, recvf);
  CyaSSL_SetIOSend(ctx, sendf);
  
  SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0);

  ssl = NULL;
  ssl = SSL_new(ctx);
  if(ssl == NULL) {
    return -3;
  }
  
  ret = SSL_set_fd(ssl, sock);
  if(ret == 0) {
    return -4;
  }

  //SSLセッション
  ret = SSL_connect(ssl);

  if(ret != 1) {
    return -5;
  }

  //リクエストの送信
  if(SSL_write(ssl, (void *)send_buf, strlen((void *)send_buf)) == -1) {
    return -6;
  }

  //レスポンス取得
  for(i=0; i<BUF_SIZE; i++) recv_buf[i] = 0;
  while((read_size = SSL_read(ssl, recv_buf, BUF_SIZE-1)) > 0) {
    fwrite(recv_buf, read_size, 1, stdout);
  }

  //SSLクローズ
  ret = SSL_shutdown(ssl);
  if(ret != 0) {
    return -7;
  }

  //close(*sck);
  close(sock);

  return 0;
}
