#include "mmc.h"

int main(int argc, char *argv[]){
  mmc_t *mmc = mmc_create();

  mmc_set_track(mmc, 0);
  mmc_set_octave(mmc, 3);
  mmc_add_programchange(mmc, 38);
  mmc_add_controlchange(mmc, SMF0_CC_PANPOT, 127);
  mmc_add_note(mmc, SMF0_KEY_G, 0, 1.0, 80, 100, 0, 1);
  mmc_set_octave(mmc, 4);
  mmc_add_note(mmc, SMF0_KEY_C, 0, 1.0, 80, 100, 0, 1);





  mmc_set_track(mmc, 1);
  mmc_add_controlchange(mmc, SMF0_CC_PANPOT, 0);

  mmc_add_note(mmc, SMF0_KEY_D, 0, 1.0, 80, 100, 0, 0);
  mmc_add_note(mmc, SMF0_KEY_F, 0, 1.0, 80, 100, 0, 0);
  mmc_add_note(mmc, SMF0_KEY_A, 0, 1.0, 80, 100, 0, 0);
  mmc_set_octave(mmc, 6);
  mmc_add_note(mmc, SMF0_KEY_C, 0, 1.0, 80, 100, 0, 1);

  mmc_set_octave(mmc, 5);
  mmc_add_note(mmc, SMF0_KEY_C, 0, 1.0, 80, 100, 0, 0);
  mmc_add_note(mmc, SMF0_KEY_E, 0, 1.0, 80, 100, 0, 0);
  mmc_add_note(mmc, SMF0_KEY_G, 0, 1.0, 80, 100, 0, 0);
  mmc_add_note(mmc, SMF0_KEY_B, 0, 1.0, 80, 100, 0, 1);





  mmc_save(mmc, "b.mid");
  mmc_destroy(mmc);
}



