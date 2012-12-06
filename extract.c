/* Last Modified: March 13, 2012. by Plemling138 */
/* extract.c   ---   Extract text from response
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
#include "extract.h"

//文字列探索用スキップテーブル作成関数
int  MakeSkipTable(char *key, int *skiptable, int length) 
{
  int i;
  for(i=0;i<length-1;i++) {
    skiptable[i] = length-i-1;
  }

  skiptable[length-1] = length;

  for(i=length-2;i>=0;i--) {
    if(key[length-1] == key[i]) skiptable[length-1] = skiptable[i];
  }

  return 0;
}

//受信したクエリデータから指定されたメソッドの値を抽出
int ExtractQuery(char *text, char *key, char *buff) {
  int textlen = 0, keylen=0;
  int i = 0, j = 0, search = 0, line = 0, cur = 0;
  int skiptable[100] = {0};
  textlen = strlen(text);
  keylen = strlen(key);
  MakeSkipTable(key, skiptable, keylen);
  
  for(i=0;i<textlen;i++) {
    if(text[i] == '\n') {
      line++;
      cur = 0;
    }
    
    for(search=0;search<keylen;search++) {
      if(text[i+search] != key[search]) break;
      else;
    }
    
    if(search == keylen) {
      break;
    }
    else cur++;
  }
  
  if(i == textlen) return -1;

  i += keylen;
  while(text[i] != '&' && text[i] != 0 && text[i] != ' ' && text[i] != '\r' && text[i] != '\n') {
    buff[j] = text[i];
    i++;
    j++;
  }
  
  return 0;
}
