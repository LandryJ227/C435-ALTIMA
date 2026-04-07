#pragma once
#ifndef C435_ALTIMA_ULTIMA_H
#define C435_ALTIMA_ULTIMA_H
#include <ncurses.h>
#include <pthread.h>
WINDOW *create_window(int height, int width, int starty, int startx);
void write_window(WINDOW * Win, const char* text);
void write_window(WINDOW * Win, int y, int x, const char* text);
inline int outputWriteLine = 3;

inline WINDOW * outputWin;
inline WINDOW * threadWin;
inline WINDOW * schedWin;
inline WINDOW * semaWin;
inline WINDOW * messWin;
inline pthread_mutex_t winMutex;
inline char tempStr[100];
class Ultima {

};



#endif //C435_ALTIMA_ULTIMA_H
