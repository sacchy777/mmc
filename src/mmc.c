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
#include "mmc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MMC_MAX_ERROR_STRING 256
char mmc_errorstring[MMC_MAX_ERROR_STRING];

static void track_init(track_t *t){
  memset(t, 0, sizeof(track_t));
  t->currenttime = 0;
  t->params[kMmcParamOctave] = 5;
  t->params[kMmcParamVelocity] = 100;
  t->params[kMmcParamGate] = 80;
  t->params[kMmcParamTiming] = 0;
}

mmc_t *mmc_create(){
  int i;
  memset(mmc_errorstring, 0, sizeof(char) * MMC_MAX_ERROR_STRING);
  mmc_t *m = calloc(sizeof(mmc_t), 1);
  if(!m){
    sprintf(mmc_errorstring, "[MMC] Out of Memory at %s", __func__);
    return NULL;
  }
  for(i = 0; i < TRACK_NUM; i ++){
    track_init(&m->tracks[i]);
  }
  m->smf0 = smf0_create();
  if(!m->smf0){
    mmc_destroy(m);
    return NULL;
  }
  m->lex = lex_create();
  if(!m->lex){
    mmc_destroy(m);
    return NULL;
  }
  m->currenttrack = 0;
  return m;
}

void mmc_destroy(mmc_t *m){
  if(m->lex) lex_destroy(m->lex);
  if(m->smf0) smf0_destroy(m->smf0);
  free(m);
}

static track_t *get_track(mmc_t *m){
  return &m->tracks[m->currenttrack];
}



//void mmc_add_note(mmc_t *m, mmc_note_params_t *param, unsigned int mask, int key, int sharp){

void mmc_add_note(mmc_t *m, int key, int sharp, double length, param_t *gate, param_t *velocity, param_t *timing, int proceed){
  track_t *t = get_track(m);
  smf0_t *s = m->smf0;
  int _gate = 0;
  int _velocity = 0;
  int _timing = 0;

  if(!gate->valid){
    _gate = t->params[kMmcParamGate];
  }else{
    if(gate->sign != 0){
      _gate = t->params[kMmcParamGate] + gate->value;
    }else{
      _gate = gate->value;
    }
  }

  if(!velocity->valid){
    _velocity = t->params[kMmcParamVelocity];
  }else{
    if(velocity->sign != 0){
      _velocity = t->params[kMmcParamVelocity] + velocity->value;
    }else{
      _velocity = velocity->value;
    }
  }

  if(!timing->valid){
    _timing = t->params[kMmcParamTiming];
  }else{
    if(timing->sign != 0){
      _timing = t->params[kMmcParamTiming] + timing->value;
    }else{
      _timing = timing->value;
    }
  }

  int note = t->params[kMmcParamOctave] * 12 + key + sharp;
  int timenext = s->timebase * 4 * length;
  float length_f = length * _gate / 100;
  if(m->debug){
    printf("Adding note time %d key %d length %f %f(gated) vel %d (proc %d)\n", t->currenttime, key, length, length_f, _velocity, proceed);
  }
  smf0_add_note(s, t->currenttime + _timing, m->currenttrack, note, length_f, _velocity);
  if(proceed) t->currenttime += timenext;
}

/*
void mmc_add_controlchange(mmc_t *m, int cc, int value){
  smf0_t *s = m->smf0;
  track_t *t = get_track(m);
  smf0_add_controlchange(s, t->currenttime, m->currenttrack, cc, value);
} 

void mmc_add_programchange(mmc_t *m, int program){
  smf0_t *s = m->smf0;
  track_t *t = get_track(m);
  smf0_add_programchange(s, t->currenttime, m->currenttrack, program);
} 



void mmc_set_track(mmc_t *m, int track){
  m->currenttrack = track;
}

void mmc_set_octave(mmc_t *m, int octave){
  get_track(m)->octave = octave;
}

void mmc_set_gate(mmc_t *m, int gate){
  get_track(m)->gate = gate;
}
void mmc_set_veloticy(mmc_t *m, int velocity){
  get_track(m)->velocity = velocity;
}

void mmc_dump(mmc_t *ti){
  int i;
  for(i = 0; i < TRACK_NUM; i ++){
    printf("\n");
  }
}
*/

int mmc_token_match(mmc_t *m, int type){
  token_t *t;
  t = lex_get_token(m->lex, &m->lex_index, 0);
  if(!t) return 0;
  return t->type == type;
}

void mmc_token_get_string(mmc_t *m, char **string, int *size){
  token_t *t;
  t = lex_get_token(m->lex, &m->lex_index, 0);
  *string = &m->lex->body[t->pos];
  *size = t->len;
}

int mmc_token_get_digit_param(mmc_t *m, param_t *param){
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


void mmc_save(mmc_t *m, const char *filename){
  smf0_save(m->smf0, filename);
}

int mmc_parse_sharp(mmc_t *m, int *value){
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

int mmc_parse_length(mmc_t *m, double *length){

  int size;
  int digit;
  char *p;
  char buf[256];

  if(m->debug){printf("  parsing length\n");}
  if(mmc_token_match(m, kTokenDigit)){
    mmc_token_get_string(m, &p, &size);
    if(size > 255){
      return 0;
    }
    memset(buf, 0, sizeof(char)*256);
    memcpy(buf, p, size);
    digit = atoi(buf);
    if(m->debug){printf("  match digit %d\n", digit);}
    if(digit <= 0) return 0;
    *length = 1.0 / (double)digit;
    return 1;
  }else{
    return 0;
  }
}



int mmc_parse_note(mmc_t *m){
  if(m->debug){
    printf("Parsing note %d\n", m->current_token->type);
  }
  token_t *t;

  mmc_note_params_t note_param;
  memset(&note_param, 0, sizeof(mmc_note_params_t));
  int key = 0;
  int sharp = 0;
  int temp_int;
  double length = 0;
  double temp_double;
  param_t gate;
  param_t velocity;
  param_t timing;
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

  while(mmc_parse_sharp(m, &temp_int) != 0){
    if(m->debug){printf(" sharp match %d \n", temp_int);}
    sharp += temp_int;
    m->lex_index ++;
  }

  if(mmc_parse_length(m, &temp_double) != 0){
    if(m->debug){printf(" length match %f\n", temp_double);}
    length += temp_double;
    m->lex_index ++;
    while(mmc_token_match(m, kTokenDot) != 0){
      if(m->debug){printf(" dot match \n");}
      temp_double /= 2.0;
      length += temp_double;
      m->lex_index ++;
    }
  }else{
    if(m->debug){printf(" no length match \n");}
    length += 1.0/4.0;
  }

  while(mmc_token_match(m, kTokenTie) != 0){
    m->lex_index ++;
    if(m->debug){printf(" tie match\n");}
    if(mmc_parse_length(m, &temp_double) != 0){
      if(m->debug){printf(" length match %f\n", temp_double);}
      length += temp_double;
      m->lex_index ++;
      while(mmc_token_match(m, kTokenDot) != 0){
	if(m->debug){printf(" dot match \n");}
	temp_double /= 2.0;
	length += temp_double;
	m->lex_index ++;
      }
    }else{
      length += 1.0/4.0;
    }
  }

  if(mmc_token_match(m, kTokenComma) != 0){
    if(m->debug){printf(" gate comma match\n");}
    m->lex_index ++;
    if(mmc_token_get_digit_param(m, &gate)){
      m->lex_index ++;
    }
  }

  if(mmc_token_match(m, kTokenComma)){
    if(m->debug){printf(" velocity comma match\n");}
    m->lex_index ++;
    if(mmc_token_get_digit_param(m, &velocity)){
      m->lex_index ++;
    }
  }

  if(mmc_token_match(m, kTokenComma)){
    if(m->debug){printf(" timing comma match\n");}
    m->lex_index ++;
    if(mmc_token_get_digit_param(m, &timing)){
      m->lex_index ++;
    }
  }
  
  if(m->debug){
    printf("note : key %d sharp %d len %f %d %d %d\n", 
	   key, sharp, length, gate.value, velocity.value, timing.value);
  }
  
  mmc_add_note(m, key, sharp, length, &gate, &velocity, &timing, 1);
}
/*
void mmc_parse_rest(mmc_t *m){
  token_t *t;
  int key;
  t = lex_get_token(m->lex, &m->lex_index, 1);
  if(!t) return 0;
  if(t->type != kTokenRest) return 0;
  m->lex_index ++;
  if(mmc_parse_length() != 0) mmc_parse_dot();
  while(mmc_parse_tie() != 0){
    if(mmc_parse_length() != 0) mmc_parse_dot();
  }
}
*/


int mmc_parse_octave(mmc_t *m, int delta){
  if(m->debug){
    printf("Parsing octave change %d\n", m->current_token->type);
  }
  token_t *t;
  t = lex_get_token(m->lex, &m->lex_index, 0);
  if(!t){
    if(m->debug){printf("parse note null\n");}
    return -1;
  }
  m->lex_index ++;

  track_t *tr = get_track(m);
  int octave = tr->params[kMmcParamOctave] + delta;
  if(octave < 0 || octave > 8){
    printf("error! %s\n", __func__);
    return -1;
  }
  tr->params[kMmcParamOctave] = octave;
}


void mmc_parse(mmc_t *m){
  while((m->current_token = lex_get_token(m->lex, &m->lex_index, 0) )!= NULL){
    switch(m->current_token->type){
    case kTokenNoteA:
    case kTokenNoteB:
    case kTokenNoteC:
    case kTokenNoteD:
    case kTokenNoteE:
    case kTokenNoteF:
    case kTokenNoteG:
      mmc_parse_note(m);
      break;

    case kTokenRest:
      if(m->debug){
	printf("Parsing rest %d\n", m->current_token->type);
      }
      //      mmc_parse_rest(m);
      m->lex_index ++;
      break;


    case kTokenOctaveUp:
      mmc_parse_octave(m, 1);
      break;

    case kTokenOctaveDown:
      mmc_parse_octave(m, -1);
      break;

    default:
      if(m->debug){
	printf("Error %d\n", m->current_token->type);
      }
      m->error = 1;
      // error!
      break;
    } // end of switch
    if(m->error) break;
  }
  
}

int mmc_parse_mml_string(mmc_t *m, const char *mml, const char *filename){
  if(lex_open_string(m->lex, mml) != 0) return -1;
  if(lex_parse(m->lex) != 0) return -1;
  lex_dump_tokens(m->lex);
  mmc_parse(m);
  smf0_dump(m->smf0);
  smf0_save(m->smf0, filename);
  return 0;
}

void mmc_parse_mml_file(mmc_t *m, const char *mml_filename, const char *mid_filename){
  lex_open(m->lex, mml_filename);
  lex_parse(m->lex);
  lex_dump_tokens(m->lex);
  mmc_parse(m);
  smf0_save(m->smf0, mid_filename);
}

/*
int main(int argc, char *argv[]){
  mmc_t *m = mmc_create();
  m->lex->debug = 1;
  m->debug = 1;
  mmc_parse_mml_string(m, "cdefgab>c", "c.mid");
  //mmc_parse_mml_file(m, "a.mml", "c.mid");

  mmc_destroy(m);
  return 0;
}
*/
