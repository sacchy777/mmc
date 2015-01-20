#include "smf0.h"

void test1(smf0_t *s){
  printf("=== added events\n");
  smf0_add_programchange(s, 0, 0, 80); 
  smf0_add_noteon(s, 0, 0, 60, 100); 
  smf0_add_noteoff(s, 480, 0, 60, 100); 
  smf0_add_noteon(s, 480, 0, 64, 100); 
  smf0_add_noteoff(s, 960, 0, 64, 100); 
  smf0_add_noteon(s, 960, 0, 67, 100); 
  smf0_add_noteoff(s, 1440, 0, 67, 100); 
  smf0_add_noteon(s, 1440, 0, 72, 100); 
  smf0_add_noteoff(s, 1920, 0, 72, 100); 
  smf0_dump(s);
  smf0_save(s, "a.mid");
  printf("=== saved events\n");
  smf0_dump(s);
} 

// Dmaj7
void test2(smf0_t *s){
  int ch = 0;
  smf0_add_programchange(s, 0, ch, 0); 
  smf0_add_controlchange(s, 0, ch, SMF0_CC_VOLUME, 40); 
  smf0_add_note(s, 0, ch, smf0_get_key(5, SMF0_KEY_D, 0), 0.10, 100);
  smf0_add_note(s, 480, ch, smf0_get_key(5, SMF0_KEY_F, 1), 0.10, 100);
  smf0_add_note(s, 960, ch, smf0_get_key(5, SMF0_KEY_A, 0), 0.10, 100);
  smf0_add_note(s, 1440, ch, smf0_get_key(6, SMF0_KEY_C, 1), 0.10, 100);

  smf0_add_note(s, 1920, ch, smf0_get_key(5, SMF0_KEY_D, 0), 1.0, 100);
  smf0_add_note(s, 1920, ch, smf0_get_key(5, SMF0_KEY_F, 1), 1.0, 100);
  smf0_add_note(s, 1920, ch, smf0_get_key(5, SMF0_KEY_A, 0), 1.0, 100);
  smf0_add_note(s, 1920, ch, smf0_get_key(6, SMF0_KEY_C, 1), 1.0, 100);
  smf0_save(s, "a.mid");
}

int main(int argc, char *argv[]){
  smf0_t *s = smf0_create();
  test2(s);
  smf0_destroy(s);
  return 0;
}


