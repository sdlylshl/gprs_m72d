

//GSM_String
//�ַ�������
extern int GSM_strstr (const char * Main_str,const char * search_str);
//�ַ�������
extern int GSM_strlen (const char * str);

//��Main_str�в��� search_str ����Main_str��������±� 0δ�ҵ�
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