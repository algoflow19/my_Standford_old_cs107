#ifndef MULTITABLE_H
#define MULTITABLE_H
#include"hashset.h"
#include<stdlib.h>
#include<assert.h>
typedef int (*MultTableHashFunction)(void *keyAddr,int bucketnum);
typedef int (*MultTableCompareFunction)(void *keyAddr1,void *keyAddr2);
typedef void (*MultTableMapFunction)(void *keyAddr,void *elemAddr,void *auxData);

typedef struct {
  hashset mappings;
  int keySize;
  int valueSize;
} multitable;


void MultiTableNew(multitable *mt, int keySizeInBytes, int valueSizeInBytes,
                   int numBuckets, MultTableHashFunction hash,
                   MultTableCompareFunction compare){
  mt->keySize=keySizeInBytes;
  mt->valueSize=valueSizeInBytes;
  HashSetNew (&mt->mappings,sizeof(keySizeInBytes)+sizeof(vector),numBuckets,hash,compare,NULL);

}
void MultiTableEnter(multitable *mt, const void *keyAddr, const void *valueAddr){
  void *node=HashSetLookup (&mt->mappings,keyAddr);
  if(node==NULL){
      void *tmpNode=malloc(mt->keySize+sizeof(vector));
      memcpy(tmpNode,keyAddr,mt->keySize);
      vector wordsList;
      VectorNew (&wordsList,mt->valueSize,NULL,3);
      VectorAppend (&wordsList,valueAddr);
      memcpy((char*)tmpNode+mt->keySize,&wordsList,sizeof(vector));
      HashSetEnter (&mt->mappings,tmpNode);
      free(tmpNode);
    }
  else{
      vector* wordsList=(char*)node+mt->keySize;
      VectorAppend (wordsList,valueAddr);
    }

}

typedef void (*MultiTableMapFunction)(const void *keyAddr,
                                      void *valueAddr, void *auxData);

struct auxDataUnit
{
  multitable* mt;
  MultiTableMapFunction mtf;
  void *auxData;
};



typedef struct auxDataUnit auxDataUnit;

void multiValuesMap(void *keyValuesPair,auxDataUnit *auxUnit){
  vector *values=(char*)keyValuesPair+auxUnit->mt->keySize;
  int len=VectorLength (values);
  for(int i=0;i<len;i++){
      auxUnit->mtf(keyValuesPair,VectorNth (values,i),auxUnit->auxData);
    }
}

void MultiTableMap(multitable *mt, MultiTableMapFunction map, void *auxData){
  auxDataUnit aux={mt,map,auxData};
  HashSetMap (& mt->mappings,multiValuesMap,&aux);
}



#endif // MULTITABLE_H
