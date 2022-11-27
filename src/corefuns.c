#include <stdio.h>
#include <string.h>
#include <libxml/HTMLtree.h>
#include <libxml/HTMLparser.h>
#include <regex.h>
#include "../include/corefuns.h"

#define REGEX_COUNT (153)
#define COMMENT_REGEX_INDEX (152)
#define COLON_REGEX_INDEX (151)

struct ParserState {
  int styleflag;
  int stylenum;
  char *style;
  };

struct elt {
  char *value;
  struct elt *next;
};

typedef struct elt *stack;


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


regex_t *create_regexes(void){
  
  char *strarr[] = {"black", "silver", "gray", "grey", "darkgray" , "darkgrey", "darkslategray", "darkslategrey", "dimgray", "dimgrey", "lightgray", "lightgrey", "slategray", "slategrey", "white", "maroon", "red", "purple", "fuchsia", "magenta", "green", "lime", "olive", "yellow", "navy", "blue", "teal", "aqua", "orange", "aliceblue", "antiquewhite", "aquamarine" "azure", "beige", "bisque", "blanchedalmond", "blueviolet", "brown", "burlywood", "cadetblue", "chartreuse", "chocolate", "coral", "cornflowerblue", "cornsilk", "crimson", "cyan", "darkblue", "darkcyan", "darkgoldenrod", "darkgreen", "darkkhaki", "darkmagenta", "darkolivegreen", "darkorange", "darkorchid", "darkred", "darksalmon", "darkseagreen", "darkslateblue", "darkturquoise", "darkviolet", "deeppink", "deepskyblue", "dodgerblue", "firebrick", "floralwhite", "forestgreen", "gainsboro", "ghostwhite", "gold", "goldenrod", "greenyellow", "honeydew", "hotpink", "indianred", "indigo", "ivory", "khaki", "lavender", "lavenderblush", "lawngreen", "lemonchiffon", "lightblue", "lightcoral", "lightcyan", "lightgoldenrodyellow", "lightgreen", "lightpink", "lightsalmon", "lightseagreen", "lightskyblue", "lightslategray", "lightslategrey", "lightsteelblue", "lightyellow", "limegreen", "linen", "mediumaquamarine", "mediumblue", "mediumorchid", "mediumpurple", "mediumseagreen", "mediumslateblue", "mediumspringgreen", "mediumturquoise", "mediumvioletred", "midnightblue", "mintcream", "mistyrose", "moccasin", "navajowhite", "oldlace", "olivedrab", "orangered", "orchid", "palegoldenrod", "palegreen", "paleturquoise", "palevioletred", "papayawhip", "peachpuff", "peru", "pink", "plum", "powderblue", "rosybrown", "royalblue", "saddlebrown", "salmon", "sandybrown", "seagreen", "seashell", "sienna", "skyblue", "slateblue", "snow", "springgreen", "steelblue", "tan", "thistle", "tomato", "turquoise", "violet", "wheat", "whitesmoke", "yellowgreen", "rebeccapurple", "#[[:alnum:]]{3,8}", "rgba?\\([^\\(\\)]*\\)", "hsla?\\([^\\(\\)]*\\)", "hwb\\([^\\(\\)]*\\)", "\\\n[^:]*\\\n", "/\\*([^\\*]|\\*+[^\\*/])*\\**\\*/"};    //the regex voodoo in the last element is a workaround for a workaround that i had to use because regex.h doesn't support lazy matching.
  regex_t *regexarr = malloc(sizeof(regex_t) * REGEX_COUNT);
  if(regexarr == 0){
    return 0;
  }
  regex_t regex;
  int flags = (REG_EXTENDED | REG_ICASE);
  
  for(int i = 0; i < REGEX_COUNT; i++){
    /* In case of a regcomp error, the result stored in regex is "undefined". we can either rely on regexec to fail at color or store a unique value in the regex and test for it later.
       The only field required by POSIX is "size_t re_nsub", which holds the "number of parenthesized subexpressions". i'll assign a ridiculous value to it and test for it later. */
    if(regcomp(&regex, strarr[i], flags)){
      switch(i){
      case COMMENT_REGEX_INDEX:
	fprintf(stderr, "Error on regex for inline comments.\n");
	free(regexarr);
	return 0;
	break;
      case COLON_REGEX_INDEX:
	fprintf(stderr, "Error on regex for colons.\n");
	free(regexarr);
	return 0;
	break;
      case 150:
	fprintf(stderr, "Error on regex for hwb colors. Skipped.\n");
	break;
      case 149:
	fprintf(stderr, "Error on regex for hsl[a] colors. Skipped.\n");
	break;
      case 148:
	fprintf(stderr, "Error on regex for rgb[a] colors. Skipped.\n");
	break;
      case 147:
	fprintf(stderr, "Error on regex for hex colors. Skipped.\n");
	break;
      default:
	fprintf(stderr, "Error on regex for color %s. Skipped.\n", strarr[i]);
	break;
      }
	regex.re_nsub = 100;
    }
    regexarr[i] = regex;
  }

  return regexarr;
}


void process_colors(char *csspath, regex_t *regexarr){
  
  FILE *fp = fopen(csspath, "r");
  if (!fp) {
    perror("fopen");
    return;
  }
  char buff[8193];
  int numbytes = 0;
  char *str;
  char *regstart; 
  char *mallocolor; 
  regmatch_t pmatch[1]; 
  regoff_t len; 
  stack s = 0; 
  
  while((numbytes = fread(buff, 1, 8192, fp))){

    if((numbytes < 8192) && ferror(fp)){
      fprintf(stderr, "An error has occurred while reading from file. Not all contents will be processed.\n");
    }

    buff[numbytes] = '\0';
    
    //detect and remove comments
    //MDN(Comments-CSS):"As with most programming languages that use the /* */ comment syntax, comments cannot be nested. In other words, the first instance of */ that follows an instance of /* closes the comment"
    str = buff;
    for(int k = 0; ; k++){
      if(regexec(&regexarr[COMMENT_REGEX_INDEX], str, 1, pmatch, 0)){ 
	break;
      }
      regstart = str + pmatch[0].rm_so;
      len = pmatch[0].rm_eo - pmatch[0].rm_so;
      //start at str+pmatch[0].rm_so and replace len characters with space
      for(; len != 0; len--, regstart++){
	*regstart = ' ';
      }
      str += pmatch[0].rm_eo;
    }
    
    //detect and remove lines that don't contain a colon
    str = buff; 
    for(int x = 0; ; x++){
      if(regexec(&regexarr[COLON_REGEX_INDEX], str, 1, pmatch, 0)){ 
	break;
      }
      regstart = str + pmatch[0].rm_so;
      len = pmatch[0].rm_eo - pmatch[0].rm_so;
      //start at str+pmatch[0].rm_so and replace len characters with space
      for(; len != 0; len--, regstart++){
	*regstart = ' ';
      }
      str += pmatch[0].rm_eo;
    }
 
    //loop through chunk and regex for colors. add matches to linkedlist. 
    //loop through color regexes
    for(int i = 0; i < COLON_REGEX_INDEX; i++){
      //filter out bad regexes
      if(regexarr[i].re_nsub != 100){
	str = buff;
        //loop through occurrences of regex, if any
	for(int j = 0; ; j++){
	  if(regexec(&regexarr[i], str, 1, pmatch, 0)){
	    break;
	  }
	  regstart = str + pmatch[0].rm_so;
	  len = pmatch[0].rm_eo - pmatch[0].rm_so;
	  mallocolor = malloc((len+1));
	  if(mallocolor == 0){
	    fprintf(stderr, "Error: An instance of color %.*s was found but storing it failed. Skipping.\n", len, regstart);
	    str += pmatch[0].rm_eo;
	    continue;
	  }
	  strncpy(mallocolor, regstart, len);
	  mallocolor[len] = '\0';
	  stack_push(&s, mallocolor);
	  str += pmatch[0].rm_eo;
	}  //end of color occurrences loop
      } //end of test if
    } //end of color regex loop
  } //end of chunk loop
   
  if(fclose(fp)){
    perror("fclose");
  }

  stack_process(&s);   

}


void destroy_regexes(regex_t *regexarr){
  for(int i = 0; i < REGEX_COUNT; i++){
      regfree(&regexarr[i]);
  }
  free(regexarr);
}


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
