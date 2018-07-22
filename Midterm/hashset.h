#ifndef HASHSET_H
#define HASHSET_H

typedef int (*HashSetHashFunction)(const void *elem, int numBuckets);
typedef int (*HashSetCompareFunction)(const void *elem1, const void *elem2);
typedef int (*HashSetFreeFunction)(void *elem);

typedef enum {
  false,
  true
} bool;

typedef struct {
  void *elems;
  // pointer to dynamically allocated space
  int elemsize;
  // the size of the client elements in bytes
  int count;
  // the effective size of the hashset
  int alloclength;
  // the allocated size of the hashset
  HashSetHashFunction hashfn;
  HashSetCompareFunction cmpfn;
  HashSetFreeFunction freefn;
} hashset;

/**
 * Initializes the raw space addressed by hs so that
 * it represents an empty hashset otherwise capable
 * of storing up to 64 client elements of the specified
 * size. The specified hash, compare, and free functions
 * are used to guide insertion and manage deletion.
 *
 * @param hs the address of the raw hashset being initialized.
 * @param elemsize the size of the client elements being store (in bytes).
 * @param hashfn the generic hash function that hashed and reduces client
 * elements to some number in the range [0, alloclength).
 * @param cmpfn the generic comparison function that returns a negative, zero,
 * or positive value, depending on whether the first element is
 * less than, equal to, or greater than the second element.
 * @param freefn the function that should be applied to any element ever
 * replaced or deleted. If there aren't any freeing needs,
 * then freefn should be set equal to NULL.
*/
void HashSetNew(hashset *hs, int elemsize,
                HashSetHashFunction hashfn,
                HashSetCompareFunction cmpfn,
                HashSetFreeFunction freefn){
  hs->alloclength=64;
  hs->elemsize=elemsize;
  hs->hashfn=hashfn;
  hs->cmpfn=cmpfn;
  hs->freefn=freefn;
  hs->count=0;
  hs->elems=malloc((elemsize+sizeof(bool))*64);
  memset(hs->elems,0,(elemsize+sizeof(bool))*64);

}

void *getPosNode(hashset *hs,int pos){
  return (char*)hs->elems+pos*(sizeof(bool)+hs->elemsize);
}


/**
* Enters the element address by elem into the hashset
* addressed by hs. As with your Assignment 3 vector and
* hashset, this particular hashset makes shallow copies
* of the elements whenever they're being inserted for the
* first time.
*
* If the new element matches some previously inserted element,
* then the old one is disposed of and the new one is copied
* in. Otherwise, the new element is dropped into a previously
* unassigned bucket, and the logical size of hashset increases by
* one.
*
* @param hs the address of the hashset being updated.
* @param elem the address of the elemsize-byte figure being
*
inserted into the addressed hashset.
* @return true if the new element is copied into a previously
*
unoccupied bucket, and false if it replaces
*
a previously inserted one.
*/

bool HashSetEnter(hashset *hs, void *elem)
{
  if (hs->count > (3 * hs->alloclength / 4))
    HashSetRehash(hs); // you’ll implement this function for part c
  int bucketNum=hs->hashfn(elem,hs->alloclength);
  void *node;
  for(int i=0;true;i++){
      bucketNum+=i;
      node=getPosNode (hs,bucketNum%hs->alloclength);
      if(*(bool*)node != false ){
          node= (void*)((char*)node+hs->elemsize);
          if(hs->cmpfn( node,elem)==0){
              if(hs->freefn!=NULL)
                hs->freefn(node);
              memcpy(node,elem,hs->elemsize);
              return false;
            }
          continue;
        }
      *(bool*)node=true;
      memcpy((char*)node+sizeof(bool),elem,hs->elemsize);
      hs->count++;
      return true;
    }

}

/**
 * Doubles the number of buckets held by the addressed hashset,
 * and redistributes all of the elements.
 *
 * @param hs the address of the hashset being resized.
*/
static void HashSetRehash(hashset *hs)
{
  void *originElems=hs->elems;
  int originCount=hs->count;
  int originLength=hs->alloclength;
  hs->count=0;
  hs->alloclength*=2;
  hs->elems=malloc((elemsize+sizeof(bool))*hs->alloclength);
  memset(hs->elems,0,hs->alloclength*(hs->elemsize+sizeof(bool)));

  for(int i=0;i<originLength;i++){
      void *node=(char*)originElems+i*(sizeof(bool)+hs->elemsize);
      if(*(bool*)node==false) continue;
      HashSetEnter (hs,(char*)node+sizeof(bool));
      if(hs->count==originCount) break;
    }
  free(originElems);
}



#endif // HASHSET_H
