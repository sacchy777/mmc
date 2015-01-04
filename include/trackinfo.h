#ifndef TRACKINFO_H
#define TRACKINFO_H

#define TRACK_NUM 16

typedef struct {
  int octave;
  int length;
  int gate;
  int velocity;
  int timing;
} trackinfo_note_params_t;

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
} trackinfo_t;

#define TRACKINFO_NOTE_MASK_OCTAVE 0x1
#define TRACKINFO_NOTE_MASK_KEY 0x2
#define TRACKINFO_NOTE_MASK_SHARP 0x4
#define TRACKINFO_NOTE_MASK_LENGTH 0x8
#define TRACKINFO_NOTE_MASK_GATE 0x10
#define TRACKINFO_NOTE_MASK_VELOCITY 0x20
#define TRACKINFO_NOTE_MASK_TIMING 0x40

void track_init(track_t *t);
trackinfo_t *trackinfo_create();
void trackinfo_note_params(trackinfo_t *ti, trackinfo_note_params_t *params);



#endif

