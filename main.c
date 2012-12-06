/* Last Modified: March 13, 2012. by Plemling138 */
/* ctwt.c  ---   Tweet sample on C
   Copyright (C) 2012. by Plemling138
   
   This source code is Free software, Free licence, but I couldn't support.
   このソースコードの使用については何のライセンスにも属さず、自由な使用・公開を許可します．
   使用に際しては自己責任でお願い致します。
   
*/
/*
  Usage: 
  ./tweet [status]
  -Enable Multi-byte text
  -NOT package character count
  (Perhaps overflow buffer)
  
  -If Status-update success:
  return HTTP/1.0 200 and json data
  -If any:
  return HTTP Error code
*/ 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "twilib.h"

void RemoveReturn(char *str, int size)
{
  int i=0;
  while(str[i] != 0 && i != size) {
    if(str[i] == '\r' || str[i] == '\n') str[i] = 0;
    i++;
  }
}

int main(int argc, char *argv[])
{
  int errcode = 0;
  char pin[20] = {0};
  char *errcode_c = 0;
  FILE *access_token;

  char usr[20] = {0}, id[20] = {0}, token[150] = {0}, secret[150] = {0};
  
  if(argc != 2) {
    printf("Usage: ./tweet [Tweet text]\n");
    exit(0);
  }
  
  struct Twitter_consumer_token *c;
  struct Twitter_request_token *r;
  struct Twitter_access_token *a;

  c = (struct Twitter_consumer_token *) calloc(1, sizeof(struct Twitter_consumer_token));
  r = (struct Twitter_request_token *) calloc(1, sizeof(struct Twitter_request_token));
  a = (struct Twitter_access_token *) calloc(1, sizeof(struct Twitter_access_token));

  //取得したコンシューマキーとコンシューマシークレットをセットする
  char consumer_key[] = "hfehRiOKDhE3H0uc6L5A";
  char consumer_secret[] = "Bht16Jxl59tursk8NUCtb0Rf7nttWvnmSRSA02NJ9Y";

  c->consumer_key = consumer_key;
  c->consumer_secret = consumer_secret;

  //アクセストークンを保存する
  access_token = fopen("access_token.txt", "r");
  
  if(access_token == NULL) {
    //リクエストトークン取得
    if(errcode = Twitter_GetRequestToken(c, r), errcode < 0) {
      printf("リクエストトークンの取得でエラーが発生しています\n");
      printf("エラーコード:%d\n", errcode);
      exit(0);
    }
    
    printf("\n\n%s%s にアクセスしてPINコードを取得してください。\nPIN:", "https://api.twitter.com/oauth/authorize?oauth_token=", r->request_token);
    if(scanf("%19s%*[^\n]", pin) == -1) exit(1);
    a->pin = pin;

    if(errcode = Twitter_GetAccessToken(c, r, a), errcode < 0) {
      printf("アクセストークンの取得でエラーが発生しています\n");
      printf("エラーコード:%d\n", errcode);
      exit(0);
    }
    
    printf("\n\nようこそ %s(ID:%s) さん。\n", a->screen_name, a->user_id);
    
    access_token = fopen("access_token.txt", "w");
    if(access_token == NULL) {
      printf("Error\n");
      exit(0);
    }
    fprintf(access_token, "%s\n%s\n%s\n%s\n%s\n", a->screen_name, a->user_id, a->access_token, a->access_secret, a->pin);
    fclose(access_token);
  }

  else {
    if(errcode_c = fgets(usr, 20, access_token), errcode_c == NULL) return -1;
    if(errcode_c = fgets(id, 20, access_token), errcode_c == NULL) return -1;
    if(errcode_c = fgets(token, 150, access_token), errcode_c == NULL) return -1;
    if(errcode_c = fgets(secret, 150, access_token), errcode_c == NULL) return -1;
    if(errcode_c = fgets(pin, 20, access_token), errcode_c == NULL) return -1;
    
    RemoveReturn(usr, 20);
    RemoveReturn(id, 20);
    RemoveReturn(token, 150);
    RemoveReturn(secret, 150);
    RemoveReturn(pin, 20);
  
    a->user_id = usr;
    a->access_token = token;
    a->access_secret = secret;
    a->pin = pin;
    fclose(access_token);
  }
  
  //strcat(status, " ");  
  if(errcode = Twitter_UpdateStatus(c, a, argv[1]), errcode < 0) {
    printf("ツイートでエラーが発生しています\n");
    printf("エラーコード:%d\n", errcode);
    exit(0);
  }
  
  return 0;
}
