#include<stdio.h>
#include<stdlib.h>
#include<string.h>


int saferfscanf(char *target, FILE *in, unsigned long lt)
{
  int code = 0;
  unsigned long j;
  int s;
  char cs;

  char *t, gotit;

  t = target;
  j = 0;
  for(;;){
    if( (s = fgetc(in)) == EOF ){
      printf("saferfscanf: reached end of file\n"); code = 1; goto BACK;
    }
    cs = (char) s;
    gotit = 1;
    if( (cs == ' ') || (cs == '\n') || (cs == '\r') || (cs == '\t')){
      *t = 0; 
      if(strlen(target) > 0) break;
      /** if we are here we read nothing **/
      gotit = 0;
      j = 0;
    }
    if(gotit){
      ++j;
      if(j == lt){
	printf("===> %s\n", target);
	printf("saferfscanf: source string too long\n"); code = 1; goto BACK;
      }

      *t++ = cs;
    }

  }

  

 BACK:
  return code;
}


