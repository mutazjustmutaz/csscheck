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
  const char *filetype1, *filetype2;			\
switch(filetype){				\
  case 0: \
   filetype1 = ".html"; \
   filetype2 = ".php"; \
   break; \
  case 1: \
    filetype1 = ".css"; \
    filetype2 = "\0"; \
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
  int pathlength = strlen(path);
  char *lastdot;
  char **filesarr;
  char *mallocpath;
  int size = 1;

  //a switch statement that expresses file types as strings, instead of numbers, and stores them in variables. 
  FILETYPE_SWITCH;
  
  dp = opendir(path);
  if(dp == NULL){
    perror("opendir");
    return 0; 
  }

  filesarr = malloc(sizeof(char *) * size);
  if(filesarr == 0){
    fprintf(stderr, "Path storage failed.\n");
    closedir(dp);
    return 0; 
  }
  
  //obtain directory descriptor for use in stat fallback code  
  int dfd = dirfd(dp);
  
   while((entry = readdir(dp))){

     lastdot = strrchr(entry->d_name, '.');
     
     #ifdef _DIRENT_HAVE_D_TYPE
    if(entry->d_type == DT_REG && lastdot && (strcmp(lastdot, filetype1) == 0 || strcmp(lastdot, filetype2) == 0)){

      mallocpath = malloc(pathlength+strlen(entry->d_name)+1);
      if(mallocpath == 0){
	fprintf(stderr, "Storing path %s failed. Skipping.\n", entry->d_name);
	 continue;
      }	  
      strcat(strcpy(mallocpath, path), entry->d_name);
      //store pointer in dynamic array
      while(*arrlen >= size){
	 size *= 2; 
	 filesarr = realloc(filesarr, sizeof(char *) * size);
	 if(filesarr == 0){
	   fprintf(stderr, "Path storage failed\n");
	   //the other paths can't be freed because filesarr is what's used to keep track of them
	   free(mallocpath);
	   closedir(dp);
	   return 0;
	 }
      }
       
      filesarr[(*arrlen)++] = mallocpath;
      
    } else if(entry->d_type == DT_UNKNOWN)
    #endif
      {
	//fallback for when there's no d_type field or d_type is DT_UNKNOWN
	struct stat sb;
	if(fstatat(dfd, entry->d_name, &sb, AT_SYMLINK_NOFOLLOW) == -1) {
            perror("fstatat");
           }
        if((sb.st_mode & S_IFMT) == S_IFREG && lastdot && (strcmp(lastdot, filetype1) == 0 || strcmp(lastdot, filetype2) == 0)){
	  
                 mallocpath = malloc(pathlength+strlen(entry->d_name)+1);
	  if(mallocpath == 0){
	    fprintf(stderr, "Storing path %s failed. Skipping.\n", entry->d_name);
	    continue;
	  }	  
	  strcat(strcpy(mallocpath, path), entry->d_name);
	  //store pointer in dynamic array
	  while(*arrlen >= size){
	    size *= 2; 
	    filesarr = realloc(filesarr, sizeof(char *) * size);
	    if(filesarr == 0){
	      fprintf(stderr, "Path storage failed\n");
	      //the other paths can't be freed because filesarr is what's used to keep track of them
	      free(mallocpath);
	      closedir(dp);
	      return 0;
	    }
	  }
	  
	    filesarr[(*arrlen)++] = mallocpath;
	    
        }
      }
  }

  closedir(dp);
   
  if(*arrlen == 0){
    fprintf(stderr, "The directory doesn't contain any files of the appropriate file type.\n");
    free(filesarr);
    return 0;
  }
  
  filesarr = realloc(filesarr, sizeof(char *) * *arrlen);
  if(filesarr == 0){
    //the paths can't be freed because filesarr is what's used to keep track of them
    return 0;
  }

  return filesarr;
}


//return filtered contents of directory recursively
char **store_dir_r(char *path, const int filetype, int *arrlen){
  char *lastdot;
  char *mallocpath;

   int size = 1;
   char **filesarr  = malloc(sizeof(char *) * size);
   if(filesarr == 0){
     fprintf(stderr, "Paths storage failed.\n");
     return 0; 
   }
   
   char **patharr = malloc(sizeof(char *) * 2);
   if(patharr == 0){
     fprintf(stderr, "Allocation failed.\n");
     return 0;
   }
   patharr[0] = path;
   patharr[1] = 0;
   FTS *hndl = fts_open(patharr, FTS_PHYSICAL, NULL);
   FTSENT *finfo;

   //a switch statement that expresses file types as strings, instead of numbers, and stores them in variables. 
  FILETYPE_SWITCH;
   
   while((finfo = fts_read(hndl))){

     lastdot = strrchr(finfo->fts_name, '.');
     
     if(finfo->fts_info == FTS_F && lastdot && (strcmp(lastdot, filetype1) == 0 || strcmp(lastdot, filetype2) == 0)){

       mallocpath = malloc(finfo->fts_pathlen+1);
       if(mallocpath == 0){
	 fprintf(stderr, "Storing path %s failed. Skipping.\n", finfo->fts_path);
	 continue;
       }	  
       strcpy(mallocpath, finfo->fts_path);
       //store pointer in dynamic array
       while(*arrlen >= size){
	 size *= 2; 
	 filesarr = realloc(filesarr, sizeof(char *) * size);
	 if(filesarr == 0){
	   fprintf(stderr, "Paths storage failed\n");
	   //the other paths can't be freed because filesarr is what's used to keep track of them
	   free(mallocpath);
	   free(patharr);
	   if(fts_close(hndl) == -1){
	     fprintf(stderr, "Error on closing file hierarchy stream.\n");
	   } 
	   return 0;
	 }
       }
       
       filesarr[(*arrlen)++] = mallocpath;
       
     }
   }
   
   if(fts_close(hndl) == -1){
     fprintf(stderr, "Error on closing file hierarchy stream.\n");
   }
   
   free(patharr);

   if(*arrlen == 0){
     fprintf(stderr, "The directory doesn't contain any files of the appropriate file type.\n");
     free(filesarr);
     return 0;
   }

   filesarr = realloc(filesarr, sizeof(char *) * *arrlen);
   if(filesarr == 0){
     fprintf(stderr, "Allocation failed.\n");
     //the paths can't be freed because filesarr is what's used to keep track of them
     return 0;
   }
   
   return filesarr;
}


//receive path and return array of contents filtered by filetype (recursion, no recursion, or regular file)
char ** files_arr(char *path, const int filetype, int *arrlen, int recursion){
 struct stat sb;
 char **filesarr;
 if(stat(path, &sb) == 0){
      if(S_ISDIR(sb.st_mode) && recursion == 0){
	char *slashpath = malloc(strlen(path)+2);
	strcat(strcpy(slashpath, path), "/");
	filesarr = store_dir(slashpath, filetype, arrlen);
	free(slashpath);
      }
      else if(S_ISDIR(sb.st_mode) && recursion){
	filesarr = store_dir_r(path, filetype, arrlen);
      }
      else if(S_ISREG(sb.st_mode)){
	 //a switch statement that expresses file types as strings, instead of numbers, and stores them in variables. 
	FILETYPE_SWITCH;
	char *lastdot = strrchr(path, '.');
	if(lastdot && (strcmp(lastdot, filetype1) == 0 || strcmp(lastdot, filetype2) == 0)){
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
