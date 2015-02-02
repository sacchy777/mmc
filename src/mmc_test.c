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
#define WARNTEST(M) init(); mmc_parse_mml_string(m, M, NULL); CU_ASSERT(m->warning == 1); cleanup()

void note_test(){
  SUCCTEST("cdefgabcr");
  SUCCTEST("c4.^2|^1r1^2.^4");
  FAILTEST("c.^2|^1");
  SUCCTEST("c0e1^1");
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
  /*
  mmc_t *m = mmc_create();
  m->debug = 0;
  m->lex->debug = 0;
  //  mmc_parse_mml_string(m, "TR17a,+20,-80\nb,100,+30\nc,-50,-100", "c.mid");
  //    mmc_parse_mml_string(m, "$b36$s38 Tempo 150 TR10 #rhythm l16brrr srrr brrr srrr #end TR1c1 c1", "c.mid");
  //  mmc_parse_mml_string(m, "y7,100 cde y7,50 cde", "c.mid");
//  mmc_parse_mml_string(m, "v10 c0d", "c.mid");
//  mmc_parse_mml_string(m, "$a", "c.mid");
  mmc_parse_mml_string(m, "[[[[[[[[[[[[[[[[[a]]]]]]]]]]]]]]]]]", "c.mid");
  //  mmc_parse_mml_string(m, "$b35$s38 TR10 #rhythm b", "c.mid");
  //mmc_parse_mml_file(m, "a.mml", "c.mid");
  printf("--log--\n");
  printf("%s",dlog_get());
  printf("--end of log--\n");
  mmc_destroy(m);
*/
  return 0;
}



