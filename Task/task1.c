#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef unsigned int uint32_t;

/* Error codes for gconv functions.  */
enum
{
  __GCONV_OK = 0,
  __GCONV_EMPTY_INPUT,
  __GCONV_ILLEGAL_INPUT,
};

#define hex2dec(x) ((x) <= '9' ? ((x)-'0') : ((x)<='F') ? ((x)-'A'+10) : ((x)-'a'+10))

#define __UNKNOWN_CHAR ((wchar_t) 0xfffd)
int unescape_char(const char **s, size_t avail)
{
    char ch = **s;
    char ch1;
    char ch2;
    int idx;
    if (avail == 0)
        return __UNKNOWN_CHAR;
    
    printf("input char : %c ", ch);
    (*s)+=1;
    if (ch != '%')
        return ch;

    if (avail < 2)
        return __UNKNOWN_CHAR;

    ch1 = (*s)[0];
    printf("ch1 %d", ch1);
    ch2 = (*s)[1];
    printf("ch2 %d", ch2);
    
    //printf("input char : %c %c ", ch1, ch2);

    idx = hex2dec(ch1) * 16 + hex2dec(ch2);
    //printf("%d", idx);
    if (idx >= 128)
    {   
        (*s) -= 1;
        return __UNKNOWN_CHAR;
    }
    (*s) += 2;
    return idx;
}

int unescape_query_string(const char **inptrp, const char *inend, char **outptrp, const char *outend)
{
    int result = __GCONV_EMPTY_INPUT;

    const char *inptr = *inptrp;
    char *outptr = *outptrp;

    while (inptr != inend)
    {
        int ch = *inptr;
        if (ch != '%'){
            ++inptr;
        }
        else
        {
            ch = unescape_char(&inptr, inend - inptr);
            if (ch == __UNKNOWN_CHAR)
            {
                result = __GCONV_ILLEGAL_INPUT;
                break;
            }
        }
        *outptr = ch;
        outptr += 1;
    }
    *inptrp = inptr;
    *outptrp = outptr;
    return result;
}

#define BUFF_SIZE 10


int main()
{
    size_t len = 5;
    char * buff =(char *) malloc(BUFF_SIZE);
    char * result =(char *) malloc(BUFF_SIZE);
    for (int i=0;i<len;++i)
        buff[i]=getchar();
    const char *inptr = buff;
    char *outptrp = result;
    unescape_query_string(&inptr, buff+len, &outptrp, result+BUFF_SIZE);
    printf("%s\n", result);
    return 0;
}
