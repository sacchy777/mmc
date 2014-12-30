#ifndef MIDIEVENT_H
#define MIDIEVENT_H

typedef struct {
  int type;
  int absolute_time;
  int relative_time;
  unsigned char short_data[4];
} midievent_t;

enum {
  MIDIEVENT_NOTE_OFF = 0,
  MIDIEVENT_NOTE_ON,
  MIDIEVENT_PROGRAM_CHANGE,
  MIDIEVENT_CONTROL_CHANGE,
  MIDIEVENT_SYSEX_RESETGM,
  MIDIEVENT_META_TEMPO,
};

void midievent_set_note_off(midievent_t *e, int time, int channel, int key, int velocity);
void midievent_set_note_on(midievent_t *e, int time, int channel, int key, int velocity);
void midievent_set_program_change(midievent_t *e, int time, int channel, int program);
void midievent_set_control_change(midievent_t *e, int time, int channel, int value);

#endif
