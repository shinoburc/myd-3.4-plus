/* edit.h
 * Copyright (C) 2000-2001 Hironori FUJII
 */

#define EDIT_MAX_TEXT_LEN 256

typedef struct {
  char text[EDIT_MAX_TEXT_LEN];
  int cursor;      /* 文字列のカーソル位置 */
} edit_t;

extern edit_t edit;

void edit_cur_head(void);
void edit_cur_tail(void);
void edit_cur_back(void);
void edit_cur_forward(void);
void edit_back_space(void);
void edit_del_char(void);
void edit_ins_char(int ch);
void edit_set_text(char *str);
void edit_kill(void);
void edit_clear(void);
void edit_transpose(void);

