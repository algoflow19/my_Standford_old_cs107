
#include"queryHashSet.h"

int wordCompareFun(void* elemaddr1,void* elemaddr2){
    assert(elemaddr1!=NULL&&elemaddr2!=NULL);
    return strcmp(elemaddr1,elemaddr2);
}

void wordFreeFun(void* element){

}

static signed long kHashMultiplier = -1664117991L;
int WordHash(char *s, int numBuckets)
{
  int i;
  unsigned long hashcode = 0;

  for (i = 0; i < strlen(s); i++)
      hashcode = hashcode * kHashMultiplier + tolower(s[i]);

  return hashcode % numBuckets;
}

int posTitleCompareFun(void *elemaddr1,void *elemaddr2){
  struct pos *p1=elemaddr1;
  struct pos *p2=elemaddr2;
  return strcmp(p1->title,p2->title);
}

int posConutCompareFun(void *elemaddr1,void *elemaddr2){
  struct pos *p1=elemaddr1;
  struct pos *p2=elemaddr2;
  return p1->count-p2->count;
}
