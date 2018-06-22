#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

const static int INIT_VECTOR_LENGTH=4;

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn)
{
  assert(elemSize>0);
  assert(numBuckets>0);
  assert(hashfn!=NULL);
  assert(comparefn!=NULL);

  h->elemSize=elemSize;
  h->numBuckets=numBuckets;
  h->count=0;
  h->hashfn=hashfn;
  h->comparefn=comparefn;
  h->freefn=freefn;

  h->buckets=(vector*)malloc(sizeof(*vector)*numBuckets);
  for(int i=0;i<numBuckets;i++){
      h->buckets[i]=malloc(sizeof(vector));
      VectorNew (h->buckets[i],elemSize,freefn,INIT_VECTOR_LENGTH);
    }

}

void HashSetDispose(hashset *h)
{
  assert(h!=NULL);
  if(h->freefn!=NULL)
  for(int i=0;i<h->numBuckets;i++)
    VectorDispose(h->buckets[i]);
  free(h->buckets);
}

int HashSetCount(const hashset *h)
{
  assert(h!=NULL);
  return h->count;
}

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData)
{

}

void HashSetEnter(hashset *h, const void *elemAddr)
{
  assert(elemAddr!=NULL&&h!=NULL);
  int bucketIndex=h->hashfn(elemAddr,h->numBuckets);
  assert(bucketIndex>=0&&bucketIndex<h->numBuckets);


}

void *HashSetLookup(const hashset *h, const void *elemAddr)
{
  assert(elemAddr!=NULL&&h!=NULL);
  int bucketIndex=h->hashfn(elemAddr,h->numBuckets);
  assert(bucketIndex>=0&&bucketIndex<h->numBuckets);


  return NULL;
}
