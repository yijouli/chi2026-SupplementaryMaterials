#define MAX_INPUT_LEN 32
//copy user-control input string to output string, replace & with '&amp;'
char * escaped_copy_input(char *s){
if (strlen(s) > MAX_INPUT_LEN)
return NULL; //input too long
char *dst = (char*)malloc(MAX_INPUT_LEN * 4 + 1);
size_t dst_index = 0;
for (size_t i = 0; i < strlen(s); i++ )
if( '&' == s[i] ){
      dst[dst_index++] = '&';
      dst[dst_index++] = 'a';
      dst[dst_index++] = 'm';
      dst[dst_index++] = 'p';
      dst[dst_index++] = ';';
}
else
      dst[dst_index++] = s[i];
dst[dst_index] = '\0';
return dst;
}

