/*
 *  MyD
 *   main.c
 *  Copyright (C) 2000-2001 Hironori FUJII 
 * 
 */
#include <config.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#include "myd.h"
#include "edit.h"
#include "term.h"

#ifdef USE_XSELECTION
#include "getsel.h"
#endif

#define PROGRAM "MyD"

MYD myd;
char *prompt = PROGRAM "> ";
#define SCROLL_STEP 4

/* テーマ番号 */
int theme_no = DEFAULT_THEME;

char * color_word[] = {
  "1",
  "34",
  "1;36"
};


void usage(char *prog){
  printf(
	 PROGRAM " ver " VERSION "\n"
	 "dictionary look up tool\n"
	 "usage: %s [options]\n"
	 " options\n"
	 "     [-d <dic_filename>]\n"
	 "     [-t <theme_no>] select a theme\n"
	 "     [-1] print random one dictionary and exit.\n"
	 "     [-h] print the help\n"
	 "     [-v] print the version\n"
	 , prog);
  exit(0);
}


inline int is_kanji(char c){
  return c & 0x80;
}

char * putkchar(char *str, int *x, int *y){
  if(is_kanji(*str)){
    if(*x == term_width - 1){
      putchar(' ');
      (*y) ++;
      (*x) = 0;
      return str;
    }
    putchar(*str++);
    (*x) ++;
    putchar(*str++);
    (*x) ++;
  }else{
    putchar(*str++);
    (*x) ++;
  }
  if(*x >= term_width){
    (*x) -= term_width;
    (*y) ++;
  }
  return str;
}

void move_cursor(int cursor){
  term_move(strlen(prompt) + cursor, 0);
  fflush(stdout);
}

void print_prompt(char *str, int cursor){
  term_move(0, 0);
  
  fputs(prompt, stdout);
  fputs(str, stdout);
  move_cursor(cursor);
}

void print_status(MYD myd, int n_match){
  int i;
  char str[256];

  snprintf(str, sizeof(str), "%d/%d", n_match, myd_n_index(myd));
  str[255] = '\0'; 

  term_move(0, term_height-1);
  term_attr_set("7");

  i = term_width - 1 - strlen(str);
  for(;i>0;i--)
    putchar(' ');
  fputs(str, stdout);

  term_attr_reset();
}

void display_word(char *str, int *x, int *y){
  term_attr_set(color_word[theme_no]);
  while(*str){
    if(*y >= term_height - 1)
      break;
    str = putkchar(str, x, y);
  }
  term_attr_reset();
}

void display_mean(char *str, int *x, int *y){
  int tab_stop = term_width / 2;
  if(tab_stop > 8)
    tab_stop = 8;
  putkchar(" ", x, y);
  while(*str){
    if(*y >= term_height - 1)
      break;
    while(*x < tab_stop)
      putkchar(" ", x, y);
    str = putkchar(str, x, y);
  }

  putchar('\n');
  if(*x != 0){
    (*x) = 0;
    (*y) ++;
  }
}

void display(MYD myd, int index, int n_match, int scroll){
  int i;
  int x, y;

  term_clear();
  print_prompt(edit.text, edit.cursor);

  term_move(0, 1);
  x = 0;
  y = 1;

  i = scroll;

  while(i < n_match){
    /*  単語の表示  */
    display_word(myd_key(myd, index+i), &x, &y);
    
    if(y >= term_height - 1)
      break;

    /* 意味の表示  */
    display_mean(myd_text(myd, index+i), &x, &y);

    i ++;

    if(y >= term_height - 1)
      break;
  }

  print_status(myd, n_match);
}

void main_loop(){
  char *cs;
  int index;
  int n_match;
  int scroll = 0;

#ifdef USE_XSELECTION
  cs = check_sel();
#endif
  if(cs)
    edit_set_text(cs);

  n_match = myd_bsearch(myd, edit.text, &index);
  display(myd, index, n_match, scroll);
  print_prompt(edit.text, edit.cursor);

  while(1){ 
    int c;

    c = term_getch();

    if(isprint(c)){
      edit_ins_char(c);
      n_match = myd_bsearch(myd, edit.text, &index);
      scroll = 0;
    }else{
      switch(c){
	/*
	 *    move cursor
	 */
      case 1:  /* ^A */
	edit_cur_head();
	move_cursor(edit.cursor);
	continue;
      case 2:  /* ^B */
      case KEY_LEFT:
	edit_cur_back();
	move_cursor(edit.cursor);
	continue;
      case 5:  /* ^E */
	edit_cur_tail();
	move_cursor(edit.cursor);
	continue;
      case 6:  /* ^F */
      case KEY_RIGHT:
	edit_cur_forward();
	move_cursor(edit.cursor);
	continue;


	/*
	 *    scroll
	 */
      case 16: /* ^P */
      case KEY_PPAGE:
      case KEY_UP:
	if(scroll == 0)
	  continue;
	scroll -= SCROLL_STEP;
	if(scroll < 0)
	  scroll = 0;
	break;
      case 14: /* ^N */
      case KEY_NPAGE:
      case KEY_DOWN:
	if(scroll == n_match - 1)
	  continue;
	scroll += SCROLL_STEP;
	if(scroll >= n_match)
	  scroll = n_match - 1;
	break;


	/*
	 *   delete text
	 */
      case 8:  /* ^H */
	edit_back_space();
	n_match = myd_bsearch(myd, edit.text, &index);
	scroll = 0;
	break;
      case 4:  /* ^D */
      case 0x7f:
	edit_del_char();
	n_match = myd_bsearch(myd, edit.text, &index);
	scroll = 0;
	break;
      case 21: /* ^U */
      case 10: /* ^J */
      case 13: /* ^M */
	edit_clear();
	n_match = myd_bsearch(myd, edit.text, &index);
	scroll = 0;
	break;
      case 11: /* ^K */
	edit_kill();
	n_match = myd_bsearch(myd, edit.text, &index);
	scroll = 0;
	break;

      case 20: /* ^T */
	edit_transpose();
	n_match = myd_bsearch(myd, edit.text, &index);
	scroll = 0;
	break;

      case 12: /* ^L */
	break;

	/*  Exit */
      case 24: /* ^X */
	return;
	
      case ERR: /* timeout */
#ifdef USE_XSELECTION
	cs = check_sel();
#endif
	if(cs){
	  edit_set_text(cs);
	  n_match = myd_bsearch(myd, edit.text, &index);
	  scroll = 0;
	  break;
	}else
	  continue;

      default:
	break;
      }
    }
    display(myd, index, n_match, scroll);
    print_prompt(edit.text, edit.cursor);
  }
}

void finish(int s){
#ifdef USE_XSELECTION
  get_sel_end();
#endif
  term_end();
  myd_close(myd);
  exit(s);
}

int main(int argc, char *argv[]){
  char dic_filename[1024];
  struct stat st;
  int update_interval = 300; /* milli second */
  int is_print_random = 0;

  snprintf(dic_filename, sizeof(dic_filename), "%s%s%s",
	  getenv("HOME"), "/", HOME_DIC_FILENAME);
  if(stat(dic_filename, &st) == -1){
    strcpy(dic_filename, SYSTEM_DIC_PATH);
  }

  while(1){
    int c;
    c = getopt(argc, argv, "d:t:1hvi:");
    if(c == -1)
      break;
    switch(c){
    case 'd':
      strcpy(dic_filename, optarg);
      break;
      
    case 't':
      theme_no = atoi(optarg);
      break;

    case 'i':
      update_interval = atoi(optarg);
      break;

    case '1':
      is_print_random = 1;
      break;
    case 'h':
      usage(argv[0]);
    case 'v':
      printf(PROGRAM " version " VERSION "\n");
      exit(0);
    case ':':
    case '?':
      exit(1);
    }
  }

  if(is_print_random){
    print_random(dic_filename);
    exit(0);
  }

  myd = myd_open(dic_filename);
  if(!myd){
    perror(dic_filename);
    exit(1);
  }

  term_init();
  term_timeout(update_interval);
#ifdef USE_XSELECTION
  get_sel_init(getenv("DISPLAY"));
#endif

  main_loop();

  finish(0);
  return 0;
}
