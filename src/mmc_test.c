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

#define FAILTEST(M) init(); mmc_parse_mml_string(m, M, NULL); CU_ASSERT(m->error == 1); cleanup();
#define SUCCTEST(M) init(); mmc_parse_mml_string(m, M, NULL); CU_ASSERT(m->error == 0); cleanup();

void length_test(){
  SUCCTEST("l8")
  FAILTEST("l0")
}


void mmc_test_suite(){
  CU_pSuite s;
  s = CU_add_suite(__func__, NULL, NULL);
  CU_add_test(s, "length test", length_test);
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
  m->debug = 1;
  m->lex->debug = 1;
  //  mmc_parse_mml_string(m, "TR17a,+20,-80\nb,100,+30\nc,-50,-100", "c.mid");
  //    mmc_parse_mml_string(m, "$b36$s38 Tempo 150 TR10 #rhythm l16brrr srrr brrr srrr #end TR1c1 c1", "c.mid");
  //  mmc_parse_mml_string(m, "y7,100 cde y7,50 cde", "c.mid");
  mmc_parse_mml_string(m, "[a|b]", "c.mid");
  //  mmc_parse_mml_string(m, "$b35$s38 TR10 #rhythm b", "c.mid");
  //mmc_parse_mml_file(m, "a.mml", "c.mid");
  printf("--log--\n");
  printf("%s",dlog_get());
  printf("--end of log--\n");
  mmc_destroy(m);
  */
  return 0;
}



