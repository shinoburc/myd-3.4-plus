/*   
 *  getsel.h  --- Get string from X11 selection
 *    Copyright (C) 2000-2001 Hironori FUJII
 */

/* get_sel() �Υ�åѡ��ؿ�
 * ���쥯������ѹ����줿�餽��ʸ������֤���
 * ��������å����ưʹ�,�ѹ����ʤ���0���֤�
 */
char *check_sel(void);

/*  return 1 if success, but return 0 if not.
 */
int  get_sel_init(char *display_name);

void get_sel_end(void);

/*  ���쥯������������롣
 *  ���������1,���Ԥ����0���֤���
 */
int  get_sel(char* buff, int size);
