#define _DEFAULT_SOURCE  //needed for dirent.h's d_type constants & for PATH_MAX
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fts.h>
#include <fcntl.h>  //needed for AT_SYMLINK_NOFOLLOW, the only fstatat() flag included in POSIX
#include "../include/storepaths.h"


#define FILETYPE_SWITCH			\
  char *filetype1, *filetype2;			\
  int ftlength1, ftlength2;			\
switch(filetype){				\
  case 0: \
   filetype1 = ".html"; \
   ftlength1 = strlen(filetype1); \
   filetype2 = ".php"; \
   ftlength2 = strlen(filetype2); \
   break; \
  case 1: \
    filetype1 = ".css"; \
    ftlength1 = strlen(filetype1); \
    filetype2 = 0; \
    ftlength2 = 0; \
    break; \
  default: \
    fprintf(stderr, "Wrong file type.\n"); \
    return 0; \
    break; \
  }


//return filtered contents of directory non-recursively.
char ** store_dir(const char *path, const int filetype, int *arrlen){
  struct dirent *entry;
  DIR *dp;
  int length = 0;
  int i = 0;
  int pathlength = strlen(path);
  char *suffix;
  char **filesarr;
  char *start;

  //a switch statement that expresses file types as strings, instead of numbers, and stores them and their lengths in variables. 
  FILETYPE_SWITCH;
  
  dp = opendir(path);
  if(dp == NULL){
    perror("opendir");
    return 0; 
  }
  
  //obtain directory descriptor for use in stat fallback code  
  int dfd = dirfd(dp);
  
   //note: realloc() isn't realistic here because you're dealing with variable-length strings, not integers or characters.
   while((entry = readdir(dp))){
     #ifdef _DIRENT_HAVE_D_TYPE
     if(entry->d_type == DT_REG && (((suffix = strstr(entry->d_name, filetype1)) && strlen(suffix) == ftlength1) || (filetype2 && (suffix = strstr(entry->d_name, filetype2)) && strlen(suffix) == ftlength2))){
      length += pathlength + strlen(entry->d_name) + 1;
      *arrlen += 1;
    } else if(entry->d_type == DT_UNKNOWN)
    #endif
      {
	//fallback for when there's no d_type field or d_type is DT_UNKNOWN
	struct stat sb;
	if(fstatat(dfd, entry->d_name, &sb, AT_SYMLINK_NOFOLLOW) == -1) {
            perror("fstatat");
           }
        if((sb.st_mode & S_IFMT) == S_IFREG && (((suffix = strstr(entry->d_name, filetype1)) && strlen(suffix) == ftlength1) || (filetype2 && (suffix = strstr(entry->d_name, filetype2)) && strlen(suffix) == ftlength2))) {
                length += pathlength + strlen(entry->d_name) + 1;
		*arrlen += 1;
        }
      }
  }

  if(*arrlen == 0){
    fprintf(stderr, "The directory %s doesn't contain any files of the appropriate file type.\n", path);
    return 0;
  }
  
  filesarr = malloc(sizeof(char *) * *arrlen + length);
  if(filesarr == 0){
    fprintf(stderr, "Allocation failed.\n");
    return 0; 
  }
  
  start = (char *)(filesarr + *arrlen);
  
  rewinddir(dp);  
  while((entry = readdir(dp))){
    #ifdef _DIRENT_HAVE_D_TYPE
    if(entry->d_type == DT_REG && (((suffix = strstr(entry->d_name, filetype1)) && strlen(suffix) == ftlength1) || (filetype2 && (suffix = strstr(entry->d_name, filetype2)) && strlen(suffix) == ftlength2))){
      strcat(strcpy(start, path), entry->d_name);
      filesarr[i] = start;
      start += strlen(start) + 1;
      i++;
    } else if(entry->d_type == DT_UNKNOWN)
      #endif 
      {
	//fallback for when there's no d_type field or d_type is DT_UNKNOWN
	struct stat sb;
	if(fstatat(dfd, entry->d_name, &sb, AT_SYMLINK_NOFOLLOW) == -1) {
            perror("fstatat");
           }
        if((sb.st_mode & S_IFMT) == S_IFREG && (((suffix = strstr(entry->d_name, filetype1)) && strlen(suffix) == ftlength1) || (filetype2 && (suffix = strstr(entry->d_name, filetype2)) && strlen(suffix) == ftlength2))) {
                strcat(strcpy(start, path), entry->d_name);
		filesarr[i] = start;
		start += strlen(start) + 1;
		i++;
        }
      }
  }
  
  closedir(dp);

  return filesarr;
}


//return filtered contents of directory recursively
char **store_dir_r(const char *path, const int filetype, int *arrlen){
  char *suffix;
  int pathslen = 0;
  int i = 0;

  //a switch statement that expresses file types as strings, instead of numbers, and stores them and their lengths in variables. 
  FILETYPE_SWITCH;
  
   char **patharr = malloc(sizeof(char *) * 2 + strlen(path) + 1);
   if(patharr == 0){
     fprintf(stderr, "Allocation failed.\n");
     return 0;
   }
   char *top = (char *) (patharr + 2);
   strcpy(top, path);
   patharr[0] = top;
   patharr[1] = 0;
   FTS *hndl1 = fts_open(patharr, FTS_PHYSICAL, NULL);
   FTSENT *finfo1;
   while((finfo1 = fts_read(hndl1))){
     if(finfo1->fts_info == FTS_F && (((suffix = strstr(finfo1->fts_name, filetype1)) && strlen(suffix) == ftlength1) || (filetype2 && (suffix = strstr(finfo1->fts_name, filetype2)) && strlen(suffix) == ftlength2))){
       *arrlen += 1;
       pathslen += strlen(finfo1->fts_path)+1;
     }
   }
   if(fts_close(hndl1) == -1){
     fprintf(stderr, "Error on closing file hierarchy stream.\n");
   }
   char **filesarr = malloc(sizeof(char *) * *arrlen + pathslen);
   if(filesarr == 0){
     fprintf(stderr, "Allocation failed.\n");
     free(patharr);
     return 0;
   }
   char *start = (char *)(filesarr + *arrlen);
   FTS *hndl2 = fts_open(patharr, FTS_PHYSICAL, NULL);
   FTSENT *finfo2;
   while((finfo2 = fts_read(hndl2))){
     if(finfo2->fts_info == FTS_F && (((suffix = strstr(finfo2->fts_name, filetype1)) && strlen(suffix) == ftlength1) || (filetype2 && (suffix = strstr(finfo2->fts_name, filetype2)) && strlen(suffix) == ftlength2))){
       strcpy(start, finfo2->fts_path);
       filesarr[i] = start;
       start += strlen(finfo2->fts_path) + 1;
       i++;
     }
   }
   if(fts_close(hndl2) == -1){
     fprintf(stderr, "Error on closing file hierarchy stream.\n");
   }
   free(patharr);
   return filesarr;
}


//receive path and return array of contents filtered by filetype (recursion, no recursion, or regular file)
char ** files_arr(char *path, const int filetype, int *arrlen, int recursion){
 struct stat sb;
 char **filesarr;
 if(stat(path, &sb) == 0){
      if(S_ISDIR(sb.st_mode) && recursion == 0){
	char *slash = "/";
	char *slashpath = malloc(strlen(path)+2);
	strcat(strcpy(slashpath, path), slash);
	filesarr = store_dir(slashpath, filetype, arrlen);
	free(slashpath);
      }
      else if(S_ISDIR(sb.st_mode) && recursion){
	filesarr = store_dir_r(path, filetype, arrlen);
      }
      else if(S_ISREG(sb.st_mode)){
	char *suffix;
	
	 //a switch statement that expresses file types as strings, instead of numbers, and stores them and their lengths in variables. 
	FILETYPE_SWITCH;
	
	if(((suffix = strstr(path, filetype1)) && strlen(suffix) == ftlength1) || (filetype2 && (suffix = strstr(path, filetype2)) && strlen(suffix) == ftlength2)){
	  *arrlen = 1;
	  filesarr = malloc(sizeof(char *) + strlen(path) + 1);
	  if(filesarr == 0){
	    fprintf(stderr, "Allocation failed.\n");
	    return 0;
	  }
	char *top = (char *) (filesarr + 1);
	strcpy(top, path);
	filesarr[0] = top;
	} else {
	  fprintf(stderr, "No file of the appropriate file type was provided.\n");
	  return 0;
	}
      } else {
	fprintf(stderr, "The path[s] you provide should be either of a directory or of a regular file.\n");
	return 0;
      }
    } else {
      fprintf(stderr, "Check the path[s] you provided.\n");
      return 0;
    }

 return filesarr;
}
