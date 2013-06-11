/* term.h
 * Copyright (C) 2000-2001 Hironori FUJII
 */

extern int term_width;
extern int term_height;

#define ERR       -1
#define KEY_UP    -2
#define KEY_DOWN  -3
#define KEY_RIGHT -4
#define KEY_LEFT  -5
#define KEY_PPAGE -6
#define KEY_NPAGE -7

int  term_init(void);
void term_end(void);
int  term_getch(void);
void term_timeout(int msec);

void term_clear(void);
void term_move(int x, int y);
void term_move_x(int x);
void term_move_y(int y);
void term_move_r(int x, int y);
void term_attr_reset(void);
void term_attr_set(char *str);
