/*
lex.h - A simple lexical analizer for mmc

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

#ifndef MMC_LEX_H_
#define MMC_LEX_H_

enum {
  kTokenNone = 0,
  kTokenDigit,
  kTokenLiteral,

  kTokenNoteA,
  kTokenNoteB,
  kTokenNoteC,
  kTokenNoteD,
  kTokenNoteE,
  kTokenNoteF,
  kTokenNoteG,
  kTokenNoteNumber,
  kTokenNoteNumberMacro,

  kTokenRest,
  kTokenNatural,
  kTokenDot,
  kTokenComma,
  kTokenOctaveUp,
  kTokenOctaveDown,
  kTokenPlus,
  kTokenMinus,
  kTokenTie,

  kTokenTR,
  kTokenProgramChange,

  kTokenKey,
  
  kTokenOctave,
  kTokenVelocity,
  kTokenGate,
  kTokenLength,

  kTokenTempo,
  kTokenQuestion,

  kTokenRhythmMacroDefine,
  kTokenRhythmMacroReference,

  kTokenCC,

  kTokenTrackName,
  kTokenCopyright,

  kTokenBracketStart,
  kTokenBracketEnd,
  kTokenColon,

};

typedef struct {
  int valid;
  int type;
  int pos;
  int len;
  int line;
  int column;
  int ext_param;
} token_t;

typedef struct {
  int debug; /* output debug info on stdout when not 0 */
  int error_skip; /* keep going when error encountered */
  int error; /* indicates error occured during lex */

  int done;
  int state;
  char *body;
  char *pos;
  int line;
  int column;
  int size;
  int num_tokens;
  int token_pos;
  token_t *tokens;
  int macro[26*2];
  int rhythm_mode;
} lex_t;


lex_t *lex_create();
void lex_destroy(lex_t *lex);
int lex_open(lex_t *lex, const char *filename);
int lex_open_string(lex_t *lex, const char *mml);

token_t *lex_get_token(lex_t *lex, int *iter, int peep);
int lex_parse(lex_t *lex);
void lex_dump_tokens(lex_t *lex);

#endif // MMC_LEX_H_
