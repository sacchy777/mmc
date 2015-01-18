/*
lex.c - A simple lexical analizer for mmc

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
#include <string.h>
#include "lex.h"

const int kNumTokensDefault = 10;

enum {
  LEX_STATE_NORMAL = 0,
  LEX_STATE_COMMENT1,
  LEX_STATE_COMMENT2,
  LEX_STATE_LITERAL,
};

enum {
  kStateNormal,
  kStateComment1,
  kStateComment2,
  kStateLiteral,
};

typedef struct {
  int type;
  char *keyword;
} token_type_t;

static const token_type_t keywords[] = {
  {kTokenNoteA, "a"},
  {kTokenNoteB, "b"},
  {kTokenNoteC, "c"},
  {kTokenNoteD, "d"},
  {kTokenNoteE, "e"},
  {kTokenNoteF, "f"},
  {kTokenNoteG, "g"},
  {kTokenRest, "r"},
  {kTokenNatural, "*"},
  {kTokenDot, "."},
  {kTokenComma, ","},
  {kTokenOctaveUp, ">"},
  {kTokenOctaveDown, "<"},
  {kTokenPlus, "+"},
  {kTokenMinus, "-"},
  {kTokenTie, "^"},
};


/*
static const char macro_single_define_start = "$";
static const char macro_normal_define_start = "#";
static const char macro_start = "{";
static const char macro_end = "}";
*/
#define LEX_MAX_ERROR_STRING 256
char lex_errorstring[LEX_MAX_ERROR_STRING];

static token_t *token_create(int num){
  token_t *t;
  t = calloc(num, sizeof(token_t));
  if(!t){
    sprintf(lex_errorstring, "[LEX] Out of Memory at %s", __func__);
    return NULL;
  }
  return t;
}

static token_t *token_copy_create(token_t *old_tokens, int old_num, int new_num){
  int copy_num;
  token_t *new_tokens = token_create(new_num);
  if(!new_tokens){
    sprintf(lex_errorstring, "[LEX] Out of Memory at %s\n", __func__);
    return NULL;
  }
  copy_num = new_num > old_num ? old_num : new_num;
  memcpy(new_tokens, old_tokens, copy_num * sizeof(token_t));
  printf("reallocation from %d to %d\n", old_num, new_num);
  return new_tokens;
}

static void token_dump(token_t *t){
  printf("pos %d, len %d, type %d\n", t->pos, t->len, t->type);
}

lex_t *lex_create(){
  lex_t *lex;
  lex = calloc(1, sizeof(lex_t));
  if (!lex) {
    sprintf(lex_errorstring, "[LEX] Out of Memory at %s\n", __func__);
    return NULL;
    //    fprintf(stderr, "[ERROR]:%s %s:%d\n",__FILE__,__func__,__LINE__);
  }
  lex->num_tokens = kNumTokensDefault;
  lex->tokens = token_create(lex->num_tokens);
  lex->line = 0;
  return lex;
}

void lex_destroy(lex_t *lex){
  if(!lex) return;
  if(lex->body){
    free(lex->body);
  }
  if(lex->tokens){
    free(lex->tokens);
  }
  free(lex);
  return;
}

int lex_open(lex_t *lex, const char *filename){
  FILE *fp;
  fp = fopen(filename, "rb");
  if(!fp){
    sprintf(lex_errorstring, "[LEX] File %s not open at %s\n", filename, __func__);
    //    fprintf(stderr, "[ERROR]:%s %s:%d\n",__FILE__,__func__,__LINE__);
    return -1;
  }
  fseek(fp, 0, SEEK_END);
  lex->size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  lex->body = calloc(lex->size, sizeof(char) + 1); /* for 0 insert at last char */
  if(!lex->body) {
    sprintf(lex_errorstring, "[LEX] Out of memory at %s\n",  __func__);
    //    fprintf(stderr, "[ERROR]:%s %s:%d\n",__FILE__,__func__,__LINE__);
    return -1;
  }
  fread(lex->body, sizeof(char), lex->size, fp);
  return 0;
}

int lex_open_string(lex_t *lex, const char *mml){
  lex->size = strlen(mml);
  lex->body = calloc(lex->size, sizeof(char) + 1); /* for 0 insert at last char */
  if(!lex->body){
    sprintf(lex_errorstring, "[LEX] Out of memory at %s\n",  __func__);
    return -1;
  }
  memcpy(lex->body, mml, lex->size);
  return 0;
}

int lex_add_token(lex_t *lex, int pos, int len, int type, int line, int column){
  token_t *t;
  token_t *new_tokens;
  int new_num_tokens;
  if(lex->token_pos >= lex->num_tokens){
    new_num_tokens = lex->num_tokens * 2;
    new_tokens = token_copy_create(lex->tokens, lex->num_tokens, new_num_tokens);
    free(lex->tokens);
    if(!new_tokens){
      sprintf(lex_errorstring, "[LEX] Out of memory at %s\n",  __func__);
      return -1;
    }
    lex->tokens = new_tokens;
    lex->num_tokens = new_num_tokens;
  }
  t = &lex->tokens[lex->token_pos ++];
  t->pos = pos;
  t->len = len;
  t->valid = 1;
  t->type = type;
  return 0;
}

int lex_is_digit(lex_t *lex){
  char *p = lex->pos;
  int len = 0;
  while(*p != 0){
    if(*p >= '0' && *p <= '9'){
      len ++;
      p ++;
    }else{
      break;
    }
  }
  return len;
}

static const char *normal_delims = " \r\n\t|";
static const char *comment1_start = "/*";
static const char *comment1_end = "*/";
static const char *comment2_start = "//";
static const char *comment2_end = "\n";
static const char *literal_start = "{\"";
static const char *literal_end = "\"}";

int lex_parse_normal(lex_t *lex){
  int i;
  int len;
  int matched = 0;
  const char *keyword;
  const char *pair_list_string[] = {
    comment1_start, comment2_start, literal_start, 
  };
  const int pair_list_status[] = {
    kStateComment1, kStateComment2, kStateLiteral,
  };

  for(i = 0; i < sizeof(pair_list_string)/sizeof(char *); i ++){
    const char *start_string = pair_list_string[i];
    const int next_state = pair_list_status[i];
    len = strlen(start_string);
    if(strncmp(start_string, lex->pos, len) == 0){
      lex->pos += len;
      lex->column += len;
      lex->state = next_state;
      if(lex->debug) printf("state changed to by %s\n", start_string);
      return;
    }
  }

  /* parse white spcae and others that can be ignored */
  if(strchr(normal_delims, *(lex->pos)) != NULL){
    if(*lex->pos == '\n'){
      lex->line ++;
      lex->column = 0;
    }else{
      lex->column ++;
    }
    lex->pos ++;
    return 0;
  }

  if(lex->debug) printf("Parsing at line %d(%d) [%c(%d)]\n", lex->line, lex->column, *lex->pos, *lex->pos);

  /* parse digit */
  len = lex_is_digit(lex);
  if(len > 0){
    if(lex_add_token(lex, lex->pos - lex->body, len, kTokenDigit, lex->line, lex->column) != 0){
      return -1;
    }
    lex->pos += len;
    lex->column += len;
    if(lex->debug) printf("  matched digit %d\n", len);
    return 0;
  }

  /* parse keywords */
  for(i = 0; i < sizeof(keywords)/sizeof(token_type_t); i ++){
    keyword = keywords[i].keyword;
    len = strlen(keyword);
    if(strncmp(keyword, lex->pos, len) == 0){
      if(lex->debug) printf("  matched %s\n", keyword);
      if(lex_add_token(lex, lex->pos - lex->body, len, keywords[i].type, lex->line, lex->column) != 0){
	return -1;
      }
      lex->pos += len;
      lex->column += len;
      matched = 1;
      break;
    }
  }

  if(!matched){
    sprintf(lex_errorstring, "[LEX] Line %d(%d) : Undefined keyword %c\n", lex->line, lex->column, *lex->pos);
    lex->error = 1;
    if(lex->debug) puts(lex_errorstring);
    if(!lex->error_skip) lex->done = 1;
    lex->pos ++;
    lex->column ++;
  }
  return 0;
}

/*
Comment nesting is not supported or aleated.
*/
int lex_parse_pair(lex_t *lex){
  int len;
  int literal_len;
  char *start = lex->pos;
  int start_column = lex->column;
  int start_line = lex->line;
  const char *target_end;
  switch(lex->state){
  case kStateComment1:{
    len = strlen(comment1_end);
    target_end = comment1_end;
    break;
  }
  case kStateComment2:{
    len = strlen(comment2_end);
    target_end = comment2_end;
    break;
  }
  case kStateLiteral:{
    len = strlen(literal_end);
    target_end = literal_end;
    break;
  }
  }

  while(1){
    if(strncmp(target_end, lex->pos, len) == 0){
      lex->pos += len;
      lex->column += len;
      if(lex->state == kStateComment1){
	lex->line ++;
	lex->column = 0;
      }
      if(lex->state == kStateLiteral){
	literal_len = lex->pos - start - 2;
	if(lex->debug) printf("  matched literal pos %d size %d\n", (int)(start - lex->body), literal_len);
	if(lex_add_token(lex, start - lex->body, literal_len, kTokenLiteral, start_line, start_column) != 0){
	  return -1;
	}
      }
      if(lex->debug) printf("state changed to normal\n");
      lex->state = kStateNormal;
      break;
    }
    lex->pos ++;
    if(*lex->pos == '\n'){
      lex->line ++;
      lex->column = 0;
    }else{
      lex->column ++;
    }

    if(lex->pos >= lex->body + lex->size){
      lex->error = 1;
      sprintf(lex_errorstring, "[LEX] Line %d(%d) : comment unterminated\n", lex->line, lex->column);
      if(lex->debug) puts(lex_errorstring);
      if(!lex->error_skip) lex->done = 1;
      break;
    }
  }
  return 0;
}

void lex_dump_tokens(lex_t *lex){
  int i;
  token_t *t;
  printf("=== dump ===\n");
  for(i = 0; i < lex->num_tokens; i ++){
    t = &lex->tokens[i];
    if(!t->valid) continue;
    printf("[index %d ] pos %d, len %d : %c (%d)\n", i, t->pos, t->len, lex->body[t->pos], t->type);
  }
}

int lex_parse(lex_t *lex){
  int ret = 0;
  lex->done = 0;
  lex->pos = lex->body;
  lex->state = kStateNormal;
  while(!lex->done){
    switch(lex->state){
    case kStateNormal: ret = lex_parse_normal(lex); break;
    case kStateComment1: ret = lex_parse_pair(lex); break;
    case kStateComment2: ret = lex_parse_pair(lex); break;
    case kStateLiteral: ret = lex_parse_pair(lex); break;
    }
    if(ret != 0) return -1;
    if(lex->pos >= lex->body + lex->size) lex->done = 1;
  }
  return 0;
}

token_t *lex_get_token(lex_t *lex, int *iter, int peep){
  token_t *t = NULL;
  int index = 0;
  if(iter) index = *iter;
  while(1){
    if(lex->num_tokens <= index) break;
    t = &lex->tokens[index];
    if(peep != 0) index ++;
    if(iter) *iter = index;
    if(!t->valid) return NULL;
    break;
  }
  return t;
}

token_t *lex_peep_token(lex_t *lex, int *iter){
}

char *lex_get_error(){
  return lex_errorstring;
}

/*
int main(){
  lex_t *lex;
  lex = lex_create();
  lex->debug = 1;
  lex->error_skip = 0;
  //  lex_open(lex, "aa");
  lex_open_string(lex, "a12de><+-*|{\"\"}bc*.v+");
  printf("===target===\nlen(%d)\n%s\n===end===\n\n", lex->size, lex->body);
  lex_parse(lex);
  if(lex->error){
    printf("Error detected\n");
    puts(lex->errorstring);
  }
  lex_dump_tokens(lex);
  int i = 0;
  token_t *t;
  printf("dump one by one\n");
  while((t = lex_get_token(lex, &i, 0) )!= NULL){
    token_dump(t);
  }
  lex_destroy(lex);
}
*/

