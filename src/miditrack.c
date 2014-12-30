static unsigned char header[] = {
  0x4d, 0x54, 0x68, 0x64, /* MThd */
  0x00, 0x00, 0x00, 0x06, /* header length 6 */
  0x00, 0x00, /* format 0 */
  0x00, 0x01, /* number of tracks = 1 */
  0x01, 0xe0, /* timebase 480 */
  0x4d, 0x54, 0x72, 0x6b, /* MTrk */
  /* 4bytes length*/
};

static unsigned char trailer [] = {
  0x83, 0x60, /* delta time 480 ticks */
  0xff, 0x2f, /* end of track */
  0x00, /* */
};

static void conv_be32(int value, unsigned char *be){
  be[3] = value & 0xff;
  be[2] = (value & 0xff00) >> 8;
  be[1] = (value & 0xff0000) >> 16;
  be[0] = (value & 0xff000000) >> 24;
}

static void conv_deltatime(int value, unsigned char *deltatime){
  int i;
  for(i = 0 ; i < 4; i ++){
    if(value < 128){
      deltatime[i] = value;
      return;
    }
    deltatime[i] = value & 0x7f | 0x80;
    value >>= 7;
  }
}

void midifile_add_event(midifile_t *mf){
}

void midifile_save(midifile_t *mf, const char *filename){
}
