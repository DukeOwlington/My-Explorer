#include "helpfunc.h"
/*Add a slash at the beginning and end of adir[].
 The function first check if slash exists in adir[] and put one where necessary.
 */
char *CreateFullPath(char path[], char *name) {
  int path_size = strlen(path) + strlen(name) + 1;
  char *exec_file = (char *) malloc(path_size * sizeof(char));
  snprintf(exec_file, path_size, "%s%s", path, name);
  return exec_file;
}
int AddSlash(char path[]) {
  int length;
  length = strlen(path);
  int i, j;
  if (path[0] == '/' && path[length - 1] == '/')
    return 0;
  j = 0;
  if (path[length - 1] != '/') {
    strncat(path, "/", 1);
    length++;
  }
  char slashed[length];
  if (path[0] != '/') {
    slashed[0] = '/';
    j++;
  }
  for (i = j; i <= length; i++)
    slashed[i] = path[i - j];
  slashed[i] = '\0';
  strcpy(path, slashed);
  return 0;
}

/*Strip from string anything that comes after the first char delim.
 Return an integer corresponding to the position of delim in the string.
 */
int Split(char delim, char *string) {
  if (strcmp(string, "/") == 0)
    return 0;
  int lastPos;
  lastPos = strlen(string);
  int lastDelim;
  int i = 0;
  char c;
  for (int pos = 0; pos < lastPos - 1; pos++) {
    c = string[pos];
    if (c == delim) {
      i++;
      lastDelim = pos;
    }
  }
  string[lastDelim] = '\0';
  return lastDelim;
}
