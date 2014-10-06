

//GSM_String
//字符串查找
extern int GSM_strstr (const char * Main_str,const char * search_str);
//字符串长度
extern int GSM_strlen (const char * str);

//在Main_str中查找 search_str 返回Main_str中数组的下表 0未找到
// 7==strstr("12345678","78"); 
int strstr (const char * Main_str,const char * search_str)
{
    char i;
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
int strlen (const char * str)
{
        const char *eos = str;

        while( *eos++ ) ;

        return( (int)(eos - str - 1) );
}