#include <stdio.h>
#include <string.h>
#include <libxml/HTMLtree.h>
#include <libxml/HTMLparser.h>
#include "../include/corefuns.h"

struct ParserState {
  int styleflag;
  int stylenum;
  char *style;
  };

//callback function for style_tag_paths
void start_element_tags(void *user_data, const xmlChar *name, const xmlChar **attrs) {
  struct ParserState *pstate = user_data;
  if(!strcmp(pstate->style, (char *) name)){
    pstate->styleflag = 1;
  }
}

//find out which files contain the style tag and print out their paths.
void style_tag_paths(char **filesarr, const int farrlen){
  
  htmlSAXHandler sh = {0};
  sh.startElement = start_element_tags;
  struct ParserState my_state = {0};
  my_state.styleflag = 0;
  my_state.style = "style";
  
  for(int i = 0; i < farrlen; i++){
    htmlDocPtr doc = htmlSAXParseFile(filesarr[i], NULL, &sh, &my_state);
    if(doc){ xmlFreeDoc(doc); }
    if(my_state.styleflag){
      puts(filesarr[i]);
      my_state.styleflag = 0;
    }
  }
}

//callback function for style_att_paths
void start_element_att(void *user_data, const xmlChar *name, const xmlChar **attrs) {
  struct ParserState *pstate = user_data;
  int i = 0;
  while(attrs && attrs[i]){
    if(!strcmp(pstate->style, (char *) attrs[i])){
      pstate->stylenum += 1;
      break;
    }
    i += 2;
  }
}

//find out which files contain style attributes and print out their paths along with the attribute's frequency.
void style_att_paths(char **filesarr, const int farrlen){
  
  htmlSAXHandler sh = {0};
  sh.startElement = start_element_att;
  struct ParserState my_state = {0};
  my_state.stylenum = 0;
  my_state.style = "style";
  
  for(int i = 0; i < farrlen; i++){
    htmlDocPtr doc = htmlSAXParseFile(filesarr[i], NULL, &sh, &my_state);
    if(doc){ xmlFreeDoc(doc); }
    if(my_state.stylenum){
      printf("%s : %d element[s]\n", filesarr[i], my_state.stylenum);
      my_state.stylenum = 0;
    }
  }
}
