/*
 * =====================================================================================
 *
 *       Filename:  stktest.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/08/2015 15:00:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Bradley Golden (BG), bgolde5@uic.edu, golden.bradley@gmail.com
 *   Organization:  University of Illinois at Chicago
 *
 * =====================================================================================
 */

#include "stack.h"

int main (){

  LIST *stk;

  stk_create();

  int i;
  for(i=0; i<10; i++){
    stk_push(stk, i);
  }

  stk_print(stk);
  stk_pop(stk);
  stk_print(stk);
  stk_pop(stk);
  stk_print(stk);
  stk_destroy(stk);

  return 0;
}//end main
