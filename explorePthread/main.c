#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "vector.h"
#include<string.h>

struct Vector;

typedef struct {
  char *girl;
  char *boy;
} couple;

typedef struct{
  Vector *couples;
  Vector *boys;
} pairAuxData;

void pairFreeFun(couple *elemaddr){
  free(elemaddr->boy);
  free(elemaddr->girl);
}


void generatePairForEveryHelper(char *item,Vector *boys,Vector *couples){
  int len=VectorLength (boys);
  for(int i=0;i<len;i++){
      couple dummy;
      dummy.boy=strdup(VectorNth (boys,i));
      dummy.girl=strdup(item);
      VectorAppend (couples,&dummy);
    }

}

void generatePairForEvery(char *item,pairAuxData *auxdata){
  generatePairForEveryHelper (item,auxdata->boys,auxdata->couples);
}

vector generateAllCouples(vector *boys, vector *girls)
{
  vector couples;
  VectorNew(&couples, sizeof(couple), pairFreeFun, 0);
  pairAuxData aux;
  aux.couples=&couples;
  aux.boys=boys;
  VectorMap (girls,generatePairForEvery,&aux);

}

typedef bool (*VectorSplitFunction)(const void *elemAddr);
void VectorSplit(vector *original,
                 vector *thoseThatPass,
                 vector *thoseThatFail,
                 VectorSplitFunction test)
{
  VectorNew (thoseThatPass,original->elemSize,original->freeFun,original->initAllocLength);
  VectorNew (thoseThatFail,original->elemSize,original->freeFun,original->initAllocLength);
  int len=VectorLength (original);
  for(int i=0;i<len;i++){
      void *elemaddr=VectorNth (original,i);
      if(test(elemaddr)==false){
          VectorAppend (thoseThatFail,elemaddr);
        }
      else
        VectorAppend (thoseThatPass,elemaddr);
    }
  free(original->datas);
  VectorNew (original,original->elemSize,original->freeFun,original->initAllocLength);

}
main()
{


  exit(0);
}
