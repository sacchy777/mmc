#include "midievent.h"
void midievent_set_note_off(midievent_t *e, int time, int channel, int key, int velocity){
  e->absolute_time = time;
  e->data[0] = 0x80 | channel;
  e->data[1] = key;
  e->data[2] = velocity;
}

void midievent_set_note_on(midievent_t *e, int time, int channel, int key, int velocity){
  e->absolute_time = time;
  e->data[0] = 0x90 | channel;
  e->data[1] = key;
  e->data[2] = velocity;
}

void midievent_set_program_change(midievent_t *e, int time, int channel, int program){
  
}
void midievent_set_control_change(midievent_t *e, int time, int channel, int value){
  
}
