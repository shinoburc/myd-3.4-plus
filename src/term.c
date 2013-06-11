/* term.c
 * Copyright (C) 2000-2001 Hironori FUJII
 */

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include "term.h"

static struct termios oterm, nterm;
static struct timeval timeout = {0, 500000};

int term_width;
int term_height;

void sig_winch(int sig){
  struct winsize ws;
  ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
  term_height = ws.ws_row;
  term_width = ws.ws_col;
}

int term_init(void){

  signal(SIGWINCH, sig_winch);

  if(!isatty(STDIN_FILENO)){
    fputs("not a tty.\n", stderr);
    return 100;
  }

  setvbuf(stdout, 0, _IOFBF, 1024);

  tcgetattr(STDIN_FILENO, &nterm);
  oterm = nterm;
  nterm.c_lflag &= ~ECHO;
  nterm.c_lflag |= ECHONL;
  /* Raw Mode */
  nterm.c_lflag &= ~ICANON;
  nterm.c_cc[VTIME] = 0;
  nterm.c_cc[VMIN] = 0;

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &nterm);

  tcgetattr(STDIN_FILENO, &nterm);
  if(nterm.c_lflag & ECHO){
    fprintf(stderr, "Can not disable ECHO\n");
    return 100;
  }


  /* get window size */
  sig_winch(0);

  return 0;
}

void term_end(void){
  tcsetattr(STDIN_FILENO, TCSANOW, &oterm);
}


int term_getch(void){
  fd_set rfds;
  int rv;
  char c;
  char buff[3];
  struct timeval tv = timeout;

  FD_ZERO(&rfds);
  FD_SET(0, &rfds);

  rv = select(1, &rfds, 0, 0, &tv);

  if (!rv)
    return -1;

  read(STDIN_FILENO, &c, 1);

  if(c != 27)
    return c;

  tv.tv_sec = 0;
  tv.tv_usec = 10;

  rv = select(1, &rfds, 0, 0, &tv);

  if (!rv)
    return 27;

  read(STDIN_FILENO, buff, sizeof(buff));

  switch(buff[0]){
  case '[':
    switch(buff[1]){
    case 'A':
      return KEY_UP;
    case 'B':
      return KEY_DOWN;
    case 'C':
      return KEY_RIGHT;
    case 'D':
      return KEY_LEFT;
    case '5':
      return KEY_PPAGE;
    case '6':
      return KEY_NPAGE;
    }
  }

  return 27;
}

void term_timeout(int msec){
  timeout.tv_sec = msec / 1000;
  timeout.tv_usec = (msec % 1000) * 1000;
}

/* 画面を消去して、カーソルを左上隅へ移動する */
void term_clear(){
  fputs("\033[2J\033[1;1H", stdout);
}

/* カーソルを(x,y)へ移動する (0,0)が左上隅 */
void term_move(int x, int y){
  fprintf(stdout, "\033[%d;%dH", y+1, x+1);
}

void term_move_x(int x){
  putchar('\r');
  term_move_r(x, 0);
}

void term_move_y(int y){
  fprintf(stdout, "\033[%dd", y+1);
}

/* カーソルの現在地からの相対移動  */
void term_move_r(int x, int y){
  if(x > 0)
    fprintf(stdout, "\033[%dC", x);
  else if(x < 0)
    fprintf(stdout, "\033[%dD", -x);

  if(y > 0)
    fprintf(stdout, "\033[%dB", y);
  else if(y < 0)
    fprintf(stdout, "\033[%dA", -y);

}

void term_attr_reset(){
  fputs("\033[0m", stdout);
}

void term_attr_set(char *str){
  fputs("\033[0;", stdout);
  fputs(str, stdout);
  fputs("m", stdout);
}
