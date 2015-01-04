#include "mmc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void track_init(track_t *t){
  memset(t, 0, sizeof(track_t));
  t->currenttime = 0;
  t->octave = 5;
  t->gate = 80;
  t->velocity = 100;
}

mmc_t *mmc_create(){
  int i;
  mmc_t *m = calloc(sizeof(mmc_t), 1);
  for(i = 0; i < TRACK_NUM; i ++){
    track_init(&m->tracks[i]);
  }
  m->smf0 = smf0_create();
  m->currenttrack = 0;
  return m;
}

void mmc_destroy(mmc_t *m){
  smf0_destroy(m->smf0);
  free(m);
}

static track_t *get_track(mmc_t *m){
  return &m->tracks[m->currenttrack];
}

void mmc_note_params_default(mmc_t *m, mmc_note_params_t *params){
  track_t *t = get_track(m);
  params->length = t->length;
  params->gate = t->gate;
  params->velocity = t->velocity;
  params->timing = t->timing;
}

//void mmc_add_note(mmc_t *m, mmc_note_params_t *param, unsigned int mask, int key, int sharp){
void mmc_add_note(mmc_t *m, int key, int sharp, float length, int gate, int velocity, int timing, int proceed){
  track_t *t = get_track(m);
  smf0_t *s = m->smf0;
  float _length = length < 0 ? t->length : length;
  int _gate = gate < 0 ? t->gate : gate;
  int _velocity = velocity < 0 ? t->velocity : velocity;
  int _timing = timing < 0 ? t->timing : timing;

  int note = t->octave * 12 + key + sharp;
  int timenext = s->timebase * 4 * _length;
  float length_f = _length * _gate / 100;
  smf0_add_note(s, t->currenttime, m->currenttrack, note, length_f, _velocity);
  if(proceed) t->currenttime += timenext;
}

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

void mmc_save(mmc_t *m, const char *filename){
  smf0_save(m->smf0, filename);
}



