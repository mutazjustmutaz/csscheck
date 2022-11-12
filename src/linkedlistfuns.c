#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/linkedlistfuns.h"


void stack_push(stack *s, char *str)
{
  struct elt *e;
  e = malloc(sizeof(struct elt));
  if(e == 0){
    fprintf(stderr, "Error: An instance of %s was found but storing it failed. Skipping.\n", str);
    return;
  }
  e->value = str;
  e->next = *s;
  *s = e;
}

char *stack_pop(stack *s)
{
  if(*s == 0){
    return 0;
  }
  char *str;
  struct elt *e;
  str = (*s)->value;
  e = *s;
  *s = e->next;
  free(e);
  return str;
}

//loop through linkedlist and count occurences of each color then nullify pointers inside structs. if > 1 print out. 
void stack_process(stack *s){
  char *clr;
  int clrcnt = 1;
  struct elt *clrelt;

  while(*s){
    
    if((clr = stack_pop(s))){
      
      for(clrelt = *s; clrelt != 0; clrelt = clrelt->next){
	if(clrelt->value && !strcmp(clrelt->value, clr)){
	  clrcnt += 1;
	  free(clrelt->value);
	  clrelt->value = 0;
	}
      }
      
      if(clrcnt > 1){  
	printf("%s : %d\n", clr, clrcnt);
	clrcnt = 1;
      } 

      free(clr);
    }  
  }
  
}
