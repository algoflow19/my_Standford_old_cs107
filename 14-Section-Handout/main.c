#include"sortedset.h"
#include<stdio.h>
#include<math.h>

int cmpInt(const void *lhs, const void *rhs){
  return *(int*)lhs-*(int*)rhs;
}

int main()
{

  sortedset* set;
  set=malloc(sizeof(sortedset));  // mix the type with instance
  SetNew (set,4,&cmpInt);
  int *elemptr=malloc(sizeof(int));
  *elemptr=5;
  SetAdd (set,elemptr);
  *elemptr=3;
  SetAdd (set,elemptr);
  assert(*(int*)SetSearch (set,elemptr)==3);
  *elemptr=5;
  assert(*(int*)SetSearch (set,elemptr)==5);
  *elemptr=-99;
  assert(SetSearch (set,elemptr)==NULL);
  for(int i=0;i<100;i++){
      *elemptr=i;
      SetAdd (set,elemptr);
    }
  printf ("%d\n",set->toAddPlace);
  for(int i=0;i<100;i++){
      *elemptr=i;
      assert(SetAdd (set,elemptr)==false);
    }
  assert(set->elemSize==4);
  return 0;
}
