#define _DEFAULT_SOURCE  //needed for dirent.h's d_type constants & for PATH_MAX
#include <stdio.h>
#include <string.h>
#include <limits.h>   //needed for PATH_MAX (note: added in POSIX) 
#include <stdlib.h>
#include <libxml/HTMLtree.h>
#include <libxml/HTMLparser.h>
#include <regex.h>
#include "../include/storepaths.h" //paths storage module
#include "../include/corefuns.h" //features functions module"

enum filetypes {HTMLPHP, CSS};

int main(int argc, char **argv){
  
  const char *unused = "--unused"; 
  const char *tags = "--style_tags";
  const char *atts = "--style_atts";
  const char *colors = "--colors";
  const char *longhelp = "--help";
  const char *shorthelp = "-h";
  const char *recursion = "-r";
  
  //declared here because of limitations of block scope
  char *path;
  char *htmlpath;
  char *csspath;
  
  //declared here because they can't be declared after case labels since declarations aren't statements
  char **htmlarr;
  char **cssarr;
  
  int opt = 0;
  int pathrecursion = 0;
  int htmlrecursion = 0;
  int cssrecursion = 0;
  int htmlarrlen = 0;
  int cssarrlen = 0;

  //VALIDATION & INITIALIZATION (2 | 3 | 3+1 - 4 | 4+1 - 4+1 | 4+2)
  switch(argc){
   case 2:
     if(!strcmp(longhelp, argv[1]) || !strcmp(shorthelp, argv[1])){
       printf("Usage: %s [%s] [%s [%s] HTMLPATH [%s] CSSPATH] [%s [%s] HTMLPATH] [%s [%s] HTMLPATH] [%s [%s] CSSPATH]\n\n%s %s\tShow this screen.\n%s\tShow paths of CSS files that contain unused classes, along with the names of those classes.\n%s\tShow paths of HTML and PHP files in which the style tag is used.\n%s\tShow paths of HTML and PHP files in which the 'style' attribute is used, along with a count of the elements that contain the attribute in each file.\n%s\tShow paths of CSS files that contain repeated colors, along with the names of those colors and their occurence frequency.\n%s\t\tSearch recursively.\n", argv[0], shorthelp, unused, recursion, recursion, tags, recursion, atts, recursion, colors, recursion, longhelp, shorthelp, unused, tags, atts, colors, recursion);
       return 0;
     } else {
       fprintf(stderr, "Usage: %s [%s] [%s [%s] HTMLPATH [%s] CSSPATH] [%s [%s] HTMLPATH] [%s [%s] HTMLPATH] [%s [%s] CSSPATH]\n", argv[0], shorthelp, unused, recursion, recursion, tags, recursion, atts, recursion, colors, recursion);
       exit(EXIT_FAILURE);
     }
     break;
   case 3:
     (!strcmp(tags, argv[1]))? (opt=2) : (!strcmp(atts, argv[1]))? (opt=3) : (!strcmp(colors, argv[1]))? (opt=4) : (fprintf(stderr, "Usage: %s [%s] [%s [%s] HTMLPATH [%s] CSSPATH] [%s [%s] HTMLPATH] [%s [%s] HTMLPATH] [%s [%s] CSSPATH]\n", argv[0], shorthelp, unused, recursion, recursion, tags, recursion, atts, recursion, colors, recursion));   //ugly, but efficient
     if(strlen(argv[2]) < PATH_MAX){
       path = argv[2];
     } else {
       fprintf(stderr, "Path provided is too long!\n");
       exit(EXIT_FAILURE);
     }
    break;
  case 4:
    if(!strcmp(unused, argv[1]) && strcmp(recursion, argv[2])){
      if(strlen(argv[2]) < PATH_MAX && strlen(argv[3]) < PATH_MAX){
       htmlpath = argv[2];
       csspath = argv[3];
     } else {
       fprintf(stderr, "Path[s] provided too long!\n");
       exit(EXIT_FAILURE);
     }
      opt = 1;
    } else if(!strcmp(recursion, argv[2])){
      (!strcmp(tags, argv[1]))? (opt=2) : (!strcmp(atts, argv[1]))? (opt=3) : (!strcmp(colors, argv[1]))? (opt=4) : (fprintf(stderr, "Usage: %s [%s] [%s [%s] HTMLPATH [%s] CSSPATH] [%s [%s] HTMLPATH] [%s [%s] HTMLPATH] [%s [%s] CSSPATH]\n", argv[0], shorthelp, unused, recursion, recursion, tags, recursion, atts, recursion, colors, recursion));
       if(strlen(argv[3]) < PATH_MAX){
	 path = argv[3];
       } else {
	 fprintf(stderr, "Path provided is too long!\n");
	 exit(EXIT_FAILURE);
       }
       pathrecursion = 1;
	} else {
      fprintf(stderr, "Usage: %s [%s] [%s [%s] HTMLPATH [%s] CSSPATH] [%s [%s] HTMLPATH] [%s [%s] HTMLPATH] [%s [%s] CSSPATH]\n", argv[0], shorthelp, unused, recursion, recursion, tags, recursion, atts, recursion, colors, recursion);
      exit(EXIT_FAILURE);
    }
     break;
  case 5:
    if(!strcmp(unused, argv[1]) && !strcmp(recursion, argv[2])){
      if(strlen(argv[3]) < PATH_MAX && strlen(argv[4]) < PATH_MAX){
       htmlpath = argv[3];
       csspath = argv[4];
      } else {
       fprintf(stderr, "Path[s] provided too long!\n");
       exit(EXIT_FAILURE);
      }
	htmlrecursion = 1;
	opt = 1;
      } else if(!strcmp(unused, argv[1]) && !strcmp(recursion, argv[3])) {
      if(strlen(argv[2]) < PATH_MAX && strlen(argv[4]) < PATH_MAX){
	htmlpath = argv[2];
	csspath = argv[4];
      } else {
	fprintf(stderr, "Path[s] provided too long!\n");
	exit(EXIT_FAILURE);
      }
	cssrecursion = 1;
	opt = 1;
      } else {
	fprintf(stderr, "Usage: %s [%s] [%s [%s] HTMLPATH [%s] CSSPATH] [%s [%s] HTMLPATH] [%s [%s] HTMLPATH] [%s [%s] CSSPATH]\n", argv[0], shorthelp, unused, recursion, recursion, tags, recursion, atts, recursion, colors, recursion);
	exit(EXIT_FAILURE);
      }
    break;
  case 6:
    if(!strcmp(unused, argv[1]) && !strcmp(recursion, argv[2]) && !strcmp(recursion, argv[4])){
	if(strlen(argv[3]) < PATH_MAX && strlen(argv[5]) < PATH_MAX){
	  htmlpath = argv[3];
	  csspath = argv[5];
	} else {
	  fprintf(stderr, "Path[s] provided too long!\n");
	  exit(EXIT_FAILURE);
	}
	htmlrecursion = 1;
	cssrecursion = 1;
	opt = 1;
    } else {
	fprintf(stderr, "Usage: %s [%s] [%s [%s] HTMLPATH [%s] CSSPATH] [%s [%s] HTMLPATH] [%s [%s] HTMLPATH] [%s [%s] CSSPATH]\n", argv[0], shorthelp, unused, recursion, recursion, tags, recursion, atts, recursion, colors, recursion);
	exit(EXIT_FAILURE);
    }
    break;
  default:
    fprintf(stderr, "Usage: %s [%s] [%s HTMLPATH CSSPATH] [%s HTMLPATH] [%s HTMLPATH] [%s CSSPATH]\n", argv[0], shorthelp, unused, tags, atts, colors);
    exit(EXIT_FAILURE);
    break;
  }
  
  //FUNCTION CALLS
 switch(opt){
  case 1:
    htmlarr = files_arr(htmlpath, HTMLPHP, &htmlarrlen, htmlrecursion);
    cssarr = files_arr(csspath, CSS, &cssarrlen, cssrecursion);
    if(htmlarr == 0 || cssarr == 0){
      fprintf(stderr, "Error: Gathering file names has failed.\n");
      exit(EXIT_FAILURE);
      }
    for(int i = 0; i < cssarrlen; i++){
      puts(cssarr[i]);
      free(cssarr[i]);
    }
    free(cssarr); 
    for(int j = 0; j < htmlarrlen; j++){
      free(htmlarr[j]);
    }
    free(htmlarr); 
    break;
  case 2:
    htmlarr = files_arr(path, HTMLPHP, &htmlarrlen, pathrecursion);
    if(htmlarr == 0){
      fprintf(stderr, "Error: Gathering file names has failed.\n");
      exit(EXIT_FAILURE);
    }
    style_tag_paths(htmlarr, htmlarrlen);    
    free(htmlarr);
    break;
  case 3:
    htmlarr = files_arr(path, HTMLPHP, &htmlarrlen, pathrecursion);
    if(htmlarr == 0){
      fprintf(stderr, "Error: Gathering file names has failed.\n");
      exit(EXIT_FAILURE);
    }
    style_att_paths(htmlarr, htmlarrlen);
    free(htmlarr);
    break;
  case 4:
    cssarr = files_arr(path, CSS, &cssarrlen, pathrecursion);
    if(cssarr == 0){
      fprintf(stderr, "Error: Gathering file names has failed.\n");
      exit(EXIT_FAILURE);
    }
    regex_t *regexarr = create_regexes();
    if(regexarr == 0){
      fprintf(stderr, "Error: Failure on creating regexes.\n");
      free(cssarr);
      exit(EXIT_FAILURE);
    }
    for(int i = 0; i < cssarrlen; i++){
      puts(cssarr[i]);
      process_colors(cssarr[i], regexarr);
      putchar('\n');
      free(cssarr[i]);
    }
    destroy_regexes(regexarr);
    free(cssarr);
    break;
  default:
    exit(EXIT_FAILURE);
    break;
  }
  
  return 0;
}
