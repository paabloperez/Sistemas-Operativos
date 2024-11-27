#ifndef COMANDOSP1_H
#define COMANDOSP1_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/utsname.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <libgen.h>


void Cmd_cwd();
void Cmd_makefile(char *tr[]);
void Cmd_makedir(char *tr[]);
void Cmd_listFile(char *tr[]);
void Cmd_listdir(char *tr[]);
void Cmd_reclist(char *tr[]);
void Cmd_revlist(char *tr[]);
void Cmd_erase(char *tr[]);
void Cmd_delrec(char *tr[]);

#endif //COMANDOSP1_H
