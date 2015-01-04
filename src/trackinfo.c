#include "trackinfo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void track_init(track_t *t){
  memset(t, 0, sizeof(track_t));
  t->currenttime = 0;
  t->octave = 5;
  t->gate = 80;
  t->velocity = 100;
  t->currenttrack = 0;
}

trackinfo_t *trackinfo_create(){
  int i;
  trackinfo *ti = calloc(sizeof(trackinfo_t), 1);
  for(i = 0; i < TRACK_NUM; i ++){
    track_init(ti->tracks[i]);
  }
  ti->smf0 = smf0_create();
  return ti;
}

void trackinfo_destroy(trackinfo_t *ti){
  smf0_destroy(ti->smf0);
  free(ti);
}


static track_t *get_track(trackinfo_t *ti){
  return &ti->tracks[ti->currenttrack];
}

void trackinfo_note_params_default(trackinfo_t *ti, trackinfo_note_params_t *params){
  track_t *t = get_track(ti);
  params->octave = t->octave;
  params->length = t->length;
  params->gate = t->gate;
  params->velocity = t->velocity;
  params->timing = t->timing;
}

void trackinfo_add_note(trackinfo_t *ti, trackinfo_note_params_t *param, unsigned int mask, int key, int sharp){
  track_t *t = get_track(ti);
  trackinfo_note_params_t default_param;
  trackinfo_note_params_default(ti, &default_param);
  int octave = TRACKINFO_NOTE_MASK_OCTAVE & mask == 0 ? default_param.octave : param->octave;
  int length = TRACKINFO_NOTE_MASK_LENGTH & mask == 0 ? default_param.length : param->length;
  int gate = TRACKINFO_NOTE_MASK_GATE & mask == 0 ? default_param.gate : param->gate;
  int velocity = TRACKINFO_NOTE_MASK_VELOCITY & mask == 0 ? default_param.velocity : param->velocity;
  int timing = TRACKINFO_NOTE_MASK_TIMING & mask == 0 ? default_param.timing : param->timing;

  int note = octave * 12 + key + sharp;
  int delta = 0;
  smf0_add_note(s, t->currenttime, ti->currenttrack, octave, key, sharp, 0.25, velocity);
  smf0_add_note(s, t->currenttime + delta, ti->currenttrack, octave, key, sharp, 0.25, velocity);
  t->currenttime += detla;
}

void trackinfo_set_track(trackinfo_t *ti, int track){
  if(track < 0 || track > 15){
    ti->error = 1;
    
	return;
  }
  ti->currenttrack = track;
}

void trackinfo_set_octave(trackinfo_t *ti, int track){
  get_track(ti)->octave = octave;
}

void trackinfo_set_gate(trackinfo_t *ti, int track){
  get_track(ti)->gate = gate;
}
void trackinfo_set_veloticy(trackinfo_t *ti, int track){
  get_track(ti)->velocity = velocity;
}

void trackinfo_dump(trackinfo_t *ti){
  int i;
  for(i = 0; i < TRACK_NUM; i ++){
    printf("");
  }
}
