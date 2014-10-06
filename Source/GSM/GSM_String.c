

//GSM_String
//字符串查找
extern int GSM_strstr (const char * Main_str,const char * search_str);
//字符串长度
extern int GSM_strlen (const char * str);
//字符串比较
extern int GSM_strcmp (const char * src,const char * dst);
//字符串分割
extern char * GSM_strsep (char **stringp, const char *delim);
//数字转字符串
extern void GSM_ltoa(char offset, char*ps,unsigned long src);
//字符串转数字
extern long GSM_atol( const char * str );

#define NULL ((void *)0)

//在Main_str中查找 search_str 返回Main_str中数组的下表 0未找到
// 7==strstr("12345678","78"); 
int GSM_strstr (const char * Main_str,const char * search_str)
{
    int i;
    char *cp = (char *)Main_str;
    char *s1, *s2;

    if ( !*search_str )
        return(0);

    while (*cp) {
        i++;
        s1 = cp;
        s2 = (char *) search_str;

        while ( *s1 && *s2 && !(*s1-*s2) )
            s1++, s2++;

        if (!*s2)
            return(i);

        cp++;
    }

    return(0);

}
// 3==strlens("123");
int GSM_strlen (const char * str)
{
        const char *eos = str;

        while( *eos++ ) ;

        return( (int)(eos - str - 1) );
}

int GSM_strcmp (const char * src,const char * dst)
{
        int ret = 0 ;

        while( ! (ret = *(unsigned char *)src - *(unsigned char *)dst) && *dst)
                ++src, ++dst;

        if ( ret < 0 )
                ret = -1 ;
        else if ( ret > 0 )
                ret = 1 ;

        return( ret );
}
//在字符串中查找给定字符第一个匹配处
static char * GSM_strchr (
        const char * string,
        int ch
        )
{
        while (*string && *string != (char)ch)
                string++;

        if (*string == (char)ch)
                return((char *)string);
        return(NULL);
}
//在字符串中查找给定字符集中的字符 
static char * GSM_strpbrk( register const char *cs, register const char *delim )
{
        register const char *search;

        do
        {
                for( search=delim; *search && *search!=*cs; search++ )
                        ;       /* look if character from cs is also in delim */

                if( *search )           /* if found the character in delim, */
                        return( (char *)cs );   /* return pointer        */
        } while( *cs++ );

        return( NULL );                 /* no matching character found     */
}

//字符串分割
//应用实例
//	char str1[]="x=\n12\nab";
//	char *p=str1;
//	char *tk,*tk1;
//	tk=strsep (&p, "\n");
//	tk1=strsep (&p, "\n");
//strtok
//tk:  x=    p: 12\nab
//tk1: 12    p: ab
char * GSM_strsep (char **stringp, const char *delim)
{
  char *begin, *end;

  begin = *stringp;
  if (begin == NULL)
    return NULL;

  /** A frequent case is when the delimiter string contains only one
     character.  Here we don't need to call the expensive `strpbrk'
     function and instead work using `strchr'.  */
  if (delim[0] == '\0' || delim[1] == '\0')
    {
      char ch = delim[0];

      if (ch == '\0')
	end = NULL;
      else
	{
	  if (*begin == ch)
	    end = begin;
	  else if (*begin == '\0')
	    end = NULL;
	  else
	    end = GSM_strchr (begin + 1, ch);
	}
    }
  else
    /** Find the end of the token.  */
    end = GSM_strpbrk (begin, delim);

  if (end)
    {
      /** Terminate the token and set *STRINGP past NUL character.  */
      //*end++ = '\0';
      *end++;
      *stringp = end;
    }
  else
    /** No more delimiters; this is the last token.  */
    *stringp = NULL;

  return begin;
}


/*-------------------------------------------------
字符串寻找
    字符串包含某字符串时，返回_OK, 只要有 1 个不同，返回_ERROR
    char  ptr: 存储区指针

     返回数据相等的末尾位置
     0 1 2 3 4 5 6 7 8    ===> i = 3
           3 4 5 6 |      ===> j = 4
                   |
                   |
                   |
                   |
          i + j == 7
          
author：wangtai

-------------------------------------------------*/

char  StrSearch(char  *a, char  *b, unsigned char  aLength, unsigned char  bLength)
{
    char  i, j, n;
    
    if(aLength < bLength)   //  重要条件
    {
    	return  0;
    }
    
    for(i=0; i<(aLength-bLength+1); i++)
    {
        n = 0;
        
        for(j=0; j<bLength; j++)
        {
            if(a[i+j] == b[j])
            {
                n++;
            }
            else
            {
                n = 0;
                break;     
            }
        }
        
        if(n == bLength)
        {
            return  (i+j); // 返回比较字符串的后一个字节的位置
        }    
    }
    
    return  0;            
}


//ps[10]=4294967295 4字节最大值
void GSM_ltoa(char offset, char*ps,unsigned long src)
{
    //低位在高端，低位在低地址； 小端模式
    char *s = ps+offset;
    char i;
    for(i=0;i<10;i++)
      *s++='0';
    s--;
    while(src>0) 
    {
        *s-- = src%10 + '0';
        src/=10;
    }
}

long GSM_atol( const char * S_str )
{
        register long result       = 0                                  ;
        register unsigned char ch                                       ;
        unsigned char neg          = 0                                  ;
        /* Skip white space (we could have used ctype isspace() */
        do                                                               /* white space */
        {
            ch = *S_str++                                               ;
        } while ((ch <= 0x20))                                          ;           

        if ( ch == 0x2D )                                                /* check for '-' */
                neg = 1                                                 ;
        else if ( ch != 0x2B )                                           /* check for '+' */
                S_str--                                                 ;
        for ( ;; )
        {
            ch = *S_str++ - '0'                                         ;
            if ( ch > 9 )
                    break                                               ;/* end of string reached */
            if ( neg )
                    ch = (signed char)(-((signed char)ch))              ;
            result = 10 * result + (signed char)ch                      ;
        }
        return result                                                   ;
}
