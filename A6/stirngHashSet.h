#ifndef _stirnghashset_
#define _stirnghashset_
#include"stdio.h"
static signed long kHashMultiplier = -1664117991L;
static int StringHash(const char **s, int numBuckets)
{
  int i;
  unsigned long hashcode = 0;


  for (i = 0; i < strlen(*s); i++)
    hashcode = hashcode * kHashMultiplier + tolower((*s)[i]);

  return hashcode % numBuckets;
}

void stringFreeFun(void* elemAddr){

  free(*(char**)elemAddr);
}

int stringCompareFun(void* elemAddr1,void *elemAddr2){
  return strcmp(*(char**)elemAddr1,*(char**)elemAddr2);
}

#endif
