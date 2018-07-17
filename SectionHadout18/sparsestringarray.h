#ifndef SPARSESTRINGARRAY_H
#define SPARSESTRINGARRAY_H
#include "vector.h"

#include<assert.h>
#include<stdlib.h>
#include<string.h>

typedef struct {
  bool *bitmap;
  vector strings;
} group;
z

typedef struct {
  group *groups;
  int numGroups;
  int arrayLength;
  int groupSize;
} sparsestringarray;


/**
 * Function: SSAMap
 * ----------------
 * Applies the specified mapping routine to every single index/string pair
 * (along with the specified auxiliary data). Note that the mapping routine
 * is called on behalf of all strings, both empty and nonempty.
*/
typedef void (*SSAMapFunction)(int index,const char *str,void *);

void SSAMap(sparsestringarray *ssa, SSAMapFunction mapfn, void *auxData){
  group *targetGroup;
  for(int i=0;i<ssa->numGroups;i++){
      targetGroup=ssa->groups+i;
      if(targetGroup->bitmap==NULL){
          for(int n=0;n<ssa->groupSize;n++)
            mapfn(i*ssa->groupSize+n,"",auxData);
        }
      else{
          int currentPos=0;
          for(int n=0;n<ssa->groupSize;n++){
              if(targetGroup->bitmap[n]==false)
                mapfn(i*ssa->groupSize+n,"",auxData);
              else{
                  mapfn(i*ssa->groupSize+n,*(char**)VectorNth(&(targetGroup->strings),currentPos),auxData);
                  currentPos++;
                }
            }
        }
    }
}




/**
* Function: SSANew
* ----------------
* Constructs the sparsestringarray addressed by the first argument to
* be of the specified length, using the specified group size to decide
* how many groups should be used to back the implementation. You can
* assume that arrayLength is greater than groupSize, and for simplicity you
* can also assume that groupSize divides evenly into arrayLength.
*/
void SSANew(sparsestringarray *ssa, int arrayLength, int groupSize){
  assert(arrayLength>0&&arrayLength>groupSize&&ssa!=NULL);

  ssa->arrayLength=arrayLength;
  ssa->groupSize=groupSize;
  ssa->numGroups=(arrayLength-1)/groupSize + 1;  // Also you can consider the not even divided status.
  ssa->groups=malloc(sizeof(group)*ssa->numGroups);
  assert(ssa->groups!=NULL);
  memset(ssa->groups,0,sizeof(group)*ssa->numGroups);

}

void stringFree(void *elemaddr){
  free(*((char**)elemaddr));
}

/**
 * Function: SSADispose
 * --------------------
 * Disposes of all the resources embedded within the addressed
 * sparsestringarray that have built up over the course of its
 * lifetime.
 */

void SSADispose(sparsestringarray *ssa){
  for(int i=0;i<ssa->numGroups;i++){
      group *targetGroup=ssa->groups+i;
      if(targetGroup->bitmap==NULL) continue;
      free(targetGroup->bitmap);
      VectorDispose(&targetGroup->strings);
    }
  free(ssa->groups);

}

/**
* Function: SSAInsert
* -------------------
* Inserts the C string addressed by str into the sparsestringarray addressed
* by ssa at the specified index. If some nonempty string already resides
* at the specified index, then it is replaced with the new one. Note that
* SSAInsert makes a deep copy of the string address by str.
*/
bool SSAInsert(sparsestringarray *ssa, int index, const char *str){
  assert(index>=0&&index<=ssa->arrayLength);
  int groupIndex=index/ssa->groupSize;
  int indexInGroup=index%ssa->groupSize;
  group *targetGroup=ssa->groups+groupIndex;
  if(targetGroup->bitmap==NULL){
      targetGroup->bitmap=malloc(sizeof(bool)*ssa->groupSize);
      assert(targetGroup->bitmap!=NULL);
      memset(targetGroup->bitmap,false,sizeof(bool)*ssa->groupSize);
      VectorNew (& (targetGroup->strings),sizeof(char*),stringFree,3);
    }
  int currentInsertPos=0;
  for(int i=0;i<indexInGroup;i++)
    if(*(targetGroup->bitmap+i)!=false)
      currentInsertPos++;
  char* tmpString=malloc(strlen(str)+1);
  memcpy(tmpString,str,strlen(str)+1);
  if(*(targetGroup->bitmap+indexInGroup)==true){
      VectorReplace (&(targetGroup->strings),&tmpString,currentInsertPos);
      return false;
    }
  else{
      VectorInsert (&(targetGroup->strings),&tmpString,currentInsertPos);
      *(targetGroup->bitmap+indexInGroup)=true;
      return true;
    }
}


#endif // SPARSESTRINGARRAY_H
