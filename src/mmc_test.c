/*
mmc_test.c - a small tests

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
#include "mmc.h"
#include "dlog.h"


#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

mmc_t *m;

int init(){
  dlog_clear();
  m = mmc_create();
  return 0;
}

int cleanup(){
  mmc_destroy(m);
  m = NULL;
  return 0;
}

#define FAILTEST(M) init(); mmc_parse_mml_string(m, M, NULL); CU_ASSERT(m->error == 1); cleanup()
#define SUCCTEST(M) init(); mmc_parse_mml_string(m, M, NULL); CU_ASSERT(m->error == 0); cleanup()
#define WARNTEST(M) init(); mmc_parse_mml_string(m, M, NULL); CU_ASSERT(m->error == 0 && m->warning == 1); cleanup()

void note_test(){
  SUCCTEST("cdefgabcr");
  SUCCTEST("c4.^2|^1r1^2.^4");
  FAILTEST("c.^2|^1");
  SUCCTEST("c0e1^1");
  SUCCTEST("c+++++ ----- +++++ ----- 4.");
  SUCCTEST("n0");
  SUCCTEST("n127");
  FAILTEST("n128");
  FAILTEST("n-1");
  FAILTEST("n");
  FAILTEST("n9999999999");
  SUCCTEST("c********+++++---");

}
void param_test(){
  SUCCTEST("v0v127q0q999");
  SUCCTEST("v-100");
  WARNTEST("v-101");
  WARNTEST("v128");
  SUCCTEST("v+27");
  WARNTEST("v+28");
  FAILTEST("v");

  SUCCTEST("q-80");
  SUCCTEST("q+920");
  WARNTEST("q+921");
  FAILTEST("q");
  WARNTEST("q-81");

  SUCCTEST("TR1");
  SUCCTEST("TR16");
  FAILTEST("TR");
  FAILTEST("TR0");
  FAILTEST("TR-1");
  FAILTEST("TR17");

  SUCCTEST("o0o10");
  FAILTEST("o");
  SUCCTEST("o-5");
  SUCCTEST("o+5");
  WARNTEST("o-6");
  WARNTEST("o+6");

  WARNTEST("r-4");

  SUCCTEST("Tempo1 Tempo999");
  FAILTEST("Tempo");
  WARNTEST("Tempo-1");
  WARNTEST("Tempo1000");

  SUCCTEST("y0,0 y127,127");
  FAILTEST("y128,1");
  FAILTEST("y1,128");
  FAILTEST("y");
  FAILTEST("y,");
  FAILTEST("y0,");
  FAILTEST("y,0");

  SUCCTEST("@1@128");
  FAILTEST("@0");
  FAILTEST("@129");

  FAILTEST("[[[[[[[[[[[[[[[[[a]]]]]]]]]]]]]]]]]");
  SUCCTEST("[[[[[[[[[[[[[[[[a]]]]]]]]]]]]]]]]"); // but fails due to too many midi events
  FAILTEST("[a");
  FAILTEST(":a");
  FAILTEST("[a:a:]");
  FAILTEST("]");
  FAILTEST("[0a]");
  SUCCTEST("[1a]");
  FAILTEST("[101a]");
  SUCCTEST("[100a]");
  FAILTEST("[cTR2]d");

  SUCCTEST("Transpose-24");
  SUCCTEST("Transpose24");
  SUCCTEST("Transpose +24");
  FAILTEST("Transpose-25");
  FAILTEST("Transpose25");
  FAILTEST("Transpose+25");

  FAILTEST("SysEx abc");
  FAILTEST("SysEx f0h,,f7h");
  FAILTEST("SysEx F0h,F7h");
  FAILTEST("SysEx G0h,F7h");
  FAILTEST("SysEx xxh,yyh");
  FAILTEST("SysEx F0h,80h, F7h"); // params shall be 00-7f except start f0 and end f7
  FAILTEST("SysEx F0h,FFh, F7h"); // 
  SUCCTEST("SysEx F0h,00h, F7h");
  SUCCTEST("SysEx F0h,7fh, F7h");
  SUCCTEST("SysEx f0h,11h, f7h");
  FAILTEST("SysEx f1h,11h, f7h");
  FAILTEST("SysEx f0h,11h, f6h");
  SUCCTEST("SysEx f0H,7eH,7fh,09h,01h,F7h cde"); // GM System On
  FAILTEST("SysEx f0H,7e,7fh,09h,01h,F7h cde"); // GM System On
  SUCCTEST("SysEx f0H,41h,20h,42h,12h,40h,00h,7fh,00h,41h,f7h cde"); // GS Reset
  SUCCTEST("SysEx f0H,43h,10h,4ch,00h,00h,7eh,00h,f7h cde"); // XG System On
  SUCCTEST("SysEx f0H,43h,10h,4ch,00h,00h,7eh,00h,f7h cde"); // XG System On
  FAILTEST("SysEx"
	   "f0h,01h,02h,03h,04h,05h,06h,07h,08h,09h,0ah,0bh,0ch,0dh,0eh,0fh,"
	   "00h,01h,02h,03h,04h,05h,06h,07h,08h,09h,0ah,0bh,0ch,0dh,0eh,0fh,"
	   "00h,01h,02h,03h,04h,05h,06h,07h,08h,09h,0ah,0bh,0ch,0dh,0eh,0fh,"
	   "00h,01h,02h,03h,04h,05h,06h,07h,08h,09h,0ah,0bh,0ch,0dh,0eh,0fh,"
	   "00h,01h,02h,03h,04h,05h,06h,07h,08h,09h,0ah,0bh,0ch,0dh,0eh,0fh,"
	   "00h,01h,02h,03h,04h,05h,06h,07h,08h,09h,0ah,0bh,0ch,0dh,0eh,0fh,"
	   "00h,01h,02h,03h,04h,05h,06h,07h,08h,09h,0ah,0bh,0ch,0dh,0eh,0fh,"
	   "00h,01h,02h,03h,04h,05h,06h,07h,08h,09h,0ah,0bh,0ch,0dh,0eh,f7h"); // 128 bytes 
  SUCCTEST("SysEx"
	   "f0h,01h,02h,03h,04h,05h,06h,07h,08h,09h,0ah,0bh,0ch,0dh,0eh,0fh,"
	   "00h,01h,02h,03h,04h,05h,06h,07h,08h,09h,0ah,0bh,0ch,0dh,0eh,0fh,"
	   "00h,01h,02h,03h,04h,05h,06h,07h,08h,09h,0ah,0bh,0ch,0dh,0eh,0fh,"
	   "00h,01h,02h,03h,04h,05h,06h,07h,08h,09h,0ah,0bh,0ch,0dh,0eh,0fh,"
	   "00h,01h,02h,03h,04h,05h,06h,07h,08h,09h,0ah,0bh,0ch,0dh,0eh,0fh,"
	   "00h,01h,02h,03h,04h,05h,06h,07h,08h,09h,0ah,0bh,0ch,0dh,0eh,0fh,"
	   "00h,01h,02h,03h,04h,05h,06h,07h,08h,09h,0ah,0bh,0ch,0dh,0eh,0fh,"
	   "00h,01h,02h,03h,04h,05h,06h,07h,08h,09h,0ah,0bh,0ch,0dh,f7h"); // 127 bytes 

  SUCCTEST("Copyright\"copyright name\"");
  SUCCTEST("TrackName    \n\"track name\"");
  FAILTEST("Copyright\"copyright name");
  FAILTEST("TrackName track name\"");


  SUCCTEST("Key ,,,,,,");
  SUCCTEST("Key 1,-1,1,-1,0,-1,1");
  FAILTEST("Key -1,1,-1,0,-1,1");
  SUCCTEST("Key 1,-,+1,-1,0,-1,1"); // warning
  FAILTEST("Key ++1,,,,,,"); 
  FAILTEST("Key --1,,,,,,"); 
  FAILTEST("Key --,,,,,,"); 


  SUCCTEST("{c}d");
  FAILTEST("c}d");
  FAILTEST("{{c");
  FAILTEST("{cTR2}d");



}

void mmc_test_suite(){
  CU_pSuite s;
  s = CU_add_suite(__func__, NULL, NULL);
  CU_add_test(s, "Note", note_test);
  CU_add_test(s, "Parameter", param_test);
}

void unit_test(){
  CU_initialize_registry();
  mmc_test_suite();
  CU_basic_run_tests();
  CU_cleanup_registry();
}

int main(int argc, char *argv[]){
  dlog_clear();
  unit_test();
  dlog_clear();
  mmc_t *m = mmc_create();
  m->debug = 1;
  m->lex->debug = 1;
  //  mmc_parse_mml_string(m, "TR17a,+20,-80\nb,100,+30\nc,-50,-100", "c.mid");
  //    mmc_parse_mml_string(m, "$b36$s38 Tempo 150 TR10 #rhythm l16brrr srrr brrr srrr #end TR1c1 c1", "c.mid");
  //  mmc_parse_mml_string(m, "y7,100 cde y7,50 cde", "c.mid");
//  mmc_parse_mml_string(m, "v10 c0d", "c.mid");
//  mmc_parse_mml_string(m, "$a", "c.mid");
  mmc_parse_mml_string(m, "{g>d}c", "c.mid");
  //  mmc_parse_mml_string(m, "$b35$s38 TR10 #rhythm b", "c.mid");
  //mmc_parse_mml_file(m, "a.mml", "c.mid");
  printf("--log--\n");
  printf("%s",dlog_get());
  printf("--end of log--\n");
  mmc_destroy(m);

  return 0;
}



