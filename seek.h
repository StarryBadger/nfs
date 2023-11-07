#ifndef __SEEK_H
#define __SEEK_H
void seek(command);
void lookFor(bool D, bool F, char *target, char *path);
int setSeekFlags(char *input, bool *F, bool *E, bool *D);
#endif