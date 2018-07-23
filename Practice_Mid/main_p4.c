#include <stdio.h>
#include"hashset.h"
typedef int (*MultiSetHashFunction)(const void *elem, int numBuckets);
typedef int (*MultiSetCompareFunction)(const void *elem1, const void* elem2);
typedef void (*MultiSetMapFunction)(void *elem, int count, void *auxData);
typedef void (*MultiSetFreeFunction)(void *elem);

typedef struct {
  hashset elements;
  int elemSize;
  MultiSetFreeFunction free;
} multiset;

/**
* Function: MultiSetNew
* ---------------------
* Initializes the raw space addressed by ms to be an empty otherwise
* capable of storing an arbitrarily large number of client elements of the
* specified size. The numBuckets, hash, compare, and free parameters
* are all supplied with the understanding that they’ll be passed right
* right through to HashSetNew. You should otherwise interact with the
* embedded hashset using only those functions which have the authority
* to access the hashset's fields.
*/
void MultiSetNew(multiset *ms, int elemSize, int numBuckets,
                 MultiSetHashFunction hash, MultiSetCompareFunction compare,
                 MultiSetFreeFunction free)
{
  ms->elemSize=elemSize;
  ms->free=free;
  HashSetNew (&ms->elements,elemSize+sizeof(int),numBuckets,hash,compare,free);
}

/**
* Function: MultiSetDispose
* -------------------------
* Disposes of all previously stored client elements by calling
* HashSetDispose.
*/
void MultiSetDispose(multiset *ms){
  HashSetDispose(& ms->elements);
}

/**
* Function: MultiSetEnter
* -----------------------
* Ensures that the client element addressed by elem is included
* in the multiset. If the element isn’t present, then the element
* is inserted for the very first time and its multiplicity is registered
* as 1. If the element matches one already present, then the old one
* is disposed of (if the free function is non-NULL), the new element
* is replicated over the space of the one just disposed of, and
* the multiplicity is incremented by 1.
*/
void MultiSetEnter(multiset *ms, const void *elem){
  void *nodeAddress=HashSetLookup (&ms->elements,elem);
  if(nodeAddress==NULL){
      char dummy[ms->elements.elemSize];
      nodeAddress=dummy;
      memcpy(nodeAddress,elem,ms->elemSize);
      *(int*)((char*)nodeAddress+ms->elemSize)=1;
      HashSetEnter (&ms->elements,nodeAddress);
    }
  else{
      if(ms->free!=NULL) ms->free(nodeAddress);
      memcpy(nodeAddress,elem,ms->elemSize);
      *((char*)nodeAddress+ms->elemSize)+=1;
    }
}

typedef struct multMapauxData
{
  MultiSetMapFunction mf;
  int elemSize;
  void *auxData;
} multMapauxData;

void multHashFun(void *elemaddr,void *auxData){
  multMapauxData *aux=auxData;
  aux->mf(elemaddr, *(int*)((char*)elemaddr+aux->elemSize),aux->auxData);
}

/**
* Function: MultiSetMap
* ---------------------
* Applies the specified MultiSetMapFunction to every single element/int
* pair maintained by the multiset, passing the supplied auxData argument as
* the third argument to every single application.
*/
void MultiSetMap(multiset *ms, MultiSetMapFunction map, void *auxData){
  multMapauxData aux;
  aux.auxData=auxData;
  aux.elemSize=ms->elemSize;
  aux.mf=map;
  HashSetMap (& ms->elements,multHashFun,& aux);
}

/*
 * Answer of p4 start here!
 */

typedef struct {
  const char *licensePlate;
  int numTickets;
} maxTicketsStruct;

void queenFindHashFun(void *elemaddr,int ticketsNum,void *auxData){
  maxTicketsStruct *aux=auxData;
  if(ticketsNum>aux->numTickets){
      memcpy(aux->licensePlate,elemaddr,8*sizeof(char));
    }
}

void FindQueenOfParkingInfractions(multiset *ms, char licensePlateOfQueen[]){
  maxTicketsStruct aux={licensePlateOfQueen,0};
  MultiSetMap (&ms->elements,queenFindHashFun,&aux);
}


int main()
{

}
