/*
 *  MyD dictionary functions
 *  myd.c
 * Copyright (C) 2000-2001 Hironori FUJII 
 * 
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "myd.h"

struct item{       /* 項目用構造体 */
  char *word;
  char *mean;
};

struct myd_tag{
  char * dic;
  struct item *item_array;  /* 項目の配列 */
  int n_item;               /* 総項目数 */
};

/*
 *  comparision function for qsort()
 */
static int cmp_item(const void *a, const void *b){
  return strcasecmp(((struct item *) a ) -> word,
		    ((struct item *) b ) -> word);
}

/*
 *  return 0 if cannot open.
 */
static MYD myd_open_pdic(char *dic_filename){
  struct stat st;
  int dic_size;
  int fd;
  int i;
  char *p;
  MYD m;
  
  m = (MYD)malloc(sizeof(struct myd_tag));
  if(!m){
    return 0;
  }

  /*  chech file size */
  if(stat(dic_filename, &st) == -1){
    free(m);
    return 0;
  }
  dic_size = st.st_size;

  m->dic = (char*)malloc(dic_size + 1);
  if(!m->dic){
    return 0;
  }

  fd = open(dic_filename, 0);
  if(fd < 0){
    free(m->dic);
    return 0;
  }

  if(read(fd, m->dic, dic_size) != dic_size){
    free(m->dic);
    close(fd);
    return 0;
  }
  close(fd);
  m->dic[dic_size] = '\0';

  /* count  words */
  m->n_item = 0;
  for(i=0;i<dic_size;i++){
    if(m->dic[i] == '\n')
      m->n_item ++;
  }
  m->n_item /= 2;

  m->item_array = (struct item *)calloc(sizeof(struct item), m->n_item);
  if(!m->item_array){
    free(m->dic);
    return 0;
  }

  p = m->dic;
  for(i=0;i<m->n_item;i++){
    m->item_array[i].word = p;
    while(*p != '\r' && *p != '\n')
      p++;
    if(*p == '\r'){
      *p++ = '\0';
      p++;
    }else
      *p++ = '\0';
      
    m->item_array[i].mean = p;
    while(*p != '\r' && *p != '\n')
      p++;
    if(*p == '\r'){
      *p++ = '\0';
      p++;
    }else
      *p++ = '\0';
  }

  qsort(m->item_array, m->n_item, sizeof(struct item), cmp_item);

  return m;
}

/*
 *  return 0 if cannnot open
 */
static MYD myd_open_tsv(char *dic_filename){
  struct stat st;
  int dic_size;
  int fd;
  int i;
  char *p;
  MYD m;
  
  m = (MYD)malloc(sizeof(struct myd_tag));
  if(!m){
    return 0;
  }

  /*  chech file size */
  if(stat(dic_filename, &st) == -1){
    free(m);
    return 0;
  }
  dic_size = st.st_size;

  m->dic = (char*)malloc(dic_size + 1);
  if(!m->dic){
    return 0;
  }

  fd = open(dic_filename, 0);
  if(fd < 0){
    free(m->dic);
    return 0;
  }

  if(read(fd, m->dic, dic_size) != dic_size){
    free(m->dic);
    close(fd);
    return 0;
  }
  close(fd);
  m->dic[dic_size] = '\0';

  /* count  words */
  m->n_item = 0;
  for(i=0;i<dic_size;i++){
    if(m->dic[i] == '\n')
      m->n_item ++;
  }

  m->item_array = (struct item *)calloc(sizeof(struct item), m->n_item);
  if(!m->item_array){
    free(m->dic);
    return 0;
  }

  p = m->dic;
  for(i=0;i<m->n_item;i++){
    m->item_array[i].word = p;
    while(*p != '\t')
      p++;
    *p++ = '\0';
      
    m->item_array[i].mean = p;
    while(*p != '\r' && *p != '\n')
      p++;
    if(*p == '\r'){
      *p++ = '\0';
      p++;
    }else
      *p++ = '\0';
  }

  qsort(m->item_array, m->n_item, sizeof(struct item), cmp_item);

  return m;
}

MYD myd_open(char *filename){
  int c;
  int tsv = 1;
  int i, tab = 0; 
  FILE *fp = fopen(filename, "r");
  if(!fp)
    return 0;

  /*  FILENAME が PDIC形式か,TSV形式かの判別  */
  i = 0;
  while(EOF != (c = fgetc(fp))){
    if(c == '\t')
      tab = 1;
    if(c == '\n'){
      if(tab)
	tab = 0;
      else{
	tsv = 0;
	break;
      }
      i ++;
      if(i >=3)
	break;
    }
  }
  
  fclose(fp);

  if(tsv)
    return myd_open_tsv(filename);
  else
    return myd_open_pdic(filename);
}

void myd_close(MYD m){
  free(m->item_array);
  free(m->dic);
  free((void *) m );
}

/*
 *   辞書の項目数を返す。
 *   return the number of items in MYD
 */
int myd_n_index(MYD myd){
  return myd->n_item;
}

char *myd_key(MYD myd, int index){
  if(index < 0)
    return 0;
  if(index >= myd->n_item)
    return 0;
  return myd->item_array[index].word;
}

char *myd_text(MYD myd, int index){
  if(index < 0)
    return 0;
  if(index >= myd->n_item)
    return 0;
  return myd->item_array[index].mean;
}

/*  strcasecmp()でマッチする先頭からの文字数を返す
 */
static int n_strcasecmp(char *a, char *b){
  int i;
  for(i=0;;i++){
    if(!*a)
      break;
    if(!*b)
      break;
    if(tolower(*a) != tolower(*b))
      break;
    a++;
    b++;
  }
  return i;
}

/*
 *  return the number of items matching to WORD
 */
int myd_bsearch(MYD m, char *word, int *index){
  int f = 0;
  int l = m->n_item - 1;
  int mid;
  int match_len;
  int fl, ll;

  /* binary search */
  while(l - f > 1){
    mid = (f + l) / 2;
    if(strcasecmp(m->item_array[mid].word, word) >= 0)
      l = mid;
    else
      f = mid;
  }

  fl = n_strcasecmp(m->item_array[f].word, word);
  ll = n_strcasecmp(m->item_array[l].word, word);
  
  if(fl >= ll){
    l = f;
    match_len = fl;
  }else{
    f = l;
    match_len = ll;
  }
  if(match_len == 0){
    (*index) = f;
    return 0;
  }

  /* f をスライドし、マッチする先頭を探す */
  while(!strncasecmp(m->item_array[f].word, word, match_len)){
    f --;
    if(f < 0)
      break;
  }
  f ++;
  (*index) = f;

  /* l をスライドし、マッチする最後を探す */
  while(!strncasecmp(m->item_array[l].word, word, match_len)){
    l ++;
    if(l >= m->n_item)
      break;
  }

  return l - f;
}
