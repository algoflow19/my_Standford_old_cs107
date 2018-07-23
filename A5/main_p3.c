#include<stdio.h>
#include<string.h>

int main(){
  int k='A';
  char ckecker[2];
  strcpy(ckecker,(char*)&k);
  if(k=='A')
    printf ("This is a little-end system\n");
  else
    printf ("This is a big-end system\n");
}
