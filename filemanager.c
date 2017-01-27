#include "filemanager.h"
#include "helpfunc.h"

const char *quitMessage = "Press q for quit";

//Change cursor position
Cursor SetCursor(int direction, int selection, Cursor c) {
  switch (direction) {
    case DOWN:
      if (selection >= c.linecount) { //Bottom of dir or file list
        selection = 0;
        c.linemarker = 0;
      } else if (selection >= c.linemax) {
        if (selection >= c.linemax + c.linemarker)
          c.linemarker += c.linemax;
      }
      break;
    case UP:
      if (selection < 0) { //Beginning of dir or file list
        selection = c.linecount - 1;
        c.linemarker = c.linecount - c.linemax;
        if (c.linemarker < 0)
          c.linemarker = 0;
      } else if (selection < c.linemarker) {
        c.linemarker -= c.linemax;
        if (c.linemarker < 0)
          c.linemarker = 0;
      }
      break;
    case SAME:
      break;
  }
  c.menuitem = c.arrowcounter = selection;
  return c;
}

// Create directory and file lists
DirectoryStat CreateList(Content *directories, Content *files, char path[]) {
  struct dirent **namelist;
  struct stat statbuf;
  DirectoryStat dir_stat;
  char str[STRLEN];
  int n, m;
  int file_index = 0;
  int dir_index = 0;
  dir_stat.dir_size = 0;
  AddSlash(path);
  n = scandir(path, &namelist, 0, alphasort);
  if (n < 0)
    printf("scandir %s %s\n", path, strerror(errno));
  else {
    for (m = 1; m < n; m++) {
      snprintf(str, STRLEN, "%s%s", path, namelist[m]->d_name);
      if (lstat(str, &statbuf) != 0) {
        perror("In CreateList File not found");
        perror(str);
        exit(1);
      }
      if (S_ISDIR(statbuf.st_mode) == 1) {  // is directory
        directories[dir_index].name = strdup(namelist[m]->d_name);
        directories[dir_index].u_size = statbuf.st_size;

        strftime(directories[dir_index++].modtime, 10, "%d-%m-%y",
                 localtime(&(statbuf.st_ctime)));
        if (dir_index > MAXDIRLIST) {
          echo();
          endwin();
          perror("Directory too long");
          exit(EXIT_FAILURE);
        }

      } else {  // is file
        dir_stat.dir_size += statbuf.st_size;
        files[file_index].name = strdup(namelist[m]->d_name);
        files[file_index].u_size = statbuf.st_size;
        strftime(files[file_index++].modtime, 10, "%d-%m-%y",
                 localtime(&(statbuf.st_ctime)));
      }
      free(namelist[m]);
    }
    free(namelist);
    namelist = NULL;
    directories[dir_index].name = '\0';
    files[file_index].name = '\0';
  }
  dir_stat.dir_count = dir_index;
  dir_stat.file_count = file_index;
  strcpy(dir_stat.dirname, path);
  return dir_stat;
}

//Draw dir and file list
int DrawList(Content *directories, Content *files, DirectoryStat dir_stat,
             int item, WINDOW *w, int fromline) {
  const int MENUMAX = LINES - 3;
  int printline = 0;
  werase(w);
  int h;
  h = MENUMAX;
  wbkgd(listmenu,COLOR_PAIR(1));
  int file_index, dir_index;

  if (dir_stat.dir_count >= fromline) {
      dir_index = fromline;
      file_index = 0;
  } else {
      dir_index = dir_stat.dir_count;
      file_index = fromline - dir_stat.dir_count;
  }

  while (directories[dir_index].name != '\0' && printline < h) {
    if (dir_index == item)  // selected element
      wattron(w, COLOR_PAIR(2));

    mvwprintw(w, printline++, 2, "/%-42s\t%-12d\t%-10s",
              directories[dir_index].name, directories[dir_index].u_size,
              directories[dir_index].modtime);
    wattroff(w, COLOR_PAIR(2));
    dir_index++;
  }
  while (files[file_index].name != '\0' && printline < h) {
    if ((file_index + dir_index) == item)  // selected element
      wattron(w, COLOR_PAIR(2));

    mvwprintw(w, printline++, 2, "%-42s\t%-12d\t%-10s", files[file_index].name,
              files[file_index].u_size, files[file_index].modtime);
    wattroff(w, COLOR_PAIR(2));
    file_index++;
  }
  wrefresh(w);
  return 0;
}

//Draw frame
int DrawFrame(WINDOW *frame, char *currentDir) {

  werase(frame);
  box(frame, 0, 0);
  mvwprintw(frame, 0, COLS / 20, "%s", currentDir);
  wattron(frame, COLOR_PAIR(3));
  mvwprintw(frame, MENUY - 1, MENUY + 2, "%-43s\t%-12s\t%-11s", "Name",
            "  Size", "Modify date");
  wattroff(frame, COLOR_PAIR(3));
  wattron(frame, COLOR_PAIR(2));
  mvwprintw(frame, LINES - 1, COLS - strlen(quitMessage) - 2, "%s", quitMessage);
  wattroff(frame,COLOR_PAIR(2));
  wrefresh(frame);

  return 0;
}

//Initialize colors
int InitializeColors(void) {
  if (has_colors() == FALSE)
    return -1;

  start_color();
  init_pair(1,COLOR_WHITE,COLOR_BLUE);

  int err = init_color(CYAND, 0, 666, 666);
  init_color(YELLOWD, 980, 972, 427);

  if (err >= 0) {
    init_pair(2,COLOR_BLACK,CYAND);
    init_pair(3,YELLOWD,COLOR_BLUE);
  } else {
    init_pair(2,COLOR_BLACK,COLOR_CYAN);
    init_pair(3,COLOR_YELLOW, COLOR_BLUE);
  }
  return 0;
}

//Initialize cursor
Cursor InitializeCursor(Cursor cursor) {
  cursor.menuitem = 0;       //index of directory or filelist
  cursor.winmarker = AT_DIR;  //-1=AT_DIR
  cursor.linemarker = 0;     //Goes from 0 to linecount
  cursor.linecount = 0;      //from directories or files list length
  cursor.arrowcounter = 0;  //Switch from arrowcounterdir or arrowcounterfile. 0 to linemax

  return cursor;
}

int main() {
  initscr();

  Cursor cursor;
  DirectoryStat dir_stat;
  Content directories[MAXDIRLIST];
  Content files[MAXFILELIST];

  const int MENUHT = LINES - 4;
  const int MENUMAX = LINES - 3;

  int key = 0;
  char currentDir[200] = { 0 };   //To keep track of the current full path dir

  curs_set(0);

  cursor = InitializeCursor(cursor);
  InitializeColors();

  cursor.linemax = MENUMAX - 1;  //or FILEMAX

  // Create frame and color it
  frame = newwin(LINES, COLS, 0, 0);
  wbkgd(frame,COLOR_PAIR(1));
  DrawFrame(frame, ROOT);

  //Create menu, that contains directory and file lists
  listmenu = newwin(MENUHT, MENUW, MENUY, MENUX);
  dir_stat = CreateList(directories, files, ROOT);
  cursor.linecount = dir_stat.dir_count + dir_stat.file_count;
  strcpy(currentDir, ROOT);
  wattron(listmenu, A_BOLD);

  DrawList(directories, files, dir_stat, 0, listmenu, cursor.linemarker);
  keypad(listmenu, true);
  noecho();

  do {
    key = wgetch(listmenu);
    switch (key) {
      case KEY_PPAGE:
        cursor = SetCursor(DOWN, cursor.menuitem + MENUMAX, cursor);
        DrawList(directories, files, dir_stat, 0, listmenu, cursor.linemarker);
        break;
      case KEY_NPAGE:
        cursor = SetCursor(UP, cursor.menuitem - MENUMAX, cursor);
        DrawList(directories, files, dir_stat, cursor.menuitem, listmenu,
                 cursor.linemarker);
        break;
      case KEY_DOWN:
        if (cursor.winmarker == AT_DIR && dir_stat.dir_count == 0)
          break;
        cursor = SetCursor(DOWN, ++cursor.menuitem, cursor);
        DrawList(directories, files, dir_stat, cursor.menuitem, listmenu,
                 cursor.linemarker);
        break;
      case KEY_UP:
        cursor = SetCursor(UP, --cursor.menuitem, cursor);
        DrawList(directories, files, dir_stat, cursor.menuitem, listmenu,
                 cursor.linemarker);
        break;
      case ENTER:
        if (directories[0].name == '\0' || cursor.menuitem >= dir_stat.dir_count)
          break;
        cursor.linemarker = 0;
        cursor.arrowcounter = 0;
        if (strcmp(directories[cursor.menuitem].name, "..") != 0)
          strncat(currentDir, directories[cursor.menuitem].name,
                  strlen(directories[cursor.menuitem].name));
        else
          Split('/', currentDir);
        dir_stat = CreateList(directories, files, currentDir);
        cursor.linecount = dir_stat.dir_count + dir_stat.file_count;
        DrawFrame(frame, currentDir);
        DrawList(directories, files, dir_stat, 0, listmenu, cursor.linemarker);
        cursor.linemarker = 0;
        cursor.menuitem = 0;
        break;
      default:
        break;
    }

  } while (key != 'q');
  echo();
  delwin(listmenu);
  delwin(frame);
  endwin();
  return 0;
}
