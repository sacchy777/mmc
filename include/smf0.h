/*
smf0.h - A trivial Standard Midi File Format 0 library

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
#ifndef SMF0_H
#define SMF0_H

#include <stdio.h>

#define MIDIEVENT_SHORT_DATA_MAX 16
#define MIDIEVENT_MAX 100000
#define ABSOLUTE_TIME_MAX 999999999

enum {
  SMF0_KEY_C = 0,
  SMF0_KEY_D = 2,
  SMF0_KEY_E = 4,
  SMF0_KEY_F = 5,
  SMF0_KEY_G = 7,
  SMF0_KEY_A = 9,
  SMF0_KEY_B = 11,
};

enum {
  SMF0_CC_BANKMSB = 0,
  SMF0_CC_MODULATION = 1,
  SMF0_CC_PORTAMENT_TIME = 5,
  SMF0_CC_DATAMSB = 6,
  SMF0_CC_VOLUME = 7,
  SMF0_CC_PANPOT = 10,
  SMF0_CC_EXPRESSION = 11,
  SMF0_CC_DATALSB = 38,
  SMF0_CC_HOLD = 64,
  SMF0_CC_NRPNLSB = 98,
  SMF0_CC_NRPNMSB = 99,
  SMF0_CC_RPNLSB = 100,
  SMF0_CC_RPNMSB = 101,
  SMF0_CC_ALLNOTEOFF = 123,
};

enum {
  SMF0_EXT_NONE = 0,
  SMF0_EXT_META_LONG,
  SMF0_EXT_QUESTION,
};



typedef struct {
  int absolute_time;
  int datasize;
  unsigned char data[MIDIEVENT_SHORT_DATA_MAX];
  int extended_type;
  char *long_msg;
  int long_msg_len;
} midievent_t;

typedef struct {
  int index;
  int timebase;
  int tracksize;
  midievent_t events[MIDIEVENT_MAX];
  midievent_t *sort_events[MIDIEVENT_MAX];
  int start_from_enable;
  int question;
  int question_detected;
  int error;
  int warning;
  int memory_used;
} smf0_t;

smf0_t *smf0_create();
void smf0_destroy(smf0_t *s);
void smf0_add_event_raw(smf0_t *s, int absolute_time, int type, int channel, int num_data, unsigned char *data);
void smf0_add_noteoff(smf0_t *s, int absolute_time, int channel, int key, int velocity);
void smf0_add_noteon(smf0_t *s, int absolute_time, int channel, int key, int velocity);
void smf0_add_keypressure(smf0_t *s, int absolute_time, int channel, int key, int value);
void smf0_add_controlchange(smf0_t *s, int absolute_time, int channel, int control, int value);
void smf0_add_programchange(smf0_t *s, int absolute_time, int channel, int program);
void smf0_add_channelpressure(smf0_t *s, int absolute_time, int channel, int value);
void smf0_add_pitchbend(smf0_t *s, int absolute_time, int channel, int low, int high);
void smf0_add_tempo(smf0_t *s, int absolute_time, int tempo);
void smf0_dump(smf0_t *s);
int smf0_save(smf0_t *s, const char *filename);
void smf0_add_note_ex(smf0_t *s, int absolute_time, int channel, int octave, int key, int sharp, float length, int velocity);
int smf0_get_key(int octave, int key, int sharp);
void smf0_add_note(smf0_t *s, int absolute_time, int channel, int key, float length, int velocity);
void smf0_add_meta_long(smf0_t *s, int absolute_time, int meta_type, char *meta_string, int meta_len);
void smf0_add_question(smf0_t *s, int absolute_time);

#endif
