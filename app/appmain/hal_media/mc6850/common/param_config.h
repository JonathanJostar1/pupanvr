#ifndef __HYSA_FILE_COMM_H__
#define __HYSA_FILE_COMM_H__

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#define  MAX_VALUE  64 /* max length of section,key,value*/
typedef struct _option {
  char    key[MAX_VALUE];   /* 对应键 */
  char    value[MAX_VALUE]; /* 对应值 */
  struct  _option *next;    /* 链表连接标识 */
}Option;

typedef struct _data {
  char    section[MAX_VALUE]; /* 保存section值          */
  Option  *option;            /* option链表头           */
  struct  _data *next;        /* 链表连接标识           */
}Data;

typedef struct {
  char    comment;              /* 表示注释的符号    */
  char    separator;            /* 表示分隔符        */
  char    re_string[MAX_VALUE]; /* 返回值字符串的值  */
  int     re_int;               /* 返回int的值       */
  bool    re_bool;              /* 返回bool的值      */
  double  re_double ;           /* 返回double类型    */
  Data    *data;                /* 保存数据的头      */
  unsigned int num;
}Config;

Config *cnf_load_config(const char *filename);
bool cnf_get_value(Config *cnf, const char *section, const char *key);
void print_config_info(Config *cnf);
int cnf_release(Config *cnf);
bool cnf_remove_section(Config *cnf, const char *section);
bool cnf_add_option(Config *cnf, const char *section, const char *key, const char *value);
Data *cnf_has_section(Config *cnf, const char *section);
unsigned int cnf_get_total_num(Config *cnf);


#endif
