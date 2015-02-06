/*
mmc.c - A yet another mml to midi converter

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

#include "mmc_error.h"
#include "lex.h"
#include "mmc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dlog.h"

#define MMC_MAX_ERROR_STRING 256
char mmc_errorstring[MMC_MAX_ERROR_STRING];

/***************************************************************
 * 
 ****************************************************************/
static void track_init(track_t *t){
  memset(t, 0, sizeof(track_t));
  t->currenttime = 1;
  t->length = 1.0/4.0;
  t->octave = 5;
  t->gate = 80;
  t->velocity = 100;
}

/***************************************************************
 * 
 ****************************************************************/
mmc_t *mmc_create(){
  int i;
  memset(mmc_errorstring, 0, sizeof(char) * MMC_MAX_ERROR_STRING);
  mmc_t *m = calloc(sizeof(mmc_t), 1);
  if(!m){
    dlog_add(MMC_MSG_ERROR_OUTOFMEMORY, __func__);
    return NULL;
  }
  for(i = 0; i < TRACK_NUM; i ++){
    track_init(&m->tracks[i]);
  }
  m->smf0 = smf0_create();
  if(!m->smf0){
    dlog_add(MMC_MSG_ERROR_OUTOFMEMORY, __func__);
    mmc_destroy(m);
    return NULL;
  }
  m->lex = lex_create();
  if(!m->lex){
    dlog_add(MMC_MSG_ERROR_OUTOFMEMORY, __func__);
    mmc_destroy(m);
    return NULL;
  }
  m->currenttrack = 0;
  return m;
}

/***************************************************************
 * 
 ****************************************************************/
void mmc_destroy(mmc_t *m){
  if(m->lex) lex_destroy(m->lex);
  if(m->smf0) smf0_destroy(m->smf0);
  free(m);
}

/***************************************************************
 * 
 ****************************************************************/
void mmc_hide_warning(mmc_t *m){
  m->hide_warning = 1;
}

/***************************************************************
 * 
 ****************************************************************/
static void mmc_clear_simultones(mmc_t *m){
  memset(m->simultones, 0, sizeof(int) * 128);
}


/***************************************************************
 * 
 ****************************************************************/
static track_t *get_track(mmc_t *m){
  return &m->tracks[m->currenttrack];
}

/***************************************************************
 * 
 ****************************************************************/
static void mmc_add_note_number(mmc_t *m, int note, double length, param_t *gate, param_t *velocity, param_t *timing, int off_pending){
  track_t *t = get_track(m);
  smf0_t *s = m->smf0;
  int _gate = 0;
  int _velocity = 0;
  int _timing = 0;
  int line = m->current_token->line;
  int column = m->current_token->column;

  if(note < 0 || note > 127){
    dlog_add(MMC_MSG_ERROR_OUTOFRANGE, line+1, column+1, "note number", note);
    m->error = 1;	
	return;
  }

  if(off_pending == 1){
    m->simultones[note] = 1;
  }

  if(!gate->valid){
    _gate = t->gate;
  }else{
    if(gate->sign != 0){
      _gate = t->gate + gate->value * gate->sign;
    }else{
      _gate = gate->value;
    }
  }

  if(_gate < 0){
    dlog_add(MMC_MSG_WARNING_OUTOFRANGE, line+1, column+1, "gate", _gate, 0);
    m->warning = 1;
    _gate = 0;
  }
  if(_gate > 1000){
    dlog_add(MMC_MSG_WARNING_OUTOFRANGE, line+1, column+1, "gate", _gate, 1000);
    m->warning = 1;
    _gate = 1000;
  }

  if(!velocity->valid){
    _velocity = t->velocity;
  }else{
    if(velocity->sign != 0){
      _velocity = t->velocity + velocity->value * velocity->sign;
    }else{
      _velocity = velocity->value;
    }
  }

  if(_velocity < 0){
    dlog_add(MMC_MSG_WARNING_OUTOFRANGE, line+1, column+1, "velocity", _velocity, 0);
    m->warning = 1;
    _velocity = 0;
  }
  if(_velocity > 127){
    dlog_add(MMC_MSG_WARNING_OUTOFRANGE, line+1, column+1, "velocity", _velocity, 127);
    m->warning = 1;
    _velocity = 127;
  }


  /*
  if(!timing->valid){
    _timing = t->params[kMmcParamTiming];
  }else{
    if(timing->sign != 0){
      _timing = t->params[kMmcParamTiming] + timing->value;
    }else{
      _timing = timing->value;
    }
  }
  */

  //  int note = t->octave * 12 + key + sharp;
  //  int note = notenumber;
  int timenext = s->timebase * 4 * length;
  float length_f = length * _gate / 100;
  if(m->debug){
    printf("Adding note time %d note num %d length %f %f(gated) vel %d (offpend %d)\n", t->currenttime, note, length, length_f, _velocity, off_pending);
  }
  if(off_pending == 1){
    if(m->debug){printf("*pending*\n");}
    smf0_add_noteon(s, t->currenttime + _timing, m->currenttrack, note, _velocity);
  }else{
    if(m->debug){printf("*no pending*\n");}
    int j;
    smf0_add_note(s, t->currenttime + _timing, m->currenttrack, note, length_f, _velocity);
    for(j = 0; j < 128; j ++){
      if(m->simultones[j] == 0) continue;
      if(m->debug){printf("*off*%d\n", j);}
      smf0_add_noteoff(s, t->currenttime + _timing + 4 * s->timebase * length_f, m->currenttrack, j, _velocity);
    }
    mmc_clear_simultones(m);
  }
  if(off_pending == 0) t->currenttime += timenext;
}

static void mmc_add_note(mmc_t *m, int key, int sharp, double length, param_t *gate, param_t *velocity, param_t *timing, int off_pending){
  int note = get_track(m)->octave * 12 + key + sharp;
  mmc_add_note_number(m, note, length, gate, velocity, timing, off_pending);
}


/***************************************************************
 * 
 ****************************************************************/
static void mmc_add_program_change(mmc_t *m, int program){
  track_t *t = get_track(m);
  smf0_t *s = m->smf0;
  smf0_add_programchange(s, t->currenttime - 1, m->currenttrack, program);
}


/***************************************************************
 * 
 ****************************************************************/
static int mmc_token_match(mmc_t *m, int type){
  token_t *t;
  t = lex_get_token(m->lex, &m->lex_index, 0);
  if(!t) return 0;
  return t->type == type;
}

/***************************************************************
 * 
 ****************************************************************/
static void mmc_token_get_string(mmc_t *m, char **string, int *size){
  token_t *t;
  t = lex_get_token(m->lex, &m->lex_index, 0);
  *string = &m->lex->body[t->pos];
  *size = t->len;
}

/***************************************************************
 * 
 ****************************************************************/
static int mmc_token_get_digit_param(mmc_t *m, param_t *param){
  int size;
  char *p;
  char buf[256];
  memset(param, 0, sizeof(param_t));
  if(mmc_token_match(m, kTokenPlus)){
    param->sign = 1;
    m->lex_index ++;
  }else if(mmc_token_match(m, kTokenMinus)){
    param->sign = -1;
    m->lex_index ++;
  }
  if(mmc_token_match(m, kTokenDigit)){
    mmc_token_get_string(m, &p, &size);
    if(size > 255){
      return 0;
    }
    memset(buf, 0, sizeof(char)*256);
    memcpy(buf, p, size);
    param->valid = 1;
    param->value = atoi(buf);
    m->lex_index ++;
    return 1;
  }else{
    return 0;
  }
}


/***************************************************************
 * 
 ****************************************************************/
void mmc_save(mmc_t *m, const char *filename){
  smf0_save(m->smf0, filename);
}

/***************************************************************
 * 
 ****************************************************************/
static int mmc_parse_sharp(mmc_t *m, int *value){
  token_t *t;
  t = lex_get_token(m->lex, &m->lex_index, 0);
  if(!t) return 0;
  if(t->type == kTokenPlus){
    *value = 1;
    return 1;
  }
  if(t->type == kTokenMinus){
    *value = -1;
    return 1;
  }
  return 0;
}


/***************************************************************
 * 
 ****************************************************************/
static int mmc_parse_length_internal(mmc_t *m, double *length){
  int line = m->current_token->line;
  int column = m->current_token->column;
 
  int size;
  int digit;
  char *p;
  char buf[256];

  if(m->debug){printf("  parsing length\n");}
  if(mmc_token_match(m, kTokenDigit)){
    mmc_token_get_string(m, &p, &size);
    if(size > 9){
      dlog_add(MMC_MSG_ERROR_OUTOFRANGE, line+1, column+1, "length", 0);
	  m->error = 1;
      return 0;
    }
    memset(buf, 0, sizeof(char)*256);
    memcpy(buf, p, size);
    digit = atoi(buf);
    if(m->debug){printf("  match digit %d\n", digit);}
    if(digit <= 0){
      *length = 0;
      return 1;
    }
    *length = 1.0 / (double)digit;
    return 1;
  }else{
    return 0;
  }
}



/***************************************************************
 * 
 ****************************************************************/
static int mmc_parse_note(mmc_t *m){
  if(m->debug){
    printf("Parsing note %d\n", m->current_token->type);
  }
  token_t *t;


  int key = 0;
  int sharp = 0;
  int temp_int;
  double length = 0;
  double temp_double;
  param_t gate;
  param_t velocity;
  param_t timing;
  int off_pending = 0;
  memset(&gate, 0, sizeof(param_t));
  memset(&velocity, 0, sizeof(param_t));
  memset(&timing, 0, sizeof(param_t));
  
  t = lex_get_token(m->lex, &m->lex_index, 0);
  if(!t){
    if(m->debug){printf("parse note null\n");}
    return 0;
  }

  switch(t->type){
  case kTokenNoteC: key = 0; break;
  case kTokenNoteD: key = 2; break;
  case kTokenNoteE: key = 4; break;
  case kTokenNoteF: key = 5; break;
  case kTokenNoteG: key = 7; break;
  case kTokenNoteA: key = 9; break;
  case kTokenNoteB: key = 11; break;
  default: return 0;
  }
  m->lex_index ++;

  while(mmc_parse_sharp(m, &temp_int)){
    if(m->debug){printf(" sharp match %d \n", temp_int);}
    sharp += temp_int;
    m->lex_index ++;
  }

  if(mmc_parse_length_internal(m, &temp_double)){
    if(m->debug){printf(" length match %f\n", temp_double);}
    if(temp_double == 0){
      if(m->debug){printf("pended by len 0\n");}
      off_pending = 1;
    }
    if(m->debug){printf("non pended \n");}
    m->lex_index ++;
    if(off_pending == 0){
      length += temp_double;
      while(mmc_token_match(m, kTokenDot)){
	if(m->debug){printf(" dot match \n");}
	temp_double /= 2.0;
	length += temp_double;
	m->lex_index ++;
      }
    }
  }else{
    if(m->debug){printf(" no length match \n");}
    length += get_track(m)->length;
  }

  
  if(off_pending == 0){
    while(mmc_token_match(m, kTokenTie)){
      m->lex_index ++;
      if(m->debug){printf(" tie match\n");}
      if(mmc_parse_length_internal(m, &temp_double)){
	if(m->debug){printf(" length match %f\n", temp_double);}
	length += temp_double;
	m->lex_index ++;
	while(mmc_token_match(m, kTokenDot)){
	  if(m->debug){printf(" dot match \n");}
	  temp_double /= 2.0;
	  length += temp_double;
	  m->lex_index ++;
	}
      }else{
	length += get_track(m)->length;
      }
    }
  }

  if(mmc_token_match(m, kTokenComma)){
    m->lex_index ++;
    if(m->debug){printf(" gate comma match\n");}
    mmc_token_get_digit_param(m, &gate);
  }

  if(mmc_token_match(m, kTokenComma)){
    m->lex_index ++;
    if(m->debug){printf(" velocity comma match\n");}
    mmc_token_get_digit_param(m, &velocity);
  }

  /*
  if(mmc_token_match(m, kTokenComma)){
    if(m->debug){printf(" timing comma match\n");}
    m->lex_index ++;
    if(mmc_token_get_digit_param(m, &timing) == 0){
      m->lex_index ++;
    }
  }
  */
  if(m->debug){
    printf("note : key %d sharp %d len %f %d %d %d\n", 
	   key, sharp, length, gate.value, velocity.value, timing.value);
  }

  mmc_add_note(m, key, sharp, length, &gate, &velocity, &timing, off_pending);
  return 0;
}

/***************************************************************
 * 
 ****************************************************************/
static int mmc_parse_note_number(mmc_t *m, int is_macro){
  int line = m->current_token->line;
  int column = m->current_token->column;
  if(m->debug){
    printf("Parsing note number %d\n", m->current_token->type);
  }
  token_t *t;

  double length = 0;
  double temp_double;
  param_t gate;
  param_t velocity;
  param_t timing;
  param_t note_number;
  memset(&gate, 0, sizeof(param_t));
  memset(&velocity, 0, sizeof(param_t));
  memset(&timing, 0, sizeof(param_t));
  
  t = lex_get_token(m->lex, &m->lex_index, 0);
  if(!t){
    if(m->debug){printf("parse note null\n");}
    return 0;
  }
  m->lex_index ++;


  if(!is_macro){
    if(mmc_token_get_digit_param(m, &note_number)){
      if(note_number.sign == -1){
	dlog_add(MMC_MSG_ERROR_OUTOFRANGE, line+1, column+1, "Note number", note_number.sign * note_number.value);
	m->error = 1;
      }
    }else{
      //error
      if(m->debug){
	printf("no note number");
      }
      dlog_add(MMC_MSG_ERROR_PARAM_MISSING, line+1, column+1, "Note number");
      m->error = 1;
      return 0;
    }
  }else{
    note_number.valid = 1;
    note_number.value = t->ext_param;
  }

  if((!is_macro && mmc_token_match(m, kTokenComma)) || is_macro){
    if(!is_macro){m->lex_index ++;}
    if(m->debug){printf( "match note number comma\n");}
    if(mmc_parse_length_internal(m, &temp_double)){
      if(m->debug){printf(" length match %f\n", temp_double);}
      length += temp_double;
      m->lex_index ++;
      while(mmc_token_match(m, kTokenDot)){
	if(m->debug){printf(" dot match \n");}
	temp_double /= 2.0;
	length += temp_double;
	m->lex_index ++;
      }
    }else{
      if(m->debug){printf(" no length match \n");}
      length += get_track(m)->length;
    }
    while(mmc_token_match(m, kTokenTie)){
      m->lex_index ++;
      if(m->debug){printf(" tie match\n");}
      if(mmc_parse_length_internal(m, &temp_double)){
	if(m->debug){printf(" length match %f\n", temp_double);}
	length += temp_double;
	m->lex_index ++;
	while(mmc_token_match(m, kTokenDot)){
	  if(m->debug){printf(" dot match \n");}
	  temp_double /= 2.0;
	  length += temp_double;
	  m->lex_index ++;
	}
      }else{
	length += get_track(m)->length;
      }
    }
    if(mmc_token_match(m, kTokenComma)){
      m->lex_index ++;
      if(m->debug){printf(" gate comma match\n");}
      mmc_token_get_digit_param(m, &gate);
    }
    
    if(mmc_token_match(m, kTokenComma)){
      m->lex_index ++;
      if(m->debug){printf(" velocity comma match\n");}
      mmc_token_get_digit_param(m, &velocity);
    }
    
    /*
      if(mmc_token_match(m, kTokenComma)){
      if(m->debug){printf(" timing comma match\n");}
      m->lex_index ++;
      if(mmc_token_get_digit_param(m, &timing) == 0){
      m->lex_index ++;
      }
      }
    */
  }else{
    if(m->debug){printf( "no match note number length\n");}
    length += get_track(m)->length;
  }

  if(m->debug){
    printf("note : notenumber %d len %f %d %d %d\n", 
	   note_number.value, length, gate.value, velocity.value, timing.value);
  }
  
  mmc_add_note_number(m, note_number.value, length, &gate, &velocity, &timing, 0);
  return 0;
}


/***************************************************************
 * 
 ****************************************************************/

static void mmc_parse_rest(mmc_t *m){
  double length = 0;
  double temp_double = 0;
  int line = m->current_token->line;
  int column = m->current_token->column;
  int sign = 1;

  if(m->debug){printf("Parsing rest %d\n", m->current_token->type);}
  m->lex_index ++;

  if(mmc_token_match(m, kTokenMinus)){
    if(m->debug){printf(" r minus match\n");}
    sign = -1;
    m->lex_index ++;
  }

  if(mmc_parse_length_internal(m, &temp_double)){
    if(m->debug){printf(" r length match %f\n", temp_double);}
    length += temp_double;
    m->lex_index ++;
    while(mmc_token_match(m, kTokenDot)){
      if(m->debug){printf(" r dot match \n");}
      temp_double /= 2.0;
      length += temp_double;
      m->lex_index ++;
    }
  }else{
    length += get_track(m)->length;
    if(m->debug){printf(" r no length match %f\n", length);}
  }

  while(mmc_token_match(m, kTokenTie)){
    m->lex_index ++;
    if(m->debug){printf(" r tie match\n");}
    if(mmc_parse_length_internal(m, &temp_double)){
      if(m->debug){printf(" r length match %f\n", temp_double);}
      length += temp_double;
      m->lex_index ++;
      while(mmc_token_match(m, kTokenDot)){
	if(m->debug){printf(" r dot match \n");}
	temp_double /= 2.0;
	length += temp_double;
	m->lex_index ++;
      }
    }else{
      length += get_track(m)->length;
    }
  }

  int delta = sign == 1 ? m->smf0->timebase * 4 * length : -m->smf0->timebase * 4 * length;
  get_track(m)->currenttime += delta ;

  if(m->debug){printf(" rest %f(%d), current %d\n", length, delta, get_track(m)->currenttime);}

  if(get_track(m)->currenttime < 1){
    dlog_add(MMC_MSG_WARNING_OUTOFRANGE, line+1, column+1, "timepointer", get_track(m)->currenttime, 1);
    m->warning = 1;
    get_track(m)->currenttime = 1;
  }

}

/***************************************************************
 * 
 ****************************************************************/

static void mmc_parse_length(mmc_t *m){
  double length = 0;
  double temp_double = 0;
  //  int line = m->current_token->line;
  //  int column = m->current_token->column;

  if(m->debug){printf("Parsing length %d\n", m->current_token->type);}
  m->lex_index ++;

  if(mmc_parse_length_internal(m, &temp_double)){
    if(m->debug){printf(" def length match %f\n", temp_double);}
    length += temp_double;
    m->lex_index ++;
    while(mmc_token_match(m, kTokenDot)){
      if(m->debug){printf(" l dot match \n");}
      temp_double /= 2.0;
      length += temp_double;
      m->lex_index ++;
    }
  }else{
    length += get_track(m)->length;
    if(m->debug){printf(" l no length match %f\n", length);}
  }

  while(mmc_token_match(m, kTokenTie)){
    m->lex_index ++;
    if(m->debug){printf(" l tie match\n");}
    if(mmc_parse_length_internal(m, &temp_double)){
      if(m->debug){printf(" l length match %f\n", temp_double);}
      length += temp_double;
      m->lex_index ++;
      while(mmc_token_match(m, kTokenDot)){
	if(m->debug){printf(" l dot match \n");}
	temp_double /= 2.0;
	length += temp_double;
	m->lex_index ++;
      }
    }else{
      length += get_track(m)->length;
    }
  }
  get_track(m)->length = length;
}


/***************************************************************
 * 
 ****************************************************************/

static int mmc_parse_octave(mmc_t *m, int delta){
  int line = m->current_token->line;
  int column = m->current_token->column;
  if(m->debug){
    printf("Parsing octave change %d\n", m->current_token->type);
  }
  m->lex_index ++;
  int _octave = get_track(m)->octave + delta;
    if(_octave < 0){
      dlog_add(MMC_MSG_WARNING_OUTOFRANGE, line+1, column+1, "octave", _octave, 0);
      m->warning = 1;
      _octave = 0;
    }
    if(_octave > 9){
      dlog_add(MMC_MSG_WARNING_OUTOFRANGE, line+1, column+1, "octave", _octave, 10);
      m->warning = 1;
      _octave = 10;
    }
    get_track(m)->octave = _octave;
    return 0;
}

/***************************************************************
 * 
 ****************************************************************/

static int mmc_parse_octave_change(mmc_t *m){
  int line = m->current_token->line;
  int column = m->current_token->column;
  if(m->debug){
    printf("Parsing octave change %d\n", m->current_token->type);
  }
  m->lex_index ++;
  param_t param;
  if(mmc_token_get_digit_param(m, &param)){
    int _octave = param.sign == 0 ? param.value : get_track(m)->octave + param.value * param.sign;
    if(_octave < 0){
      dlog_add(MMC_MSG_WARNING_OUTOFRANGE, line+1, column+1, "octave", _octave, 0);
      m->warning = 1;
      _octave = 0;
    }
    if(_octave > 10){
      dlog_add(MMC_MSG_WARNING_OUTOFRANGE, line+1, column+1, "octave", _octave, 10);
      m->warning = 1;
      _octave = 10;
    }
    get_track(m)->octave = _octave;
    return 0;
  }else{
    dlog_add(MMC_MSG_ERROR_PARAM_MISSING, line+1, column+1, "Octave value");
    m->error = 1;
    return -1;
  }
}

/***************************************************************
 * 
 ****************************************************************/
static int mmc_parse_velocity(mmc_t *m){
  int line = m->current_token->line;
  int column = m->current_token->column;
  if(m->debug){
    printf("Parsing velocity change %d\n", m->current_token->type);
  }
  m->lex_index ++;
  param_t param;
  if(mmc_token_get_digit_param(m, &param)){
    int _velocity = param.sign == 0 ? param.value : get_track(m)->velocity + param.value * param.sign;
    if(_velocity < 0){
      dlog_add(MMC_MSG_WARNING_OUTOFRANGE, line+1, column+1, "velocity", _velocity, 0);
      m->warning = 1;
      _velocity = 0;
    }
    if(_velocity > 127){
      dlog_add(MMC_MSG_WARNING_OUTOFRANGE, line+1, column+1, "velocity", _velocity, 127);
      m->warning = 1;
      _velocity = 127;
    }
    get_track(m)->velocity = _velocity;
    return 0;
  }else{
    dlog_add(MMC_MSG_ERROR_PARAM_MISSING, line+1, column+1, "Velocity value");
    m->error = 1;
    return -1;
  }
}
/***************************************************************
 * 
 ****************************************************************/
static int mmc_parse_tempo(mmc_t *m){
  int line = m->current_token->line;
  int column = m->current_token->column;
  if(m->debug){
    printf("Parsing tempo change %d\n", m->current_token->type);
  }
  m->lex_index ++;
  param_t param;
  if(mmc_token_get_digit_param(m, &param)){
    if(param.sign != 0){
      dlog_add(MMC_MSG_WARNING_SIGN_IGNORED, line+1, column+1, "Tempo");
      m->warning = 1;
    }
    if(param.value < 1){
      dlog_add(MMC_MSG_WARNING_OUTOFRANGE, line+1, column+1, "Tempo", param.value, 1);
      m->warning = 1;
    }
    if(param.value > 999){
      dlog_add(MMC_MSG_WARNING_OUTOFRANGE, line+1, column+1, "Tempo", param.value, 999);
      m->warning = 1;
    }
    track_t *t = get_track(m);
    smf0_t *s = m->smf0;
    smf0_add_tempo(s, t->currenttime - 1, param.value);
    return 0;
  }else{
    dlog_add(MMC_MSG_ERROR_PARAM_MISSING, line+1, column+1, "Tempo value");
	m->error = 1;
    return -1;
  }
}

/***************************************************************
 * mmc_parse_track
 ****************************************************************/

static int mmc_parse_gate(mmc_t *m){
  int line = m->current_token->line;
  int column = m->current_token->column;
  if(m->debug){
    printf("Parsing gate change %d\n", m->current_token->type);
  }
  m->lex_index ++;
  param_t param;
  if(mmc_token_get_digit_param(m, &param)){
    int _gate = param.sign == 0 ? param.value : get_track(m)->gate + param.value * param.sign;
    if(_gate < 0){
      dlog_add(MMC_MSG_WARNING_OUTOFRANGE, line+1, column+1, "gate", _gate, 0);
      m->warning = 1;
      _gate = 0;
    }
    if(_gate > 1000){
      dlog_add(MMC_MSG_WARNING_OUTOFRANGE, line+1, column+1, "gate", _gate, 1000);
      m->warning = 1;
      _gate = 1000;
    }
    get_track(m)->gate = _gate;
    return 0;
  }else{
    dlog_add(MMC_MSG_ERROR_PARAM_MISSING, line+1, column+1, "gate number");
    m->error = 1;
    return -1;
  }
}

/***************************************************************
 * mmc_parse_cc
 ****************************************************************/

static int mmc_parse_cc(mmc_t *m){
  int line = m->current_token->line;
  int column = m->current_token->column;
  if(m->debug){
    printf("Parsing control change %d\n", m->current_token->type);
  }
  m->lex_index ++;

  param_t param1;
  param_t param2;

  if(mmc_token_get_digit_param(m, &param1) == 0){
    dlog_add(MMC_MSG_ERROR_PARAM_MISSING, line+1, column+1, "Control number");
    m->error = 1;
    return -1;
  }else{
    if(param1.value < 0 || param1.value > 127){
      dlog_add(MMC_MSG_ERROR_OUTOFRANGE, line+1, column+1, "y", param1.value);
      m->error = 1;
      return -1;
    }
  }

  if(mmc_token_match(m, kTokenComma) == 0){
    dlog_add(MMC_MSG_ERROR_SYNTAX, line+1, column+1, "Control Change");
    m->error = 1;
    return -1;
  }
  m->lex_index ++;
  if(mmc_token_get_digit_param(m, &param2) == 0){
    dlog_add(MMC_MSG_ERROR_PARAM_MISSING, line+1, column+1, "Control value");
    m->error = 1;
    return -1;
  }else{
    if(param2.value < 0 || param2.value > 127){
      dlog_add(MMC_MSG_ERROR_OUTOFRANGE, line+1, column+1, "y", param2.value);
      m->error = 1;
      return -1;
    }
  }
  
  smf0_add_controlchange(m->smf0, 
			 get_track(m)->currenttime - 1, 
			 m->currenttrack,
			 param1.value, 
			 param2.value);
  
  return 0;
}



/***************************************************************
 * mmc_parse_track
 ****************************************************************/
static int mmc_parse_track(mmc_t *m){
  int line = m->current_token->line;
  int column = m->current_token->column;
  if(m->debug){
    printf("Parsing track change %d\n", m->current_token->type);
  }
  m->lex_index ++;

  param_t param;
  if(mmc_token_get_digit_param(m, &param)){
    int tracknum = param.sign == 0 ? param.value : param.value * param.sign;
    //    printf(" Parsing track to %d\n", tracknum);
    if(tracknum < 1 || tracknum > 16){
      dlog_add(MMC_MSG_ERROR_OUTOFRANGE, line+1, column+1, "TR", tracknum);
      m->error = 1;
      return -1;
    }
    m->currenttrack = param.value - 1;
    return 0;
  }else{
    dlog_add(MMC_MSG_ERROR_PARAM_MISSING, line+1, column+1, "Track number");
    m->error = 1;
    return -1;
  }
}

/***************************************************************
 * mm_parse_program_change
 ****************************************************************/
static int mmc_parse_program_change(mmc_t *m){
  int line = m->current_token->line;
  int column = m->current_token->column;
  if(m->debug){
    printf("Parsing prog change %d\n", m->current_token->type);
  }
  m->lex_index ++;

  param_t param;
  if(mmc_token_get_digit_param(m, &param)){
    if(param.sign != 0 || param.value < 1 || param.value > 128){
      dlog_add(MMC_MSG_ERROR_OUTOFRANGE, line+1, column+1, "@", param.value);
      m->error = 1;
      return -1;
    }
    if(m->debug){printf("added %d\n", param.value-1);}
    mmc_add_program_change(m, param.value - 1);
    return 0;
  }else{
    dlog_add(MMC_MSG_ERROR_PARAM_MISSING, line+1, column+1, "program number");
    m->error = 1;
    return -1;
  }
}

/***************************************************************
 * mm_parse_meta
 ****************************************************************/
static int mmc_parse_meta_long(mmc_t *m, int meta_type){
  int line = m->current_token->line;
  int column = m->current_token->column;
  if(m->debug){
    printf("Parsing trackname %d\n", m->current_token->type);
  }
  m->lex_index ++;
  //void smf0_add_meta_long(smf0_t *s, int absolute_time, int meta_type, char *meta_string, int meta_len){
  if(mmc_token_match(m, kTokenLiteral)){
    if(m->debug){printf("match lietral \n");}
    token_t *t = lex_get_token(m->lex, &m->lex_index, 0);
    smf0_add_meta_long(m->smf0, 
		       get_track(m)->currenttime - 1, 
		       meta_type,
		       &m->lex->body[t->pos],
		       t->len);
    m->lex_index ++;
    return 0;
  }else{
    dlog_add(MMC_MSG_ERROR_PARAM_MISSING, line+1, column+1, "Meta String");
	m->error = 1;
    return -1;
  }
}

/***************************************************************
 * mm_parse_question
 ****************************************************************/
static int mmc_parse_question(mmc_t *m){
  //  int line = m->current_token->line;
  //  int column = m->current_token->column;
  if(m->debug){
    printf("Parsing question %d\n", m->current_token->type);
  }
  m->lex_index ++;
  smf0_add_question(m->smf0, get_track(m)->currenttime - 1);
  return 0;
}

/***************************************************************
 * mm_parse_bracket_start
 ****************************************************************/
static int mmc_parse_bracket_start(mmc_t *m){
  int line = m->current_token->line;
  int column = m->current_token->column;
  int repeat = 2;
  param_t param;
  if(m->debug){
    printf("Parsing bracket start %d\n", m->current_token->type);
  }

  if(m->bracket_skipping){
    m->bracket_nest_level_during_skipping ++;
    m->lex_index ++;
    return 0;
  }

  if(m->bracket_stack_index == MMC_BRACKET_NEST_MAX){
    dlog_add(MMC_MSG_ERROR_BRACKET_NEST_MAX, line+1, column+1);
    //    if(m->debug){printf("bracket start max nest\n");}
    m->error = 1;
  }
  m->lex_index ++;
  if(mmc_token_get_digit_param(m, &param)){
    if(m->debug){printf("repeat %d\n", param.value);}
    if(param.sign == -1 || param.value > 100 || param.value == 0){
      dlog_add(MMC_MSG_ERROR_BRACKET_REPEAT_ILLEGAL, line+1, column+1);
      m->error = 1;
    }
    repeat = param.value;
  }

  mmc_bracket_t *b = &m->bracket_state[m->bracket_stack_index ++];
  if(m->debug){printf("   pushed %d\n", m->lex_index);}
  b->position = m->lex_index;
  b->repeat = repeat;
  b->current_repeat = 1;
  return 0;
}

/***************************************************************
 * mm_parse_bracket_end
 ****************************************************************/
static int mmc_parse_bracket_end(mmc_t *m){
  //  int line = m->current_token->line;
  //  int column = m->current_token->column;
  if(m->debug){
    printf("Parsing bracket end %d\n", m->current_token->type);
  }

  if(m->bracket_nest_level_during_skipping > 0){
    m->bracket_nest_level_during_skipping --;
    m->lex_index ++;
    return 0;
  }

  if(m->bracket_stack_index == 0){
    // error!
    if(m->debug){printf("bracket end stack empty\n");}
    m->error = 1;
    return -1;
  }
  mmc_bracket_t *b = &m->bracket_state[m->bracket_stack_index - 1];
  if(b->current_repeat == b->repeat){
    m->bracket_stack_index --;
    m->lex_index ++;
    m->bracket_skipping = 0;
  }else{
    b->current_repeat ++;
    m->lex_index = b->position;
  if(m->debug){printf("   poped %d\n", m->lex_index);}
  }
  return 0;
}

/***************************************************************
 * mm_parse_colon
 ****************************************************************/
static int mmc_parse_colon(mmc_t *m){
  if(m->debug){
    printf("Parsing colon %d\n", m->current_token->type);
  }
  int line = m->current_token->line;
  int column = m->current_token->column;
  if(m->bracket_skipping){
    if(m->bracket_nest_level_during_skipping == 0){
      // found second colon at same nest level. warning !
      dlog_add(MMC_MSG_ERROR_UNEXPECTED_TOKEN, line+1, column+1, "colon");
      m->error = 1;
    }
    m->lex_index ++; 
    return 0;
  }
  if(m->bracket_stack_index == 0){
    // error!
    if(m->debug){printf("colon at stack 0\n");}
    dlog_add(MMC_MSG_ERROR_UNEXPECTED_TOKEN, line+1, column+1, "colon");
    m->error = 1;
    return -1;
  }
  mmc_bracket_t *b = &m->bracket_state[m->bracket_stack_index - 1];
  if(b->current_repeat == b->repeat){
    m->bracket_skipping = 1;
    m->bracket_nest_level_during_skipping = 0;
  }
  m->lex_index ++;
  return 0;
}

/***************************************************************
 * mmc_parse
 ****************************************************************/

static void mmc_parse(mmc_t *m){

  while((m->current_token = lex_get_token(m->lex, &m->lex_index, 0) )!= NULL){
    switch(m->current_token->type){

    case kTokenBracketStart:
      mmc_parse_bracket_start(m);
      break;

    case kTokenBracketEnd:
      mmc_parse_bracket_end(m);
      break;

    case kTokenColon:
      mmc_parse_colon(m);
      break;

    case kTokenQuestion:
      if(m->bracket_skipping){m->lex_index ++; break;}
      mmc_parse_question(m);
      break;

    case kTokenTrackName:
      if(m->bracket_skipping){m->lex_index ++; break;}
      mmc_parse_meta_long(m, 0x03);
      break;

    case kTokenCopyright:
      if(m->bracket_skipping){m->lex_index ++; break;}
      mmc_parse_meta_long(m, 0x02);
      break;

    case kTokenNoteA:
    case kTokenNoteB:
    case kTokenNoteC:
    case kTokenNoteD:
    case kTokenNoteE:
    case kTokenNoteF:
    case kTokenNoteG:
      if(m->bracket_skipping){m->lex_index ++; break;}
      mmc_parse_note(m);
      break;

    case kTokenRest:
      if(m->bracket_skipping){m->lex_index ++; break;}
      mmc_parse_rest(m);
      break;

    case kTokenNoteNumber:
      if(m->bracket_skipping){m->lex_index ++; break;}
      mmc_parse_note_number(m, 0);
      break;

    case kTokenNoteNumberMacro:
      if(m->bracket_skipping){m->lex_index ++; break;}
      mmc_parse_note_number(m, 1);
      break;

    case kTokenOctaveUp:
      if(m->bracket_skipping){m->lex_index ++; break;}
      mmc_parse_octave(m, 1);
      break;

    case kTokenOctaveDown:
      if(m->bracket_skipping){m->lex_index ++; break;}
      mmc_parse_octave(m, -1);
      break;

    case kTokenOctave:
      if(m->bracket_skipping){m->lex_index ++; break;}
      mmc_parse_octave_change(m);
      break;

    case kTokenLength:
      if(m->bracket_skipping){m->lex_index ++; break;}
      mmc_parse_length(m);
      break;

    case kTokenVelocity:
      if(m->bracket_skipping){m->lex_index ++; break;}
      mmc_parse_velocity(m);
      break;

    case kTokenGate:
      if(m->bracket_skipping){m->lex_index ++; break;}
      mmc_parse_gate(m);
      break;

    case kTokenTR:
      if(m->bracket_skipping){m->lex_index ++; break;}
      mmc_parse_track(m);
      break;

    case kTokenCC:
      if(m->bracket_skipping){m->lex_index ++; break;}
      mmc_parse_cc(m);
      break;

    case kTokenProgramChange:
      if(m->bracket_skipping){m->lex_index ++; break;}
      mmc_parse_program_change(m);
      break;

    case kTokenTempo:
      if(m->bracket_skipping){m->lex_index ++; break;}
      mmc_parse_tempo(m);
      break;

    default:
      if(m->bracket_skipping){m->lex_index ++; break;}
      dlog_add(MMC_MSG_ERROR_UNSUPPORTED_TOKEN,
	       m->current_token->line+1, m->current_token->column+1, 
	       m->lex->body[m->current_token->pos]);
      m->error = 1;
      break;
    } // end of switch
    if(m->error){
      break;
    }
  }
  if(m->bracket_stack_index != 0){
    dlog_add(MMC_MSG_ERROR_BRACKET_NUMBER_MISMATCH);
    m->error = 1;
  }
  if(m->error){
    dlog_add(MMC_MSG_ERROR_CONVERT_FAILED);
  }
}

/***************************************************************
 * mmc_parse_mml_string
 ****************************************************************/
int mmc_parse_mml_string(mmc_t *m, const char *mml, const char *filename){
  if(lex_open_string(m->lex, mml) != 0) return -1;
  if(lex_parse(m->lex) != 0) return -1;
  if(m->debug) lex_dump_tokens(m->lex);
  mmc_parse(m);
  if(m->debug) smf0_dump(m->smf0);
  if(filename) smf0_save(m->smf0, filename);
  if(m->error == 1) return -1;
  dlog_add("test0.0.2\n");
  return 0;
}

/***************************************************************
 * mmc_parse_mml_file
 ****************************************************************/
int mmc_parse_mml_file(mmc_t *m, const char *mml_filename, const char *mid_filename){
  lex_open(m->lex, mml_filename);
  if(lex_parse(m->lex) != 0) return -1;
  if(m->debug) lex_dump_tokens(m->lex);
  mmc_parse(m);
  if (mid_filename) smf0_save(m->smf0, mid_filename);
  if(m->error == 1) return -1;
  return 0;
}

/***************************************************************
 * main
 ****************************************************************/
/*
int main(int argc, char *argv[]){
  dlog_clear();
  mmc_t *m = mmc_create();
  m->lex->debug = 1;
  m->debug = 1;
  //  mmc_parse_mml_string(m, "TR17a,+20,-80\nb,100,+30\nc,-50,-100", "c.mid");
  //    mmc_parse_mml_string(m, "$b36$s38 Tempo 150 TR10 #rhythm l16brrr srrr brrr srrr #end TR1c1 c1", "c.mid");
  //  mmc_parse_mml_string(m, "y7,100 cde y7,50 cde", "c.mid");
  mmc_parse_mml_string(m, "cd0e", "c.mid");
  //  mmc_parse_mml_string(m, "$b35$s38 TR10 #rhythm b", "c.mid");
  //mmc_parse_mml_file(m, "a.mml", "c.mid");
  printf("--log--\n");
  printf("%s",dlog_get());
  printf("--end of log--\n");
  mmc_destroy(m);
  return 0;
}
*/
