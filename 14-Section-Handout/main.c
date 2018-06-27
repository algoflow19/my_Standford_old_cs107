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
  SetNew (set,sizeof(int),&cmpInt);
  int *elemptr=malloc(sizeof(int));

  assert(SetSearch (set,elemptr)==NULL);
  for(int i=0;i<100;i++){
      *elemptr=rand()%5;
      SetAdd (set,elemptr);
    }
  int* unitArrays=(int*)set->unitArrays;


  for(int i=0;i<10;i++){
      printf ("The pos:%d data:%d lhs:%d rhs:%d \n",i,*((int*)unitArrays),*((int*)unitArrays+1),*((int*)unitArrays+2));
      unitArrays+=3;
    }
  printf ("SetSize: %d\n",set->toAddPlace);

  return 0;
}
