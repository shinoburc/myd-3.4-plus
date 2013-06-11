/*  myd.h Copyright (C) 2000-2001 Hironori FUJII
 */

typedef struct myd_tag * MYD;

/* return 0 if cannot open
 * オープンに失敗すると0を返します
 */
MYD myd_open(char *filename);

void myd_close(MYD m);


/* binary search 
 * index に検索にマッチする最初の項目を返します
 * 返り値はマッチする項目の数
 */
int myd_bsearch(MYD m, char *word, int *index);

/* index is from 0 to myd_n_keys() - 1. 0 <= index < myd_n_keys() .
 * index番目の項目のキーを返します。
 */
char* myd_key(MYD m, int index);

char* myd_text(MYD m, int index);


/* return the number of items in m.
 * 項目数を返します
 */
int myd_n_index(MYD m);
