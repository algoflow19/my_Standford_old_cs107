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

void ListRecordsInRange(multitable *zipCodes, char *low, char *high)
{
  char *endpoints[] = {low, high};
  MultiTableMap(zipCodes, InRangePrint, endpoints);
}
static void InRangePrint(void *keyAddr, void *valueAddr, void *auxData)
{
  char *zipcode;
  char *city;
  char *low;
  char *high;

  zipcode=keyAddr;
  low=auxData[0];
  high= auxData[1];
  vector *citysList=valueAddr;
  int cityNum=VectorLength (citysList);
  if(cityNum==0) return;
  int cityNameLen=strlen(VectorNth (citysList,0));
  city=malloc(cityNameLen+1);
  strcpy(city,VectorNth (citysList,0));
  int totalLen=strlen(city)+1;
  for(int i=1;i<cityNum;i++){
      cityNameLen=strlen(VectorNth (citysList,i));
      city=realloc(city,totalLen+1+cityNameLen+1);
      city[totalLen]=',';
      strcpy(city+totalLen+1,VectorNth (citysList,i));
      totalLen+=2+cityNameLen;
    }
  if ( (strcmp(zipcode, low) >= 0) && (strcmp(zipcode, high) <= 0) )
    printf("%5s: %s\n", zipcode, city);
}


int main()
{

}
