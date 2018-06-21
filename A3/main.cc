#include <iostream>

using namespace std;

int intCmp(const void* i1,const void* i2){
  return *(int*)i1 - *(int*)i2;
}

/**
 * @brief evalPos used only for array.
 * @param target
 * @param base
 * @param elementSize
 * @return
 */
int evalPos(void* target,void *base,int elementSize){
  return (unsigned long)((char*)target-(char*)base)/elementSize;
}

int main()
{
  int array[100];
  for(int i=0;i<100;i++)
    array[i]=i+1;
  int* k=(int*)malloc (sizeof(int));
  *k=55
      ;
  int *result=(int*)bsearch (k,array,100,sizeof(int),intCmp);
  cout<<*result<<endl;
  cout<<evalPos (result,array,sizeof(int));
  return 0;
}
