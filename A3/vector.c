#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <search.h>

const static int DEFUAT_INITALLOC_LENGTH=8;

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation)
{
  assert(v!=NULL);
  v->elemSize=elemSize;
  v->logicSLength=0;
  v->freeFun=freeFn;
  assert(initialAllocation>=0);
  if(initialAllocation==0){
    v->allocLength=DEFUAT_INITALLOC_LENGTH;
    v->initAllocLength=DEFUAT_INITALLOC_LENGTH;
    }
  else{
    v->allocLength=initialAllocation;
    v->initAllocLength=initialAllocation;
    }
  v->datas=malloc(elemSize*v->initAllocLength);
}

void VectorDispose(vector *v)
{
  if(v==NULL){
      printf ("Attation:Disposed NULL vector.");
      return;
    }
  if(v->freeFun!=NULL){
      for(int i=0;i<v->logicSLength;i++)
          v->freeFun((char*)v->datas+v->elemSize*i);
    }
  free(v->datas);
}

int VectorLength(const vector *v)
{
  return v->logicSLength;
}

void *VectorNth(const vector *v, int position)
{
  assert(position>=0);
  assert(position<v->logicSLength);
  return (char*)v->datas+v->elemSize*position;
}

void VectorReplace(vector *v, const void *elemAddr, int position)
{
  void* elemInVecotr=VectorNth (v,position);
  if(elemInVecotr==elemAddr) return;
  if(v->freeFun!=NULL)
    v->freeFun(elemInVecotr);
  memcpy(elemInVecotr,elemAddr,v->elemSize);
}

void VectorInsert(vector *v, const void *elemAddr, int position)
{
  assert(position>=0);
  assert(position<=v->logicSLength);
  if(v->allocLength==v->logicSLength){
      v->datas=realloc(v->datas,(v->allocLength+v->initAllocLength)*v->elemSize);
      assert(v->datas!=NULL);
      v->allocLength+=v->initAllocLength;
    }
  if(position!=v->logicSLength){
      int endLength=(v->logicSLength-1)+1-position;
      void* insertAddr=VectorNth (v,position);
      memmove((char*)insertAddr+v->elemSize,insertAddr,endLength*v->elemSize);
      memcpy(insertAddr,elemAddr,v->elemSize);
      v->logicSLength++;
      return;
    }

  memcpy((char*)v->datas+ v->elemSize*position , elemAddr , v->elemSize);
  v->logicSLength++;
}

void VectorAppend(vector *v, const void *elemAddr)
{
  VectorInsert (v,elemAddr,v->logicSLength);
}

void VectorDelete(vector *v, int position)
{
  assert(position>=0);
  assert(position<v->logicSLength);
  void* delAddr=VectorNth (v,position);
  if(v->freeFun!=NULL)
    v->freeFun(delAddr);
  if(position==v->logicSLength-1){
      v->logicSLength--;
      return;
    }
  size_t endLength=v->logicSLength-1-position;
  memmove(delAddr,(char*)delAddr+v->elemSize,endLength*v->elemSize);
  v->logicSLength--;

}

void VectorSort(vector *v, VectorCompareFunction compare)
{
  assert(compare!=NULL);
  qsort (v->datas,v->logicSLength,v->elemSize,compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{
  assert(mapFn!=NULL);
  int elemSize=v->elemSize;
  char *datas=(char*)v->datas;
  for(int i=0;i<v->logicSLength;i++)
    mapFn(datas+elemSize*i,auxData);
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted)
{

  assert(v!=NULL);
  assert(startIndex>=0);
  assert(startIndex<=v->logicSLength);
  assert(searchFn!=NULL);
  if(startIndex==v->logicSLength) return -1;
  char* datas=(char*)v->datas;
  size_t searchSize=v->logicSLength-startIndex;
  void* elemAddr;
  int result;
  if(!isSorted){
      elemAddr=lfind (key,datas+startIndex*v->elemSize,&searchSize,v->elemSize,searchFn);
      assert(searchSize==v->logicSLength-startIndex);
      if(elemAddr==NULL) return -1;
    }
  else{

      elemAddr=bsearch (key,datas+startIndex*v->elemSize,searchSize,v->elemSize,searchFn);
      if(elemAddr==NULL) return -1;
    }

  result=((unsigned long)elemAddr-(unsigned long)datas)/v->elemSize;
  return result;
}




