/* edit.c
 * Copyright (C) 2000-2001 Hironori FUJII
 */
#include <string.h>
#include "edit.h"

edit_t edit;

void edit_cur_head(){
  edit.cursor = 0;
}

void edit_cur_tail(){
  edit.cursor = strlen(edit.text);
}

void edit_cur_back(){
  if(edit.cursor > 0)
    edit.cursor --;
}

void edit_cur_forward(){
  if(edit.cursor < strlen(edit.text))
    edit.cursor ++;
}

void edit_del_char(){
  memmove(edit.text + edit.cursor,
	  edit.text + edit.cursor + 1,
	  strlen(edit.text + edit.cursor));
}

void edit_back_space(){
  if(edit.cursor == 0)
    return;
  edit_cur_back();
  edit_del_char();
}

void edit_ins_char(int ch){
  if(edit.cursor >= EDIT_MAX_TEXT_LEN - 1)
    return;
  memmove(edit.text + edit.cursor + 1,
	  edit.text + edit.cursor,
	  strlen(edit.text + edit.cursor) + 1);
  edit.text[edit.cursor++] = ch;
  edit.text[EDIT_MAX_TEXT_LEN-1] = '\0';
}

void edit_set_text(char *str){
  strncpy(edit.text, str, EDIT_MAX_TEXT_LEN);
  edit.text[EDIT_MAX_TEXT_LEN-1] = '\0';
  edit.cursor = strlen(edit.text);
}

void edit_kill(){
  if(edit.text[edit.cursor] == '\0')
    return;
  edit.text[edit.cursor] = '\0';
}

void edit_clear(){
  edit_cur_head();
  edit_kill();
}

void edit_transpose(){
  char t, len;
  len = strlen(edit.text);

  if(len < 2)
    return;

  if(edit.cursor == 0)
    edit.cursor ++;

  if(len != edit.cursor)
    edit.cursor ++;

  t = edit.text[edit.cursor - 2];
  edit.text[edit.cursor - 2] = edit.text[edit.cursor - 1];
  edit.text[edit.cursor - 1] = t;
}
