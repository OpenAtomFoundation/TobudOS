/*
    Copyright (c) 2000-2011 by Nicolas Devillard.
    MIT License

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/
/*-------------------------------------------------------------------------*/
/**
   @file    iniparser.c
   @author  N. Devillard
   @brief   Parser for ini files.
*/
/*--------------------------------------------------------------------------*/
/*---------------------------- Includes ------------------------------------*/
#include <ctype.h>
#include "iniparser.h"

/*---------------------------- Defines -------------------------------------*/
#define ASCIILINESZ         (100)
#define INI_INVALID_KEY     ((char*)-1)

/*---------------------------------------------------------------------------
                        Private to this module
 ---------------------------------------------------------------------------*/
/**
 * This enum stores the status for each parsed line (internal use only).
 */
typedef enum _line_status_
{
    LINE_UNPROCESSED,
    LINE_ERROR,
    LINE_EMPTY,
    LINE_COMMENT,
    LINE_SECTION,
    LINE_VALUE
} line_status ;

/*-------------------------------------------------------------------------*/
/**
  @brief    Convert a string to lowercase.
  @param    s   String to convert.

  This function modifies the string passed, the modified string
  contains a lowercased version of the input string.
 */
/*--------------------------------------------------------------------------*/

static void strlwc(char * s)
{
    int i ;

    if (s == NULL) return;

    i = 0 ;

    while (s[i])
    {
        s[i] = (char)tolower((int)s[i]);
        i++ ;
    }
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Remove blanks at the beginning and the end of a string.
  @param    s   String to parse.

  This function modifies the input string and returns a modified string
  which is identical to the input string, except that all blank
  characters at the end and the beg. of the string have been removed.
 */
/*--------------------------------------------------------------------------*/
static void strstrip(char * s)
{
    if (s == NULL) return ;

    char *last = s + strlen(s);
    char *dest = s;

    while (isspace((int)*s) && *s) s++;

    while (last > s)
    {
        if (!isspace((int) * (last - 1)))
            break ;

        last -- ;
    }

    *last = (char)0;

    memmove(dest, s, last - s + 1);
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Get number of sections in a dictionary
  @param    d   Dictionary to examine
  @return   int Number of sections found in dictionary

  This function returns the number of sections found in a dictionary.
  The test to recognize sections is done on the string stored in the
  dictionary: a section name is given as "section" whereas a key is
  stored as "section:key", thus the test looks for entries that do not
  contain a colon.

  This clearly fails in the case a section name contains a colon, but
  this should simply be avoided.

  This function returns -1 in case of error.
 */
/*--------------------------------------------------------------------------*/
int iniparser_getnsec(dictionary * d)
{
    int i ;
    int nsec ;

    if (d == NULL) return -1 ;

    nsec = 0 ;

    for (i = 0 ; i < d->size ; i++)
    {
        if (d->key[i] == NULL)
            continue ;

        if (strchr(d->key[i], ':') == NULL)
        {
            nsec ++ ;
        }
    }

    return nsec ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Get name for section n in a dictionary.
  @param    d   Dictionary to examine
  @param    n   Section number (from 0 to nsec-1).
  @return   Pointer to char string

  This function locates the n-th section in a dictionary and returns
  its name as a pointer to a string statically allocated inside the
  dictionary. Do not free or modify the returned string!

  This function returns NULL in case of error.
 */
/*--------------------------------------------------------------------------*/
char * iniparser_getsecname(dictionary * d, int n)
{
    int i ;
    int foundsec ;

    if (d == NULL || n < 0) return NULL ;

    foundsec = 0 ;

    for (i = 0 ; i < d->size ; i++)
    {
        if (d->key[i] == NULL)
            continue ;

        if (strchr(d->key[i], ':') == NULL)
        {
            foundsec++ ;

            if (foundsec > n)
                break ;
        }
    }

    if (foundsec <= n)
    {
        return NULL ;
    }

    return d->key[i] ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Dump a dictionary to an opened file pointer.
  @param    d   Dictionary to dump.
  @param    f   Opened file pointer to dump to.
  @return   void

  This function prints out the contents of a dictionary, one element by
  line, onto the provided file pointer. It is OK to specify @c stderr
  or @c stdout as output files. This function is meant for debugging
  purposes mostly.
 */
/*--------------------------------------------------------------------------*/
#ifdef SUPPORT_FATFS
void iniparser_dump(dictionary * d, FIL *f)
#else
void iniparser_dump(dictionary * d, FILE * f)
#endif
{
    int     i ;

    if (d == NULL || f == NULL) return ;

    for (i = 0 ; i < d->size ; i++)
    {
        if (d->key[i] == NULL)
            continue ;

        if (d->val[i] != NULL)
        {
            #ifdef SUPPORT_FATFS
            f_printf(f, "[%s]=[%s]\n", d->key[i], d->val[i]);
            #else
            fprintf(f, "[%s]=[%s]\n", d->key[i], d->val[i]);
            #endif
        }
        else
        {
            #ifdef SUPPORT_FATFS
            f_printf(f, "[%s]=UNDEF\n", d->key[i]);
            #else
            fprintf(f, "[%s]=UNDEF\n", d->key[i]);
            #endif
        }
    }

    return ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Save a dictionary to a loadable ini file
  @param    d   Dictionary to dump
  @param    f   Opened file pointer to dump to
  @return   void

  This function dumps a given dictionary into a loadable ini file.
  It is Ok to specify @c stderr or @c stdout as output files.
 */
/*--------------------------------------------------------------------------*/
#ifdef SUPPORT_FATFS
void iniparser_dump_ini(dictionary * d, FIL *f)
#else
void iniparser_dump_ini(dictionary * d, FILE *f)
#endif
{
    int     i ;
    int     nsec ;
    char *  secname ;

    if (d == NULL || f == NULL) return ;

    nsec = iniparser_getnsec(d);

    if (nsec < 1)
    {
        /* No section in file: dump all keys as they are */
        for (i = 0 ; i < d->size ; i++)
        {
            if (d->key[i] == NULL)
                continue ;
            
            #ifdef SUPPORT_FATFS
            f_printf(f, "%s = %s\n", d->key[i], d->val[i]);
            #else
            fprintf(f, "%s = %s\n", d->key[i], d->val[i]);
            #endif
        }

        return ;
    }

    for (i = 0 ; i < nsec ; i++)
    {
        secname = iniparser_getsecname(d, i) ;
        iniparser_dumpsection_ini(d, secname, f) ;
    }
    
    #ifdef SUPPORT_FATFS
    f_printf(f, "\n");
    #else
    fprintf(f, "\n");
    #endif
    return ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Save a dictionary section to a loadable ini file
  @param    d   Dictionary to dump
  @param    s   Section name of dictionary to dump
  @param    f   Opened file pointer to dump to
  @return   void

  This function dumps a given section of a given dictionary into a loadable ini
  file.  It is Ok to specify @c stderr or @c stdout as output files.
 */
/*--------------------------------------------------------------------------*/
#ifdef SUPPORT_FATFS
void iniparser_dumpsection_ini(dictionary * d, char * s, FIL *f)
#else
void iniparser_dumpsection_ini(dictionary * d, char * s, FILE * f)
#endif
{
    int     j ;
    char    *keym;
    int     secsize ;
    
    if (d == NULL || f == NULL) return ;
    
    if (! iniparser_find_entry(d, s)) return ;
    
    #ifdef SUPPORT_FATFS
    f_printf(f, "\n[%s]\n", s);
    #else
    fprintf(f, "\n[%s]\n", s);
    #endif
    secsize = (int)strlen(s) + 2;
    keym = (char *)malloc(secsize);
    snprintf(keym, secsize, "%s:", s);

    for (j = 0 ; j < d->size ; j++)
    {
        if (d->key[j] == NULL)
            continue ;

        if (!strncmp(d->key[j], keym, secsize - 1))
        {
            #ifdef SUPPORT_FATFS
            f_printf(f,
                     "%-30s = %s\n",
                     d->key[j] + secsize - 1,
                     d->val[j] ? d->val[j] : "");
            #else
            fprintf(f,
                    "%-30s = %s\n",
                    d->key[j]+secsize-1,
                    d->val[j] ? d->val[j] : "");
            #endif
        }
    }
    
    #ifdef SUPPORT_FATFS
    f_printf(f, "\n");
    #else
    fprintf(f, "\n");
    #endif
    
    free(keym);
    return ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Get the number of keys in a section of a dictionary.
  @param    d   Dictionary to examine
  @param    s   Section name of dictionary to examine
  @return   Number of keys in section
 */
/*--------------------------------------------------------------------------*/
int iniparser_getsecnkeys(dictionary * d, char * s)
{
    int     secsize, nkeys ;
    char    *keym;
    int j ;

    nkeys = 0;

    if (d == NULL) return nkeys;

    if (! iniparser_find_entry(d, s)) return nkeys;

    secsize  = (int)strlen(s) + 2;
    keym = (char *)malloc(secsize);
    snprintf(keym, secsize, "%s:", s);

    for (j = 0 ; j < d->size ; j++)
    {
        if (d->key[j] == NULL)
            continue ;

        if (!strncmp(d->key[j], keym, secsize - 1))
            nkeys++;
    }

    free(keym);
    return nkeys;

}

/*-------------------------------------------------------------------------*/
/**
  @brief    Get the number of keys in a section of a dictionary.
  @param    d   Dictionary to examine
  @param    s   Section name of dictionary to examine
  @return   pointer to statically allocated character strings

  This function queries a dictionary and finds all keys in a given section.
  Each pointer in the returned char pointer-to-pointer is pointing to
  a string allocated in the dictionary; do not free or modify them.

  This function returns NULL in case of error.
 */
/*--------------------------------------------------------------------------*/
char ** iniparser_getseckeys(dictionary * d, char * s)
{

    char **keys;

    int i, j ;
    char    *keym;
    int     secsize, nkeys ;

    keys = NULL;

    if (d == NULL) return keys;

    if (! iniparser_find_entry(d, s)) return keys;

    nkeys = iniparser_getsecnkeys(d, s);

    keys = (char**) malloc(nkeys * sizeof(char*));

    secsize  = (int)strlen(s) + 2;
    keym = (char *)malloc(secsize);
    snprintf(keym, secsize, "%s:", s);

    i = 0;

    for (j = 0 ; j < d->size ; j++)
    {
        if (d->key[j] == NULL)
            continue ;

        if (!strncmp(d->key[j], keym, secsize - 1))
        {
            keys[i] = d->key[j];
            i++;
        }
    }

    free(keym);
    return keys;

}

/*-------------------------------------------------------------------------*/
/**
  @brief    Get the string associated to a key
  @param    d       Dictionary to search
  @param    key     Key string to look for
  @param    def     Default value to return if key not found.
  @return   pointer to statically allocated character string

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the pointer passed as 'def' is returned.
  The returned char pointer is pointing to a string allocated in
  the dictionary, do not free or modify it.
 */
/*--------------------------------------------------------------------------*/
char * iniparser_getstring(dictionary * d, const char * key, char * def)
{
    char * lc_key ;
    char * sval ;

    if (d == NULL || key == NULL)
        return def ;

    lc_key = xstrdup(key);
    strlwc(lc_key);
    sval = dictionary_get(d, lc_key, def);
    free(lc_key);
    return sval ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Get the string associated to a key, convert to an int
  @param    d Dictionary to search
  @param    key Key string to look for
  @param    notfound Value to return in case of error
  @return   integer

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the notfound value is returned.

  Supported values for integers include the usual C notation
  so decimal, octal (starting with 0) and hexadecimal (starting with 0x)
  are supported. Examples:

  "42"      ->  42
  "042"     ->  34 (octal -> decimal)
  "0x42"    ->  66 (hexa  -> decimal)

  Warning: the conversion may overflow in various ways. Conversion is
  totally outsourced to strtol(), see the associated man page for overflow
  handling.

  Credits: Thanks to A. Becker for suggesting strtol()
 */
/*--------------------------------------------------------------------------*/
int iniparser_getint(dictionary * d, const char * key, int notfound)
{
    char    *   str ;

    str = iniparser_getstring(d, key, INI_INVALID_KEY);

    if (str == INI_INVALID_KEY) return notfound ;

    return (int)strtol(str, NULL, 0);
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Get the string associated to a key, convert to a double
  @param    d Dictionary to search
  @param    key Key string to look for
  @param    notfound Value to return in case of error
  @return   double

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the notfound value is returned.
 */
/*--------------------------------------------------------------------------*/
double iniparser_getdouble(dictionary * d, const char * key, double notfound)
{
    char    *   str ;

    str = iniparser_getstring(d, key, INI_INVALID_KEY);

    if (str == INI_INVALID_KEY) return notfound ;

    return atof(str);
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Get the string associated to a key, convert to a boolean
  @param    d Dictionary to search
  @param    key Key string to look for
  @param    notfound Value to return in case of error
  @return   integer

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the notfound value is returned.

  A true boolean is found if one of the following is matched:

  - A string starting with 'y'
  - A string starting with 'Y'
  - A string starting with 't'
  - A string starting with 'T'
  - A string starting with '1'

  A false boolean is found if one of the following is matched:

  - A string starting with 'n'
  - A string starting with 'N'
  - A string starting with 'f'
  - A string starting with 'F'
  - A string starting with '0'

  The notfound value returned if no boolean is identified, does not
  necessarily have to be 0 or 1.
 */
/*--------------------------------------------------------------------------*/
int iniparser_getboolean(dictionary * d, const char * key, int notfound)
{
    char    *   c ;
    int         ret ;

    c = iniparser_getstring(d, key, INI_INVALID_KEY);

    if (c == INI_INVALID_KEY) return notfound ;

    if (c[0] == 'y' || c[0] == 'Y' || c[0] == '1' || c[0] == 't' || c[0] == 'T')
    {
        ret = 1 ;
    }
    else if (c[0] == 'n' || c[0] == 'N' || c[0] == '0' || c[0] == 'f' || c[0] == 'F')
    {
        ret = 0 ;
    }
    else
    {
        ret = notfound ;
    }

    return ret;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Finds out if a given entry exists in a dictionary
  @param    ini     Dictionary to search
  @param    entry   Name of the entry to look for
  @return   integer 1 if entry exists, 0 otherwise

  Finds out if a given entry exists in the dictionary. Since sections
  are stored as keys with NULL associated values, this is the only way
  of querying for the presence of sections in a dictionary.
 */
/*--------------------------------------------------------------------------*/
int iniparser_find_entry(
    dictionary  *   ini,
    const char  *   entry
)
{
    int found = 0 ;

    if (iniparser_getstring(ini, entry, INI_INVALID_KEY) != INI_INVALID_KEY)
    {
        found = 1 ;
    }

    return found ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Set an entry in a dictionary.
  @param    ini     Dictionary to modify.
  @param    entry   Entry to modify (entry name)
  @param    val     New value to associate to the entry.
  @return   int 0 if Ok, -1 otherwise.

  If the given entry can be found in the dictionary, it is modified to
  contain the provided value. If it cannot be found, -1 is returned.
  It is Ok to set val to NULL.
 */
/*--------------------------------------------------------------------------*/
int iniparser_set(dictionary * ini, const char * entry, const char * val)
{
    int result = 0;
    char *lc_entry = xstrdup(entry);
    strlwc(lc_entry);
    result = dictionary_set(ini, lc_entry, val) ;
    free(lc_entry);
    return result;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Delete an entry in a dictionary
  @param    ini     Dictionary to modify
  @param    entry   Entry to delete (entry name)
  @return   void

  If the given entry can be found, it is deleted from the dictionary.
 */
/*--------------------------------------------------------------------------*/
void iniparser_unset(dictionary * ini, const char * entry)
{
    char* lc_entry = xstrdup(entry);
    strlwc(lc_entry);
    dictionary_unset(ini, lc_entry);
    free(lc_entry);
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Load a single line from an INI file
  @param    input_line  Input line, may be concatenated multi-line input
  @param    section     Output space to store section
  @param    key         Output space to store key
  @param    value       Output space to store value
  @return   line_status value
 */
/*--------------------------------------------------------------------------*/
static line_status iniparser_line(
    int line_size,
    const char * input_line,
    char ** section_out,
    char ** key_out,
    char ** value_out)
{
    line_status sta ;
    int len = line_size - 1;
    char * line = (char *)malloc(line_size);
    char * key = NULL;
    char * value = NULL;
    char * equals = NULL;

    if (!line)
    {
        fprintf(stderr, "iniparser: memory alloc error\n");
        return LINE_ERROR;
    }

    *line = 0;


    strcpy(line, input_line);
    strstrip(line);
    len = (int)strlen(line);

    /* only allocate necessary space for key & val */
    equals = strchr(line, '=');

    if (equals)
    {
        value = (char *)malloc((len + line) - equals + 1);
        key = (char *)malloc(equals - line + 1);
        *value = 0;
    }
    else
    {
        key = (char *)malloc(line_size + 1);
    }

    if (!key || (equals && !value))
    {
        fprintf(stderr, "iniparser: memory alloc error\n");
        sta = LINE_ERROR;
        goto out;
    }

    *key = 0;

    sta = LINE_UNPROCESSED ;

    if (len < 1)
    {
        /* Empty line */
        sta = LINE_EMPTY ;
    }
    else if (line[0] == '#' || line[0] == ';')
    {
        /* Comment line */
        sta = LINE_COMMENT ;
    }
    else if (line[0] == '[' && line[len - 1] == ']')
    {
        /* Section name */
        sscanf(line, "[%[^]]", key);
        strstrip(key);
        strlwc(key);
        sta = LINE_SECTION ;
        *section_out = key;
        /* don't free key's memory */
        key = NULL;
    }
    else if (equals && (sscanf (line, "%[^=] = \"%[^\"]\"", key, value) == 2
                        ||  sscanf (line, "%[^=] = '%[^\']'",   key, value) == 2
                        ||  sscanf (line, "%[^=] = %[^;#]",     key, value) == 2))
    {
        /* Usual key=value, with or without comments */
        strstrip(key);
        strlwc(key);
        strstrip(value);

        /*
         * sscanf cannot handle '' or "" as empty values
         * this is done here
         */
        if (!strcmp(value, "\"\"") || (!strcmp(value, "''")))
        {
            value[0] = 0 ;
        }

        *key_out = key;
        *value_out = value;
        key = NULL;
        value = NULL;
        sta = LINE_VALUE ;
    }
    else if (equals && (sscanf(line, "%[^=] = %[;#]", key, value) == 2
                        ||  sscanf(line, "%[^=] %[=]", key, value) == 2))
    {
        /*
         * Special cases:
         * key=
         * key=;
         * key=#
         */
        strstrip(key);
        strlwc(key);
        value[0] = 0 ;
        *key_out = key;
        *value_out = value;

        /* don't free out params key or val's memory */
        key = NULL;
        value = NULL;
        sta = LINE_VALUE ;
    }
    else
    {
        /* Generate syntax error */
        sta = LINE_ERROR ;
    }

out:

    if (line)
    {
        free(line);
        line = NULL;
    }

    if (key)
    {
        free(key);
        key = NULL;
    }

    if (value)
    {
        free(value);
        value = NULL;
    }

    return sta ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Parse an ini file and return an allocated dictionary object
  @param    ininame Name of the ini file to read.
  @return   Pointer to newly allocated dictionary

  This is the parser for ini files. This function is called, providing
  the name of the file to be read. It returns a dictionary object that
  should not be accessed directly, but through accessor functions
  instead.

  The returned dictionary must be freed using iniparser_freedict().
 */
/*--------------------------------------------------------------------------*/
dictionary * iniparser_load(const char * ininame)
{
    char line    [ASCIILINESZ + 1] ;
    char *section = xstrdup("");
    char *current_section = NULL;
    char *key = NULL;
    char *val = NULL;
    char* full_line = NULL;
    char* prev_line = NULL;

    int  len ;
    int  lineno = 0 ;
    int  errs = 0;
    int  seckey_size = 0;
    
    #ifndef SUPPORT_FATFS
    FILE * in = NULL ;
    #endif
    
    dictionary * dict = NULL ;

    #ifdef SUPPORT_FATFS
    retUSER_SYS_CONFIG = f_open(&USER_SYS_CONFIG_File, ininame, FA_OPEN_EXISTING | FA_READ);
    if(FR_OK != retUSER_SYS_CONFIG)
    {
        fprintf(stderr, "iniparser: cannot open %s\n", ininame);
        goto out ;
    }
    #else
    
    if ((in=fopen(ininame, "r"))==NULL) {
        fprintf(stderr, "iniparser: cannot open %s\n", ininame);
        goto out;
    }
    #endif
    
    dict = dictionary_new(0) ;

    if (!dict)
    {
        goto out;
    }

    memset(line,    0, ASCIILINESZ);
    
    #ifdef SUPPORT_FATFS
    while(f_gets(line, 100, &USER_SYS_CONFIG_File) != NULL)
    #else
    while (fgets(line, ASCIILINESZ, in)!=NULL)
    #endif
    {
        int prev_line_len = 0;
        int multi_line = 0;
        int total_size = 0;

        if (key)
        {
            free(key);
            key = NULL;
        }

        if (val)
        {
            free(val);
            val = NULL;
        }

        lineno++ ;
        len = (int)strlen(line) - 1;

        if (len == 0)
            continue;

        /* Safety check against buffer overflows */
        #ifdef SUPPORT_FATFS
        if (line[len] != '\n' && !f_eof(&USER_SYS_CONFIG_File))
        #else
        if (line[len]!='\n' && !feof(in))
        #endif
        {

            fprintf(stderr,
                    "iniparser: input line too long in %s (%d)\n",
                    ininame,
                    lineno);
            errs++;
            goto out;
        }

        /* Get rid of \n and spaces at end of line */
        while ((len >= 0) &&
                ((line[len] == '\n') || (isspace(line[len]))))
        {
            line[len] = 0 ;
            len-- ;
        }

        if (len < 0)   /* Line was entirely \n and/or spaces */
        {
            len = 0;
        }

        /* Detect multi-line */
        if (line[len] == '\\')
        {
            multi_line = 1;
        }

        if (multi_line)
        {
            /* Multi-line value */
            /* length without trailing '\' */
            /* remove multi-line indicator before appending*/
            line[len] = 0;
            len--;
        }

        /*
         * If processing a multi-line then append it the previous portion,
         * at this point 'full_line' has the previously read portion of a
         * multi-line line (or NULL)
         */
        prev_line = full_line;
        prev_line_len = 0;

        if (prev_line)
        {
            prev_line_len = strlen(prev_line);
        }

        /* len is not strlen(line) but strlen(line) -1 */
        total_size = (len + 1) + prev_line_len + 1;

        full_line = (char *)malloc(total_size);

        if (!full_line)
        {
            fprintf(stderr,
                    "iniparser: out of mem\n");
            errs++;
            goto out;
        }

        memset(full_line, 0, total_size);

        if (prev_line)
        {
            strcpy(full_line, prev_line);
        }

        strcpy(full_line + prev_line_len, line);
        free(prev_line);
        prev_line = NULL;

        if (multi_line)
        {
            continue ;
        }

        switch (iniparser_line(total_size, full_line, &current_section, &key, &val))
        {
            case LINE_EMPTY:
            case LINE_COMMENT:
                break ;

            case LINE_SECTION:
                if (section)
                {
                    free(section);
                    section = NULL;
                }

                errs = dictionary_set(dict, current_section, NULL);
                section = current_section;
                break ;

            case LINE_VALUE:
            {
                char *seckey;
                /* section + ':' + key + eos */
                seckey_size = strlen(section) + strlen(key) + 2;
                seckey = (char *)malloc(seckey_size);

                if (!seckey)
                {
                    errs++;
                    fprintf(stderr,
                            "iniparser: out of mem\n");
                    goto out;
                }

                snprintf(seckey, seckey_size, "%s:%s", section, key);
                errs = dictionary_set(dict, seckey, val) ;
                free(seckey);
                seckey = NULL;
            }
            break ;

            case LINE_ERROR:
                fprintf(stderr, "iniparser: syntax error in %s (%d):\n",
                        ininame,
                        lineno);
                fprintf(stderr, "-> %s\n", full_line);
                errs++ ;
                break;

            default:
                break ;
        }

        memset(line, 0, ASCIILINESZ);

        if (full_line)
        {
            free(full_line);
            full_line = NULL;
        }

        if (errs < 0)
        {
            fprintf(stderr, "iniparser: memory allocation failure\n");
            break ;
        }
    }

out:

    if (errs)
    {
        dictionary_del(dict);
        dict = NULL ;
    }

    if (val)
    {
        free(val);
        val = NULL;
    }

    if (key)
    {
        free(key);
        key = NULL;
    }

    if (section)
    {
        free(section);
        section = NULL;
    }

    if (full_line)
    {
        free(full_line);
        full_line = NULL;
    }

    if (prev_line)
    {
        free(prev_line);
        prev_line = NULL;
    }

    #ifdef SUPPORT_FATFS
    f_close(&USER_SYS_CONFIG_File);
    #else
    if (in) 
    {
        fclose(in);
        in = NULL;
    }
    #endif
    return dict ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Free all memory associated to an ini dictionary
  @param    d Dictionary to free
  @return   void

  Free all memory associated to an ini dictionary.
  It is mandatory to call this function before the dictionary object
  gets out of the current context.
 */
/*--------------------------------------------------------------------------*/
void iniparser_freedict(dictionary * d)
{
    dictionary_del(d);
}

/* vim: set ts=4 et sw=4 tw=75 */
