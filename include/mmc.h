/*
mmc.h - A yet another mml to midi converter

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
#ifndef MMC_H
#define MMC_H

#define MMC_VERSION "0.0.2"

#define TRACK_NUM 16
#include "smf0.h"
#include "lex.h"

/*
 * a digit parameters
 */
typedef struct {
  int valid;
  int value;
  int sign;
} param_t;

typedef struct {
  int timestamp;
  int octave;
} mmc_brace_t;

#define MMC_NOTE_MAX 16
#define MMC_BRACKET_NEST_MAX 16
#define MMC_BRACE_NEST_MAX 16
/*
 * track information structure
 */
typedef struct {
  int currenttime;
  int octave;
  int gate;
  float length;
  int velocity;
  int brace_stack_index;
  mmc_brace_t brace_state[MMC_BRACE_NEST_MAX];
} track_t;


typedef struct {
  int position;
  int repeat;
  int current_repeat;
} mmc_bracket_t;


typedef struct {
  smf0_t *smf0;
  lex_t *lex;
  int lex_index;
  token_t *current_token; // just for reference
  track_t tracks[TRACK_NUM];
  int currenttrack;
  int error;
  int warning;
  int debug;
  int hide_warning;
  int simultones[128];
  int bracket_stack_index;
  mmc_bracket_t bracket_state[MMC_BRACKET_NEST_MAX];
  int bracket_skipping;
  int bracket_nest_level_during_skipping;
  int global_key;
  int global_keysig[7];// abcdefg
} mmc_t;

mmc_t *mmc_create();
void mmc_destroy(mmc_t *m);
void mmc_save(mmc_t *m, const char *filename);
int mmc_parse_mml_string(mmc_t *m, const char *mml, const char *filename);
int mmc_parse_mml_file(mmc_t *m, const char *mml_filename, const char *mid_filename);
void mmc_hide_warning(mmc_t *m);

#endif

