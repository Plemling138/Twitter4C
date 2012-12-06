/* Last Modified: March 13, 2012. by Plemling138 */
/* extract.h   ---   Extract text
   Copyright (C) 2012. by Plemling138

   This source code is Free software, Free licence, but I couldn't support.
   このソースコードの使用については何のライセンスにも属さず、自由な使用・公開を許可します．
   使用に際しては自己責任でお願い致します。
   
*/
#ifndef _EXTRACT_H_
#define _EXTRACT_H_

int MakeSkipTable(char *key, int *skiptable, int length);
int ExtractQuery(char *text, char *key, char *buff);

#endif
