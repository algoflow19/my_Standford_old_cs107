#include <stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct node node;

struct node{
  node *next;
  char headOfString[];
};

int *doSerializeList(const void * listHead,int *privouseCreated,int totalByteused){
  if(listHead==NULL) return privouseCreated;
  *privouseCreated++;
  int len=strlen((char*)((node*)listHead+1));
  privouseCreated=realloc(privouseCreated,totalByteused+len+1);
  strcpy((char*)privouseCreated+totalByteused,(char*)((node*)listHead+1));
  return doSerializeList ((const void*)*(node**)listHead,privouseCreated,totalByteused+len+1);
}

int *serializeList(const void * listHead){
  int *SL=malloc(sizeof(int));
  *SL=0;
  if(listHead==NULL) return SL;
  return doSerializeList ((const void*)*(node**)listHead ,SL,sizeof(int) );
}

int main()
{
  char kk[12];
  char kk2[11]="123445";
  kk[0]='\0';
  strcpy(kk,kk2);
  printf ("%s\n",kk);
}
