/* Last Modified: March 13, 2012. by Plemling138 */
/* twilib.h   ---   Generate OAuth request message for use Twitter API
   Copyright (C) 2012. by Plemling138

   This source code is Free software, Free licence, but I couldn't support.
   このソースコードの使用については何のライセンスにも属さず、自由な使用・公開を許可します．
   使用に際しては自己責任でお願い致します。
   
*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<netdb.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/time.h>
#include "hmac.h"
#include "base64.h"
#include "urlenc.h"
#include "extract.h"
#include "twilib.h"
#include "session.h"

int Twitter_GetRequestToken(struct Twitter_consumer_token *c, struct Twitter_request_token *r)
{
  int i;

  char buf[BUF_SIZE] = {'\0'};

  char oauth_signature_key[60] = {'\0'};

  struct timeval tv;
  char tstamp[20] = {'\0'};

  char nonce_tmp[20] = {'\0'};
  char nonce[20] = {'\0'};
  char nonce_urlenc[20] = {'\0'};

  char auth_tmpmsg[200] = {'\0'};//HMAC-SHA1でメッセージ作るときの仮メッセージ
  char auth_encmsg[250] = {'\0'};//HMAC-SHA1でメッセージ作るときの仮メッセージ(URL-Encoded)
  char encpath[60] = {'\0'}; //エンコードしたURL
  char auth_postmsg[250] = {'\0'};

  char tmp_token[200] = {'\0'};
  char tmp_secret[200] = {'\0'};

  char reqheader[300] = {'\0'};//POSTヘッダ

  char hmacmsg[40] = {'\0'};
  char b64msg[40] = {'\0'};

  char b64urlenc[50] = {'\0'};

  //署名キー
  sprintf(oauth_signature_key, "%s&", c->consumer_secret);

  //時間を取得してタイムスタンプと一意な値をセット
  gettimeofday(&tv, NULL);
  sprintf(tstamp, "%ld", tv.tv_sec);

  sprintf(nonce_tmp, "%ld", tv.tv_usec);
  base64_encode(nonce_tmp, strlen(nonce_tmp), nonce, 128);
  URLEncode(nonce, nonce_urlenc);

  //OAuth用メッセージ結合
  sprintf(auth_tmpmsg, "%s%s&%s%s&%s%s&%s%s&%s%s", OAUTH_CONSKEY, c->consumer_key, OAUTH_NONCE, nonce, OAUTH_SIGMETHOD, HMAC_SHA1, OAUTH_TSTAMP, tstamp,  OAUTH_VER, VER_1_0);
  URLEncode(auth_tmpmsg, auth_encmsg);
  URLEncode(REQUEST_TOKEN_URL, encpath);
  sprintf(auth_postmsg, "%s&%s&%s", MSG_POST, encpath, auth_encmsg);

  //シグネイチャ生成
  hmac_sha1(oauth_signature_key, strlen(oauth_signature_key), auth_postmsg, strlen(auth_postmsg), hmacmsg);

  //シグネイチャの文字数カウント
  //HMAC署名では署名中に\0記号が出てくることもあるため、今チェックしているポイントから3連続で
  //\0が続いた時にbreakするようにしている
  i=0;
  while(i < (40 - 3)) {
    if(hmacmsg[i] == '\0' && hmacmsg[i+1] == '\0' && hmacmsg[i+2] == '\0') break;
    i++;
  }

  //署名文字列をBASE64エンコード、さらにURLエンコード
  base64_encode(hmacmsg, i, b64msg, 128);
  URLEncode(b64msg, b64urlenc);

  //POST用メッセージ生成
  sprintf(reqheader, "%s %s?%s%s&%s%s&%s%s&%s%s&%s%s&%s%s %s\r\n\r\n", MSG_POST, REQUEST_TOKEN_URL, OAUTH_CONSKEY, c->consumer_key, OAUTH_NONCE, nonce_urlenc, OAUTH_SIG, b64urlenc, OAUTH_SIGMETHOD, HMAC_SHA1, OAUTH_TSTAMP, tstamp, OAUTH_VER, VER_1_0 , MSG_HTTP);

  //SSL通信で送受信
  SSL_send_and_recv(reqheader, buf);

  if(ExtractQuery(buf, "oauth_token=", tmp_token) < 0) return -8;
  if(ExtractQuery(buf, "oauth_token_secret=", tmp_secret) < 0) return -9;

  r->request_token = (char *)calloc((strlen(tmp_token))+1, sizeof(char));
  if(r->request_token == NULL) return -10;
  memcpy(r->request_token, tmp_token, strlen(tmp_token));

  r->request_secret = (char *)calloc((strlen(tmp_secret))+1, sizeof(char));
  if(r->request_secret == NULL) return -11;
  memcpy(r->request_secret, tmp_secret, strlen(tmp_secret));

  return 0;
}

int Twitter_GetAccessToken(struct Twitter_consumer_token *c, struct Twitter_request_token *r, struct Twitter_access_token *a)
{
  char buf[BUF_SIZE] = {'\0'};

  char oauth_signature_key[100] = {'\0'};

  struct timeval tv;
  char tstamp[20] = {'\0'};

  char nonce_tmp[20] = {'\0'};
  char nonce[20] = {'\0'};
  char nonce_urlenc[20] = {'\0'};

  char auth_tmpmsg[400] = {'\0'};//HMAC-SHA1でメッセージ作るときの仮メッセージ
  char auth_encmsg[450] = {'\0'};//HMAC-SHA1でメッセージ作るときの仮メッセージ(URL-Encoded)
  char encpath[80] = {'\0'}; //エンコードしたURL
  char auth_postmsg[450] = {'\0'};

  char tmp_token[200] = {'\0'};
  char tmp_secret[200] = {'\0'};

  char reqheader[500] = {'\0'};//POSTヘッダ

  char hmacmsg[40] = {'\0'};
  char b64msg[40] = {'\0'};

  char b64urlenc[50] = {'\0'};
  
  char tmp_usrid[20] = {0};
  char tmp_usrname[20] = {0};

  int i = 0;

  //署名キー
  sprintf(oauth_signature_key, "%s&%s", c->consumer_secret, r->request_secret);
  printf("osk = %s, %d bytes\n", oauth_signature_key, strlen(oauth_signature_key));

  //時間を取得してタイムスタンプと一意な値をセット
  gettimeofday(&tv, NULL);
  sprintf(tstamp, "%ld", tv.tv_sec);

  sprintf(nonce_tmp, "%ld", tv.tv_usec);
  base64_encode(nonce_tmp, strlen(nonce_tmp), nonce, 128);
  URLEncode(nonce, nonce_urlenc);

  //OAuth用メッセージ結合
  sprintf(auth_tmpmsg, "%s%s&%s%s&%s%s&%s%s&%s%s&%s%s&%s%s", OAUTH_CONSKEY, c->consumer_key, OAUTH_NONCE, nonce, OAUTH_SIGMETHOD, HMAC_SHA1, OAUTH_TSTAMP, tstamp,  OAUTH_TOKEN, r->request_token, OAUTH_VERIFIER, a->pin, OAUTH_VER, VER_1_0);
  URLEncode(auth_tmpmsg, auth_encmsg);
  URLEncode(ACCESS_TOKEN_URL, encpath);
  sprintf(auth_postmsg, "%s&%s&%s", MSG_POST, encpath, auth_encmsg);
  printf("auth_postmsg = %s, %d bytes\n", auth_postmsg, strlen(auth_postmsg));

  //シグネイチャ生成
  hmac_sha1(oauth_signature_key, strlen(oauth_signature_key), auth_postmsg, strlen(auth_postmsg), hmacmsg);

  //シグネイチャの文字数カウント
  //HMAC署名では署名中に\0記号が出てくることもあるため、今チェックしているポイントから3連続で
  //\0が続いた時にbreakするようにしている
  i=0;
  while(i < (40 - 3)) {
    if(hmacmsg[i] == 0 && hmacmsg[i+1] == 0 && hmacmsg[i+2] == 0) break;
    i++;
  }

  //署名文字列をBASE64エンコード、さらにURLエンコード
  base64_encode(hmacmsg, i, b64msg, 128);
  URLEncode(b64msg, b64urlenc);
  printf("b64msg = %s, %d bytes\n", b64urlenc, strlen(b64urlenc));

  //POST用メッセージ生成
  sprintf(reqheader, "%s %s?%s%s&%s%s&%s%s&%s%s&%s%s&%s%s&%s%s&%s%s %s\r\n\r\n", MSG_POST, ACCESS_TOKEN_URL, OAUTH_CONSKEY, c->consumer_key, OAUTH_NONCE, nonce_urlenc, OAUTH_SIG, b64urlenc, OAUTH_SIGMETHOD, HMAC_SHA1, OAUTH_TSTAMP, tstamp, OAUTH_TOKEN, r->request_token, OAUTH_VERIFIER, a->pin, OAUTH_VER, VER_1_0, MSG_HTTP);

  printf("RequestMsg=%s, %d bytes\n", reqheader, strlen(reqheader));

  SSL_send_and_recv(reqheader, buf);

  if(ExtractQuery(buf, "oauth_token=", tmp_token) < 0) return -8;
  if(ExtractQuery(buf, "oauth_token_secret=", tmp_secret) < 0) return -9;
  if(ExtractQuery(buf, "user_id=", tmp_usrid) < 0) return -10;
  if(ExtractQuery(buf, "screen_name=", tmp_usrname) < 0) return -11;

  a->access_token = (char *)calloc((strlen(tmp_token))+1, sizeof(char));
  if(a->access_token == NULL) return -12;
  memcpy(a->access_token, tmp_token, strlen(tmp_token));

  a->access_secret = (char *)calloc((strlen(tmp_secret))+1, sizeof(char));
  if(a->access_secret == NULL) return -13;
  memcpy(a->access_secret, tmp_secret, strlen(tmp_secret));

  a->user_id = (char *)calloc((strlen(tmp_usrid))+1, sizeof(char));
  if(a->user_id == NULL) return -14;
  memcpy(a->user_id, tmp_usrid, strlen(tmp_usrid));

  a->screen_name = (char *)calloc((strlen(tmp_usrname))+1, sizeof(char));
  if(a->screen_name == NULL) return -15;
  memcpy(a->screen_name, tmp_usrname, strlen(tmp_usrname));

  return 0;
}

int Twitter_UpdateStatus(struct Twitter_consumer_token *c,  struct Twitter_access_token *a, char *status)
{
  char buf[BUF_SIZE] = {'\0'};

  char oauth_signature_key[100] = {'\0'};

  struct timeval tv;
  char tstamp[20] = {'\0'};

  char nonce_tmp[20] = {'\0'};
  char nonce[20] = {'\0'};
  char nonce_urlenc[20] = {'\0'};

  char auth_tmpmsg[400 + (TWEET_LENGTH * 4)] = {'\0'};//HMAC-SHA1でメッセージ作るときの仮メッセージ
  char auth_encmsg[450 + (TWEET_LENGTH * 4)] = {'\0'};//HMAC-SHA1でメッセージ作るときの仮メッセージ(URL-Encoded)
  char encpath[80] = {'\0'}; //エンコードしたURL
  char auth_postmsg[600 + (TWEET_LENGTH * 4)] = {'\0'};
  char encstatus[(TWEET_LENGTH * 4)] = {'\0'};

  char reqheader[600 + (TWEET_LENGTH * 4)] = {'\0'};//POSTヘッダ

  char hmacmsg[40] = {'\0'};
  char b64msg[40] = {'\0'};

  char b64urlenc[50] = {'\0'};

  int i = 0;

  //署名キー
  sprintf(oauth_signature_key, "%s&%s", c->consumer_secret, a->access_secret);

  //時間を取得してタイムスタンプと一意な値をセット
  gettimeofday(&tv, NULL);
  sprintf(tstamp, "%ld", tv.tv_sec);

  sprintf(nonce_tmp, "%ld", tv.tv_usec);
  base64_encode(nonce_tmp, strlen(nonce_tmp), nonce, 128);
  URLEncode(nonce, nonce_urlenc);

  URLEncode(status, encstatus);

  //OAuth用メッセージ結合
  sprintf(auth_tmpmsg, "%s%s&%s%s&%s%s&%s%s&%s%s&%s%s&%s%s&%s%s", OAUTH_CONSKEY, c->consumer_key, OAUTH_NONCE, nonce_urlenc, OAUTH_SIGMETHOD, HMAC_SHA1, OAUTH_TSTAMP, tstamp,  OAUTH_TOKEN, a->access_token, OAUTH_VERIFIER, a->pin, OAUTH_VER, VER_1_0, STATUS, encstatus);
  URLEncode(auth_tmpmsg, auth_encmsg);
  URLEncode(STATUS_UPDATE_URL, encpath);
  sprintf(auth_postmsg, "%s&%s&%s", MSG_POST, encpath, auth_encmsg);

  //シグネイチャ生成
  hmac_sha1(oauth_signature_key, strlen(oauth_signature_key), auth_postmsg, strlen(auth_postmsg), hmacmsg);

  //シグネイチャの文字数カウント
  //HMAC署名では署名中に\0記号が出てくることもあるため、今チェックしているポイントから3連続で
  //\0が続いた時にbreakするようにしている
  i=0;
  while(i<300) {
    if(hmacmsg[i] == 0 && hmacmsg[i+1] == 0 && hmacmsg[i+2] == 0) break;
    i++;
  }

  //署名文字列をBASE64エンコード、さらにURLエンコード
  base64_encode(hmacmsg, i, b64msg, 128);
  URLEncode(b64msg, b64urlenc);

  //POST用メッセージ生成
  sprintf(reqheader, "%s %s?%s%s&%s%s&%s%s&%s%s&%s%s&%s%s&%s%s&%s%s&%s%s %s\r\n\r\n", MSG_POST, STATUS_UPDATE_URL, OAUTH_CONSKEY, c->consumer_key, OAUTH_NONCE, nonce_urlenc, OAUTH_SIG, b64urlenc, OAUTH_SIGMETHOD, HMAC_SHA1, OAUTH_TSTAMP, tstamp, OAUTH_TOKEN, a->access_token, OAUTH_VERIFIER, a->pin, OAUTH_VER, VER_1_0, STATUS, encstatus, MSG_HTTP);

  SSL_send_and_recv(reqheader, buf);

  return 0;
}
