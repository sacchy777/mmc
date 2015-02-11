/*
smf0.c - A trivial Standard Midi File Format 0 library

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
#include "smf0.h"
#include "smf0_error.h"
#include "dlog.h"
#include <stdlib.h>
#include <string.h>


smf0_t *smf0_create(){
  int i;
  smf0_t *s;
  s = calloc(sizeof(smf0_t), 1);
  if (!s) {
    dlog_add(SMF0_MSG_ERROR_OUTOFMEMORY, __func__);
    return NULL;
  }
  s->memory_used += sizeof(smf0_t);
  for(i = 0; i < MIDIEVENT_MAX; i ++){
    s->sort_events[i] = &s->events[i];
    s->events[i].absolute_time = ABSOLUTE_TIME_MAX;
  }
  s->timebase = 480;
  return s;
}

void smf0_destroy(smf0_t *s){
  int i;
  midievent_t *e;
  for(i = 0; i < MIDIEVENT_MAX; i ++){
    e = &s->events[i];
    if(e->extended_type == SMF0_EXT_DIRECT){
      //      printf("*** free direct mem %d\n", e->long_msg_len);
      free(e->long_msg);
    }
  }
  free(s);
}


void smf0_add_event_raw(smf0_t *s, int absolute_time, int type, int channel, int num_data, unsigned char *data){
  int i;
  static int error_detected = 0;
  if(s->index == MIDIEVENT_MAX){
    if(!error_detected){
      dlog_add(SMF0_MSG_ERROR_TOOMANYEVENTS);
      s->error = 1;
      error_detected = 1;
    }
    return;
  }
  midievent_t *e = &s->events[s->index ++];
  e->absolute_time = absolute_time;
  e->data[0] = type | channel;
  for(i = 0; i < num_data; i ++){
    e->data[i+1] = data[i];
  }
  e->datasize = num_data + 1;
}



void smf0_add_noteoff(smf0_t *s, int absolute_time, int channel, int key, int velocity){
  unsigned char data[2];
  data[0] = key;
  data[1] = velocity;
  smf0_add_event_raw(s, absolute_time, 0x80, channel, 2, data); 
}

void smf0_add_noteon(smf0_t *s, int absolute_time, int channel, int key, int velocity){
  unsigned char data[2];
  data[0] = key;
  data[1] = velocity;
  smf0_add_event_raw(s, absolute_time, 0x90, channel, 2, data); 
}

void smf0_add_keypressure(smf0_t *s, int absolute_time, int channel, int key, int value){
  unsigned char data[2];
  data[0] = key;
  data[1] = value;
  smf0_add_event_raw(s, absolute_time, 0xA0, channel, 2, data); 
}

void smf0_add_controlchange(smf0_t *s, int absolute_time, int channel, int control, int value){
  unsigned char data[2];
  data[0] = control;
  data[1] = value;
  smf0_add_event_raw(s, absolute_time, 0xB0, channel, 2, data); 
}

void smf0_add_programchange(smf0_t *s, int absolute_time, int channel, int program){
  unsigned char data[1];
  data[0] = program;
  smf0_add_event_raw(s, absolute_time, 0xC0, channel, 1, data); 
}

void smf0_add_channelpressure(smf0_t *s, int absolute_time, int channel, int value){
  unsigned char data[1];
  data[0] = value;
  smf0_add_event_raw(s, absolute_time, 0xD0, channel, 1, data); 
}

void smf0_add_pitchbend(smf0_t *s, int absolute_time, int channel, int low, int high){
  unsigned char data[2];
  data[0] = low;
  data[1] = high;
  smf0_add_event_raw(s, absolute_time, 0xE0, channel, 2, data); 
}

void smf0_add_note_ex(smf0_t *s, int absolute_time, int channel, int octave, int key, int sharp, float length, int velocity) {
  smf0_add_noteon(s, absolute_time, channel, octave * 12 + key + sharp, velocity);  
  smf0_add_noteoff(s, absolute_time + 4 * s->timebase * length, channel, octave * 12 + key + sharp, velocity);  
}

void smf0_add_tempo(smf0_t *s, int absolute_time, int tempo){
  unsigned char data[5];
  int usec = 60 * 1000000 / tempo;
  data[0] = 0x51;
  data[1] = 0x03;
  data[2] = (usec & 0xff0000 ) >> 16;
  data[3] = (usec & 0xff00 ) >> 8;
  data[4] = usec & 0xff;

  smf0_add_event_raw(s, absolute_time, 0xFF, 0, 5, data); 
}

void smf0_add_endoftrack(smf0_t *s, int absolute_time){
  unsigned char data[2];
  data[0] = 0x2F;
  data[1] = 0x00;
  smf0_add_event_raw(s, absolute_time, 0xFF, 0, 2, data); 
}



void smf0_add_meta_long(smf0_t *s, int absolute_time, int meta_type, char *meta_string, int meta_len){

  midievent_t *e = &s->events[s->index ++];
  e->absolute_time = absolute_time;
  e->data[0] = 0xff;
  e->data[1] = meta_type;
  e->data[2] = meta_len > 127 ? 127 : meta_len;
  e->datasize = 3;
  e->extended_type = SMF0_EXT_META_LONG;
  e->long_msg_len = meta_len;
  e->long_msg = meta_string;
}

void smf0_add_sysex(smf0_t *s, int absolute_time, char *string, int len){

  midievent_t *e = &s->events[s->index ++];
  e->absolute_time = absolute_time;
  e->extended_type = SMF0_EXT_DIRECT;
  e->long_msg_len = len + 1;
  e->long_msg = calloc(sizeof(char) * (len+1) , sizeof(char));
  //  printf("*** hunt direct mem %d\n", e->long_msg_len);
  e->long_msg[0] = string[0];
  e->long_msg[1] = len - 1;
  memcpy(e->long_msg + 2, string + 1, len - 1);
}


void smf0_add_question(smf0_t *s, int absolute_time){

  if(s->question == 1){
    dlog_add(SMF0_MSG_WARNING_QUESTION_DUPLICATED);
    return;
  }
  midievent_t *e = &s->events[s->index ++];
  e->absolute_time = absolute_time;
  e->datasize = 0;
  e->extended_type = SMF0_EXT_QUESTION;
  s->question = 1;
}




int smf0_get_key(int octave, int key, int sharp){
  return octave * 12 + key + sharp;
}

void smf0_add_note(smf0_t *s, int absolute_time, int channel, int key, float length, int velocity) {
  smf0_add_noteon(s, absolute_time, channel, key, velocity);  
  smf0_add_noteoff(s, absolute_time + 4 * s->timebase * length, channel, key, velocity);  
}

static int compare(const void *p, const void *q){
  return (*(midievent_t **)p)->absolute_time - (*(midievent_t **)q)->absolute_time;
}

static void smf0_reorder(smf0_t *s){
  qsort(s->sort_events, MIDIEVENT_MAX, sizeof(midievent_t *), compare);
}

void smf0_dump(smf0_t *s){
  int i;
  for(i = 0; i < MIDIEVENT_MAX; i ++){
    midievent_t *e = s->sort_events[i];
    if(e->absolute_time == ABSOLUTE_TIME_MAX) continue;
    printf("%09d : %02x %02x %02x\n", e->absolute_time, e->data[0], e->data[1], e->data[2]);
  }
}

static unsigned char header[] = {
  0x4d, 0x54, 0x68, 0x64, /* MThd */
  0x00, 0x00, 0x00, 0x06, /* header length 6 */
  0x00, 0x00, /* format 0 */
  0x00, 0x01, /* number of tracks = 1 */
  0x01, 0xe0, /* timebase 480 */
  0x4d, 0x54, 0x72, 0x6b, /* MTrk */
  0x00, 0x00, 0x00, 0x00, /* 4bytes length*/
};

//static unsigned char trailer [] = {
//  0x83, 0x60, /* delta time 480 ticks */
//  0xff, 0x2f, /* end of track */
//  0x00, /* */
//};

static void conv_be32(int value, unsigned char *be){
  be[3] = value & 0xff;
  be[2] = (value & 0xff00) >> 8;
  be[1] = (value & 0xff0000) >> 16;
  be[0] = (value & 0xff000000) >> 24;
}

static void conv_deltatime(int value, unsigned char *deltatime, int *size){
  memset(deltatime, 0, sizeof(unsigned char) * 4);
  if(value < 128){
    deltatime[0] = value;
    *size = 1;
	return;
  }
  if(value < 128 * 128){
    deltatime[1] = value % 128;
    deltatime[0] = 0x80 | (value / 128);
	*size = 2;
    return;
  }
  if(value < 128 * 128 * 128){
    deltatime[2] = value % 128;
    deltatime[1] = (value / 128 ) % 128;
    deltatime[0] = 0x80 | (value / 128 / 128);
	*size = 3;
    return;
  }
  deltatime[3] = value % 128;
  deltatime[2] = (value / 128) % 128;
  deltatime[1] = (value / 128 / 128) % 128;
  deltatime[0] = 0x80 | (value / 128 / 128 / 128);
  *size = 4;
}
/*
void midifile_add_event(midifile_t *mf){
}

void midifile_save(midifile_t *mf, const char *filename){
}
*/

static void smf0_write_event(smf0_t *s, FILE *fp){
  int i;
  int deltatime;
  int currenttime = 0;
  unsigned char deltatime_char[4];
  int deltatime_size;

  for(i = 0; i < MIDIEVENT_MAX; i ++){
    midievent_t *e = s->sort_events[i];
    if(e->absolute_time == ABSOLUTE_TIME_MAX) return;

    if(e->extended_type == SMF0_EXT_QUESTION){
      s->question_detected = 1;
      continue;
    }

    deltatime = e->absolute_time - currenttime;

    if(s->question == 1 && s->question_detected == 0) deltatime = 0;

    currenttime = e->absolute_time;

    conv_deltatime(deltatime, deltatime_char, &deltatime_size);

    if((s->question == 1 && s->question_detected == 0) && 
       (e->extended_type == SMF0_EXT_NONE && ((e->data[0] & 0xf0) == 0x90 || ((e->data[0] & 0xf0) == 0x80)))){
      ; //do nothing. skip until question detected
    }else{
      fwrite(deltatime_char, 1, deltatime_size, fp);
      s->tracksize += deltatime_size;
      fwrite(e->data, 1, e->datasize, fp);
      s->tracksize += e->datasize;
    }

    if(e->extended_type == SMF0_EXT_META_LONG || 
       e->extended_type == SMF0_EXT_DIRECT){
      fwrite(e->long_msg, 1, e->long_msg_len, fp);
      s->tracksize += e->long_msg_len;
    }

  }

}

int smf0_save(smf0_t *s, const char *filename){
  FILE *fp;
  //  int size;
  unsigned char tracksize[4];
  fp = fopen(filename , "wb");
  if(!fp){
    dlog_add(SMF0_MSG_ERROR_SAVE_FAILED, filename);
    s->error = 1;
    return -1;
  }
  smf0_reorder(s);
  fwrite(header, 1, sizeof(header)/sizeof(char), fp);

  smf0_write_event(s, fp);

  /*
    // end of track shall be inserted by user
  size = sizeof(trailer)/sizeof(char); 
  fwrite(trailer, 1, size, fp);
  s->tracksize += size;
  */

  conv_be32(s->tracksize, tracksize);
  fseek(fp, sizeof(header)/sizeof(unsigned char) - 4, SEEK_SET);
  fwrite(tracksize, 1, 4, fp);  

  fclose(fp);
  return 0;
}

/*
static void test_conv_deltatime(){
  unsigned char c[4];
  int size;
  conv_deltatime(480, c, &size);
  printf("%d : %x %x %x %x\n", size, c[0], c[1], c[2], c[3]);
}

static void test_smf0(){
  smf0_t *s = smf0_create();
  smf0_add_noteoff(s, 480, 0, 64, 100); 
  smf0_add_noteon(s, 0, 0, 64, 100); 
  smf0_reorder(s); 
  smf0_dump(s);
  smf0_save(s, "a.mid");
  smf0_destroy(s);
}

int main(int argc, char *argv[]){
  test_conv_deltatime();
  test_smf0();
}
*/

