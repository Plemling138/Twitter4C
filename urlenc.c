/* Last Modified: March 13, 2012. by Plemling138 */
/* urlenc.c   --- URL Encode function
   Copyright (C) 2012. by Plemling138

   This source code is Free software, Free licence, but I couldn't support.
   このソースコードの使用については何のライセンスにも属さず、自由な使用・公開を許可します．
   使用に際しては自己責任でお願い致します。
   
*/
/*
   Notice: This function replaces 'Space(0x20)' character to '%20'(NOT '+' symbol).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "urlenc.h"

int Encode_char(unsigned char c, char *c1, char *c2, char *c3)
{
  
  *c1 = '%';
  *c2 = ((c >> 4) < 0xA) ? ((c >> 4) | 0x30) : ((c >> 4) + 0x37);
  *c3 = ((c & 0x0F) < 0xA) ?  ((c & 0xF) | 0x30) : ((c & 0xF) + 0x37);
  
  return 0;
}

void URLEncode(char *str, char *urlenc)
{
  int pt = 0, i = 0;;
  
  //URLEncode
  for(i=0; i<strlen(str); i++) {
    /*if(str[i] == ' ') {
      urlenc[pt] = '+';
      pt++;
      }*/
    if( (str[i] >= 0x30 && str[i] <= 0x39) || (str[i] >= 0x41 && str[i] <= 0x5A) || (str[i] >= 0x61 && str[i] <= 0x7A) || str[i] == '.' || str[i] == '~' || str[i] == '-' || str[i] == '_') {
      urlenc[pt] = str[i];
      pt++;
    }
    else {
      Encode_char((unsigned char)str[i], &urlenc[pt], &urlenc[pt+1], &urlenc[pt+2]);
      pt += 3;
    }
  }
}
