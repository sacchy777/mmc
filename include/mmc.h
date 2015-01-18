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

#define MMC_VERSION "0.0"

#define TRACK_NUM 16
#include "smf0.h"
#include "lex.h"


enum {
  kMmcParamNone = 0,
  kMmcParamVelocity,
  kMmcParamGate,
  kMmcParamTiming,
  kMmcParamOctave,
  kMmcParamMax,
};

typedef struct {
  int valid;
  int value;
  int sign;
} param_t;

typedef struct {
  int currenttime;
  int params[kMmcParamMax];
  int length;
} track_t;

typedef struct {
  int length;
  param_t param;
} mmc_note_params_t;

typedef struct {
  int value;
  int value_default;
  int value_plusminus;
} mmc_param_t;

typedef struct {
  int key;
  int sharp;
  int natural;
  int length_default;
  float length;
  mmc_param_t gate;
  mmc_param_t velocity;
  mmc_param_t timing;
} mmc_note_t;

#define MMC_NOTE_MAX 16

typedef struct {
  smf0_t *smf0;
  lex_t *lex;
  int lex_index;
  token_t *current_token;
  track_t tracks[TRACK_NUM];
  mmc_note_t notes[MMC_NOTE_MAX];
  mmc_note_t current_note;
  int currenttrack;
  int error;
  int debug;
} mmc_t;



#define MMC_NOTE_MASK_OCTAVE 0x1
#define MMC_NOTE_MASK_KEY 0x2
#define MMC_NOTE_MASK_SHARP 0x4
#define MMC_NOTE_MASK_LENGTH 0x8
#define MMC_NOTE_MASK_GATE 0x10
#define MMC_NOTE_MASK_VELOCITY 0x20
#define MMC_NOTE_MASK_TIMING 0x40

mmc_t *mmc_create();
void mmc_destroy(mmc_t *m);
void mmc_save(mmc_t *m, const char *filename);

void mmc_add_note(mmc_t *m, int key, int sharp, double length, param_t *gate, param_t *velocity, param_t *timing, int proceed);

void mmc_add_controlchange(mmc_t *m, int cc, int value);
void mmc_add_programchange(mmc_t *m, int program);



void mmc_set_track(mmc_t *m, int track);
void mmc_set_octave(mmc_t *m, int octave);
void mmc_set_gate(mmc_t *m, int gate);
void mmc_set_veloticy(mmc_t *m, int velocity);








#endif

