/*  myd.h Copyright (C) 2000-2001 Hironori FUJII
 */

typedef struct myd_tag * MYD;

/* return 0 if cannot open
 * �����ץ�˼��Ԥ����0���֤��ޤ�
 */
MYD myd_open(char *filename);

void myd_close(MYD m);


/* binary search 
 * index �˸����˥ޥå�����ǽ�ι��ܤ��֤��ޤ�
 * �֤��ͤϥޥå�������ܤο�
 */
int myd_bsearch(MYD m, char *word, int *index);

/* index is from 0 to myd_n_keys() - 1. 0 <= index < myd_n_keys() .
 * index���ܤι��ܤΥ������֤��ޤ���
 */
char* myd_key(MYD m, int index);

char* myd_text(MYD m, int index);


/* return the number of items in m.
 * ���ܿ����֤��ޤ�
 */
int myd_n_index(MYD m);
