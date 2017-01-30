#ifndef FILEMANAGER_H_INCLUDED
#define FILEMANAGER_H_INCLUDED

#define AT_DIR -1
#define MAXDIRLIST 3000
#define MAXFILELIST 3000
#define MENUW 75            //The width of the directory and file listing.
#define ROOT "/"
#define ENTER '\n'
#define STRLEN 1024
#define MENUY 2
#define MENUX 2
#define CYAND 8
#define YELLOWD 9

#include <dirent.h>
#include <errno.h>
#include <ncurses.h> //includes stdio.h and stdbool.h
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>  //for file info
#include <unistd.h>    //exec

extern int errno;
extern void perror();

WINDOW *frame, *listmenu;

typedef struct {
  int menuitem;         //index of directory or filelist
  int winmarker;        //-1=AT_DIR, 1=AT_FILE
  int linemarker;       //Goes from 0 to linecount
  int linemax;          //or FILEMAX
  int linecount;        //from directories or files list length
  int arrowcounter;  //Switch from arrowcounterdir or arrowcounterfile. 0 to linemax
} Cursor;

typedef struct {
  char dirname[STRLEN];
  int dir_count;
  int file_count;
  long dir_size;
} DirectoryStat;

typedef struct {
  char *name;
  long u_size;
  char modtime[10];
} Content;

enum direction {
  UP = -1,
  DOWN = 1,
  SAME = 0
};

Cursor SetCursor(int direction, int selection, Cursor c);
int DrawList(Content *directories, Content *files, DirectoryStat dir_stat,
             char path[], int item, WINDOW *w, int fromline);
DirectoryStat CreateList(Content *directories, Content *files, char path[]);
int DrawFrame(WINDOW *frame, char *currentDir);
int InitializeColors(void);
Cursor InitializeCursor(Cursor cursor);
#endif // FILEMANAGER_H_INCLUDED
