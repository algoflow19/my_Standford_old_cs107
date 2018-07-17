#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include"multitable.h"
typedef struct node node;

struct node{
  node *next;
  char headOfString[];
};

int *doSerializeList(const void * listHead,int *privouseCreated,int totalByteused){
  if(listHead==NULL) return privouseCreated;
  *privouseCreated++;
  int len=strlen((const char*)((const void **)listHead+1));
  privouseCreated=realloc(privouseCreated,totalByteused+len+1);
  strcpy((char*)privouseCreated+totalByteused,(char*)((node**)listHead+1));
  return doSerializeList ((const void*)*(node**)listHead,privouseCreated,totalByteused+len+1);
}

int *serializeList(const void * listHead){
  int *SL=malloc(sizeof(int));
  *SL=0;
  if(listHead==NULL) return SL;
  return doSerializeList ((const void*)*(node**)listHead ,SL,sizeof(int) );
}

static void InRangePrint(void *keyAddr, void *valueAddr, void *auxData)
{
  char *zipcode;
  char *city;
  char *low;
  char *high;
  char **endPoints=auxData;
  zipcode=keyAddr;
  low=endPoints[0];
  high= endPoints[1];
  city = *(char **) valueAddr;

  if ( (strcmp(zipcode, low) >= 0) && (strcmp(zipcode, high) <= 0) )
    printf("%5s: %s\n", zipcode, city);
}


void ListRecordsInRange(multitable *zipCodes, char *low, char *high)
{
  char *endpoints[] = {low, high};
  MultiTableMap(zipCodes, InRangePrint, endpoints);
}


int main()
{

}
