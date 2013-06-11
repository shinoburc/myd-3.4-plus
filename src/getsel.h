/*   
 *  getsel.h  --- Get string from X11 selection
 *    Copyright (C) 2000-2001 Hironori FUJII
 */

/* get_sel() のラッパー関数
 * セレクションが変更されたらその文字列を返す。
 * 前回チェックして以降,変更がないと0を返す
 */
char *check_sel(void);

/*  return 1 if success, but return 0 if not.
 */
int  get_sel_init(char *display_name);

void get_sel_end(void);

/*  セレクションを取得する。
 *  成功すれば1,失敗すれば0を返す。
 */
int  get_sel(char* buff, int size);
