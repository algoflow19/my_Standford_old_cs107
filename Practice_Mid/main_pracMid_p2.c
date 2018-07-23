#include<stdio.h>
#include<string.h>


void *packetize(const void *image, int size, int packetSize){
  int currentBlockSize;
  void *nodeHead;
  void **currentNode=&nodeHead;
  int originTotalSize=size;
  while(size!=0){
      if(size>=packetSize)
        currentBlockSize=packetSize;
      else
        currentBlockSize=size;
      size-=currentBlockSize;
      *currentNode=malloc(currentBlockSize+sizeof(void*));
      memcpy(*currentNode,(const char*)image+originTotalSize-size,currentBlockSize);
      currentNode=(void**)((char*)*currentNode+currentBlockSize);
      *currentBlockSize=NULL;
    }
  return nodeHead;
}


int main(){

  return 0;
}
