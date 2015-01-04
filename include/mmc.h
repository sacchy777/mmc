#ifndef MMC_H
#define MMC_H

#define TRACK_NUM 16
#include "smf0.h"

typedef struct {
  int length;
  int gate;
  int velocity;
  int timing;
  int currenttime;
} mmc_note_params_t;

typedef struct {
  int currenttime;
  int octave;
  int length;
  int gate;
  int velocity;
  int timing;
} track_t;

typedef struct {
  smf0_t *smf0;
  track_t tracks[TRACK_NUM];
  int currenttrack;
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


void mmc_add_note(mmc_t *m, int key, int sharp, float length, int gate, int velocity, int timing, int proceed);
void mmc_add_controlchange(mmc_t *m, int cc, int value);
void mmc_add_programchange(mmc_t *m, int program);



void mmc_set_track(mmc_t *m, int track);
void mmc_set_octave(mmc_t *m, int octave);
void mmc_set_gate(mmc_t *m, int gate);
void mmc_set_veloticy(mmc_t *m, int velocity);








#endif

