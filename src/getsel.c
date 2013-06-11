/* getsel.c
 *  Copyright (C) 2000-2001 Hironori FUJII
 */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>

#include <unistd.h>

#include "getsel.h"

static Display *x_display = 0;
static Window window;

/*
 *  セレクションを取得する。
 *  成功すれば1,失敗すれば0を返す。
 */
int get_sel(char *str, int size){
  unsigned char *pch;
  Atom actual_type_return;
  int actual_format_return;
  unsigned long nitems_return;
  unsigned long bytes_after_return;
  XEvent report;

  int i;

  if(!x_display)
    return 0;

  XConvertSelection(x_display, XA_PRIMARY, XA_STRING,
		    XA_STRING, window, CurrentTime);

  i = 0; 
  while(False == XCheckTypedEvent(x_display, SelectionNotify, &report)){
    /* kterm対策 : kterm はSelectionNotifyを送ってくれないこともあるようだ */
    i ++;
    if(i > 10)
      return 0;
    usleep(100);
  }

  if(report.type == SelectionNotify){

    if(report.xselection.property == None)
      return 0;

    XGetWindowProperty(x_display, window,
		       report.xselection.property,
		       0,              /*long_offset*/
		       size - 1,       /*long_length*/
		       True,           /*delete*/
		       AnyPropertyType,/*req_type*/
		       &actual_type_return,
		       &actual_format_return,
		       &nitems_return,
		       &bytes_after_return,
		       &pch);
    if(!nitems_return)
      return 0;

    strncpy(str, pch, size - 1);
    str[size - 1] = '\0';
    XFree(pch);

    return 1;
  }

  return 0;
}

int get_sel_init(char *display_name){
  int screen_num;

  x_display = XOpenDisplay(display_name);
  if(!x_display)
    return 0;

  screen_num = DefaultScreen(x_display);
  window = XCreateSimpleWindow(x_display, RootWindow(x_display, screen_num),
			       0, 0, 1, 1, 1, 
			       BlackPixel(x_display, screen_num),
			       WhitePixel(x_display, screen_num));

  return 1;
}

void get_sel_end(void){
  if(x_display){
    XCloseDisplay(x_display);
    x_display = 0;
  }
  return;
}


void check_sel_format(char *pch){
  while(*pch){
    if(*pch == '\n'){
      (*pch) = '\0';
      break;
    }
    pch ++;
  }
}

#define MAX_CHECK_SEL_SIZE 100
static char cs_pre[MAX_CHECK_SEL_SIZE] = "";
static char cs_now[MAX_CHECK_SEL_SIZE];

/*
 *  セレクションが変更されていたらその内容を指すchar*を返す。free()不要。
 *  変更が無ければ,0を返す。
 */
char *check_sel(void){
  if(!get_sel(cs_now, MAX_CHECK_SEL_SIZE))
    return 0;

  check_sel_format(cs_now);

  if(cs_now[0]){
    if(strcmp(cs_pre, cs_now)){
      strcpy(cs_pre, cs_now);
      return cs_pre;
    }
  }
  return 0;
}
