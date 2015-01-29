/*
dlog.c - A trivial debug log 

Copyright (c) 2015 sada.gussy at gmail dot com

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define MAXBUF 256

static char buf[MAXBUF];

static int enable_stdout = 0;

void dlog_clear(){
  memset(buf, 0, MAXBUF);
}

const char *dlog_get(){
  return buf;
}

void dlog_add(const char *format, ...){
  char buf2[MAXBUF];
  va_list arg;
  int len1;
  int len2;
  va_start(arg, format);
  vsnprintf(buf2, MAXBUF - 1, format, arg);
  va_end(arg);  
  if(enable_stdout){
    fprintf(stdout, "%s\n", buf2);
  }
  len1 = strlen(buf);
  len2 = strlen(buf2);
  if(len1 + len2 + 1 < MAXBUF - 1){
    strcat(buf, buf2);
    strcat(buf, "\r\n");
  }
}

void dlog_enable_stdout(){
  enable_stdout = 1;
}

void dlog_disable_stdout(){
  enable_stdout = 0;
}
/*
int main(int argc, char *argv[]){
  dlog_clear();
  dlog_add("test1 %d", 2);
  dlog_enable_stdout();
  dlog_add("test2");
  printf("%s", dlog_get());
}
*/
