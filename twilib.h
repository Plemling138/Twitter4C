/* Last Modified: March 13, 2012. by Plemling138 */
/* twilib.h   ---   Twitter Library
   Copyright (C) 2012. by Plemling138

   This source code is Free software, Free licence, but I couldn't support.
   このソースコードの使用については何のライセンスにも属さず、自由な使用・公開を許可します．
   使用に際しては自己責任でお願い致します。
    
*/

#ifndef _TWILIB_H_
#define _TWILIB_H_

#define BUF_SIZE 3000
#define TWEET_LENGTH 150

#define MSG_POST "POST"
#define MSG_HTTP "HTTP/1.0"

#define OAUTH_CONSKEY   "oauth_consumer_key="
#define OAUTH_NONCE     "oauth_nonce="
#define OAUTH_SIG       "oauth_signature="
#define OAUTH_SIGMETHOD "oauth_signature_method="
#define OAUTH_TSTAMP    "oauth_timestamp="
#define OAUTH_VER       "oauth_version="
#define OAUTH_TOKEN     "oauth_token="
#define OAUTH_VERIFIER  "oauth_verifier="

#define VER_1_0   "1.0"
#define HMAC_SHA1 "HMAC-SHA1"
#define STATUS    "status="

#define REQUEST_TOKEN_URL "https://api.twitter.com/oauth/request_token"
#define ACCESS_TOKEN_URL  "https://api.twitter.com/oauth/access_token"
#define STATUS_UPDATE_URL "https://api.twitter.com/1.1/statuses/update.json"

struct Twitter_consumer_token
{
  char *consumer_key;
  char *consumer_secret;
};

struct Twitter_request_token
{
  char *request_token;
  char *request_secret;
};

struct Twitter_access_token
{
  char *access_token;
  char *access_secret;
  char *user_id;
  char *screen_name;
  char *pin;
};

int Twitter_GetRequestToken(struct Twitter_consumer_token *c, struct Twitter_request_token *r);
int Twitter_GetAccessToken(struct Twitter_consumer_token *c, struct Twitter_request_token *r, struct Twitter_access_token *a);
int Twitter_UpdateStatus(struct Twitter_consumer_token *c, struct Twitter_access_token *a, char *status);

#endif
