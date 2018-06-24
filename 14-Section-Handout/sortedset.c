
#include"sortedset.h"

static const int kInitialCapacity = 4;

void SetNew(sortedset *set, int elemSize, int (*cmpfn)(const void *, const void *))
{
  assert(cmpfn!=NULL);
  assert(elemSize>0);
  set->allocLength=kInitialCapacity;
  set->cmp=cmpfn;
  set->elemSize=elemSize;
  set->storeStart=malloc((elemSize+8)*set->allocLength);
  set->toAddPlace=0;
  set->unitArrays=(char*)set->storeStart+4;
  *(int*)set->storeStart=-1;

}

void *SetSearch(sortedset *set, const void *elemPtr)
{
  assert(elemPtr!=NULL);
  if(*(int*)set->storeStart==-1) return NULL;
  return SetFind (set,elemPtr,0,false);
}

bool SetAdd(sortedset *set, const void *elemPtr)
{
  assert(elemPtr!=NULL);
  if(*(int*)set->storeStart==-1){
      AddNullNode(set,elemPtr);
      *(int*)set->storeStart=0;
      return true;
    }

  void* fatherNode=SetFind (set,elemPtr,0,true);
  int cmpResult=set->cmp(fatherNode,elemPtr);
  if(cmpResult==0) return false;
  if(set->allocLength==set->toAddPlace){
      set->allocLength+=kInitialCapacity;
      set->storeStart=realloc(set->storeStart,(set->elemSize+8)*(set->allocLength));
      set->unitArrays=(char*)set->storeStart+4;
    }

  if(cmpResult>0){
      *(int*)((char*)fatherNode+set->elemSize)=set->toAddPlace;
      AddNullNode (set,elemPtr);
    }
  else{
      *(int*)((char*)fatherNode+set->elemSize+4)=set->toAddPlace;
      AddNullNode (set,elemPtr);
    }

  return true;
}

void *SetFind(sortedset *set, const void *elemPtr, int pos, bool backtail)
{
  assert(pos>=0);
  assert(pos<set->toAddPlace);
  void *targetUnit=(char*)set->unitArrays+(set->elemSize+8)*pos;
  int cmpResult=set->cmp(targetUnit,elemPtr);
  int nextTargetPos;
  if(cmpResult==0) return targetUnit;
  if(cmpResult>0){
      nextTargetPos=*(int*)((char*)targetUnit+set->elemSize);
      if(nextTargetPos==-1){
          if(backtail) return targetUnit;
          return NULL;
        }
      return SetFind (set,elemPtr,nextTargetPos,backtail);
    }
  else{
      nextTargetPos=*(int*)((char*)targetUnit+set->elemSize+4);
      if(nextTargetPos==-1){
          if(backtail) return targetUnit;
          return NULL;
        }
      return SetFind (set,elemPtr,nextTargetPos,backtail);
    }
}

void AddNullNode(sortedset *set, const void *elemPtr){
  assert(elemPtr!=NULL);
  void* newUnit=(char*)set->unitArrays+(set->elemSize+8)*set->toAddPlace;
  memcpy(newUnit,elemPtr,set->elemSize);
  *(int*)((char*)newUnit+set->elemSize)=-1;
  *(int*)((char*)newUnit+set->elemSize+4)=-1;
  set->toAddPlace++;
}
