#include <stdio.h>
#include <errno.h>

#include "param_config.h"


#ifdef DEBG_PARAM
#define  PRINT_ERRMSG(STR) fprintf(stderr,"line:%d,msg:%s,eMsg:%s\n", __LINE__, STR, strerror(errno))
#else
#define PRINT_ERRMSG(STR) do{}while(0)
#endif
/**
* Check whether string is empty 
**/
bool str_empty(const char *string)
{
    return NULL == string || 0 == strlen(string);
}

/**
* add information to Linklist
**/
bool cnf_add_option(Config *cnf, const char *section, const char *key, const char *value)
{
    if (NULL == cnf || str_empty(section) || str_empty(key) || str_empty(value)) {
        return false; 
    }

    Data *p,*q;
    p = q = cnf->data;  //check if the seciton exist
    while (NULL != p && 0 != strcmp(p->section, section)) {
        q = p;
        p = p->next;
    }

    if (NULL == p) { /*Non-existent ,malloc the data Linklist*/
        p = (Data*)malloc(sizeof(Data));
        if (NULL == p) {
            return false;
        }
        strcpy(p->section, section);
        p->option = NULL;    // the link of option must be NULL
        p->next = NULL; 
        if(q==NULL)
            cnf->data = p;
        else
            q ->next = p; 
    }

    Option *m,*n;
    m = n = p->option;
    while (NULL != m && 0 != strcmp(m->key, key)) {
        n=m;
        m= m->next; /* check if the key exist*/
    }

    if (NULL == m) { /*Non-existent ,malloc the option Linklist*/
        m = (Option*)malloc(sizeof(Option));
        if (NULL == m) {
            return false;
        }
        strcpy(m->key, key);
        m->next = NULL; 
        if(n == NULL)
            p->option = m;
        else
            n->next = m;
    }
    strcpy(m->value, value); // copy the value of option

    return true;
}

/**
* Remove all the blanks and annotations in the string
**/
bool strip_comments(char *string, char comment)
{
    if (NULL == string || '\n' == *string || '\r' == *string) { // enter ,newline : empty line
        return false; 
    }

    char *p, *q; //Remove all the blanks and annotations
    for (p = q = string; *p != '\0' && *p != comment; p++) {
        if (0 == isspace(*p)) {
            *q++ = *p;
        }
    }
    *q = '\0';

    return 0 != strlen(string); /* valid data length is not 0,return true*/
}

/**
*  paser config file.
**/
Config *cnf_load_config(const char *filename)
{
    Config *cnf     = (Config*)malloc(sizeof(Config));
    cnf->comment    = '#';  //The character and following characters of each line will be discarded 
    cnf->separator  = '=';  // Used to separate Section and data
    cnf->data       = NULL;
    cnf->num        = 0;

    if (str_empty(filename)) {
        return NULL;
    }

    char *p, sLine[MAX_VALUE*2];    /* sLine: one line data */
    char section[MAX_VALUE], key[MAX_VALUE], value[MAX_VALUE];
    int off=0;
    FILE *fp = fopen(filename, "r");
    if(NULL == fp) {
        PRINT_ERRMSG("fopen");
        free(cnf);
        return NULL;
    }
  
    while (NULL != fgets(sLine, MAX_VALUE, fp)) {
        if (strip_comments(sLine, cnf->comment)) { 
            if ('[' == sLine[0] && ']' == sLine[strlen(sLine)-1]) {
                memset(section, '\0', MAX_VALUE);
                strncpy(section, sLine+1, strlen(sLine)-2);
                off = strlen(sLine)-2;
                cnf->num++;
            }else if ('{' == sLine[0] && '}' == sLine[strlen(sLine)-1]) {
                strncpy(section+off, "_", 1);
				if((strlen(sLine)-5 == 1)||(strlen(sLine)-2 == 1))
	                strncpy(section+off+1, sLine+strlen(sLine)-2, 1);
				else if(strlen(sLine)-5 == 2)
					strncpy(section+off+1, sLine+strlen(sLine)-3, 2);
            } 
            else if (NULL != (p = strchr(sLine, cnf->separator))) {  
                memset(key,   '\0', MAX_VALUE); 
                strncpy(key,  sLine, p - sLine);
                strcpy(value, p + 1); 
                cnf_add_option(cnf, section, key, value); /* add information to linklist */
            }
        }
    } 
  
  fclose(fp);
  return cnf;
}
unsigned int cnf_get_total_num(Config *cnf)
{
    if (NULL == cnf) {
        PRINT_ERRMSG("cnf is NULL");
        return false;
    }

    return cnf->num;
}

/**
* get value of the key under section.
**/
bool cnf_get_value(Config *cnf, const char *section, const char *key)
{
    Data *p = cnf->data; /* check if have the section*/
    while (NULL != p && 0 != strcmp(p->section, section)) {
        p = p->next;
    }

    if (NULL == p) {
        PRINT_ERRMSG("section not find!");
        return false;
    }

    Option *q = p->option;     /* check if have the key*/
    while (NULL != q && 0 != strcmp(q->key, key)) {
        q = q->next;
    }

    if (NULL == q) {
        PRINT_ERRMSG("key not find!");
        return false;
    }

    strcpy(cnf->re_string, q->value);       /* get string*/
    cnf->re_int    = atoi(cnf->re_string);  /* get int*/
    cnf->re_bool   = 0 == strcmp ("true", cnf->re_string); /* get bool*/
    cnf->re_double = atof(cnf->re_string);  /*get double*/

    return true;
}

/**
* check if the seciton exist
**/
Data *cnf_has_section(Config *cnf, const char *section)
{
    Data *p = cnf->data; 
    while (NULL != p && 0 != strcmp(p->section, section)) {
        p = p->next;
    }

    if (NULL == p) { 
        return NULL;
    }

    return p;
}

/**
*  check if the option under section exist
**/
Option *cnf_has_option(Config *cnf, const char *section, const char *key)
{
  Data *p = cnf_has_section(cnf, section);
  if (NULL == p) {
    return NULL;
  }
  
  Option *q = p->option;
  while (NULL != q && 0 != strcmp(q->key, key)) {
    q = q->next;
  }
  if (NULL == q) {
    return NULL;
  }
  
  return q;
}

/**
* print config information
**/
void print_config_info(Config *cnf)
{
    Data *p = cnf->data;
    while (NULL != p) {
        printf("[%s]\n",p->section);
        Option *q = p->option;
        while (NULL != q) {
            printf("  %s %c %s\n", q->key, cnf->separator, q->value);
            q = q->next;
        }
        p = p->next;
    }
}

/**
* write config information to file
**/
bool cnf_write_file(Config *cnf, const char *filename, const char *header)
{
    FILE *fp = fopen(filename, "w");
    if(NULL == fp) {
        PRINT_ERRMSG("fopen");
        return false;
    }

    if (0 < strlen(header)) {
        fprintf(fp, "%c %s\n\n", cnf->comment, header);
    }

    Option *q;
    Data   *p = cnf->data;
    while (NULL != p) {
        fprintf(fp, "[%s]\n", p->section);
        q = p->option;
        while (NULL != q) {
            fprintf(fp, "\t %s %c %s\n", q->key, cnf->separator, q->value);
            q = q->next;
        }
        p = p->next;
    }
    fclose(fp);
    return true;
}

/**
* delete option
**/
bool cnf_remove_option(Config *cnf, const char *section, const char *key)
{
    Data *ps = cnf_has_section(cnf, section);
    if (NULL == ps) {
        return NULL;
    }

    Option *p, *q;
    q = p = ps->option;
    while (NULL != p && 0 != strcmp(p->key, key)) {
        if (p != q) { 
            q = q->next; 
        } 
        p = p->next;
    }

    if (NULL == p) { 
        return NULL;
    }

    if (p == q) { /* head node */
        ps->option = p->next;
    } else {
        q->next = p->next;
    }

    free(p);
    q = p = NULL;

    return true;
}

/**
*delete section
**/
bool cnf_remove_section(Config *cnf, const char *section)
{
    if (str_empty(section)) {
        return false;
    }

    Data *p, *q;
    q = p = cnf->data; 
    while (NULL != p && 0 != strcmp(p->section, section)) {
        if (p != q) { q = q->next; }
        p = p->next;
    }

    if (NULL == p) { 
        return false;
    }

    if (p == q) { /* head node*/
        cnf->data = p->next;
    } else { 
        q->next = p->next;
    }

    Option *m,*n;
    m = n = p->option;
    while (NULL != m) {
        n = m;
        m = m->next;
        free(n);
    }
    p->option = NULL;
    free(p); 
    p = NULL; 

    return true;
}

/**
* release config section & option
**/
int cnf_release(Config *cnf)  
{  
    int ret = 0;  
    if(cnf->data== NULL || cnf == NULL)  
    {  
        ret = -1;  
        printf("release error:%d from (handler == NULL)\n",ret);  
        return ret;  
    }  
    Data *p, *q;
    Option *m,*n;
    q = p = cnf->data;
    m = n = p->option;  
    while(NULL != p){
        m = n = p->option;   //free option
        while (NULL != n) {
            m = n;
            n = n->next;
            free(m);
        }
        q=p;                        //free data
        p=p->next;
        free(q);
    }
    p=q=NULL;
    m=n=NULL;
    
    return ret;  
}  

