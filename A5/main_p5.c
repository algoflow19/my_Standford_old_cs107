#include<stdio.h>
#include<string.h>


const void* const kHeapStart=(void*)12345;
const int kHeapSize=123456;


#define isInHeap(pointer) kHeapStart<=(void*)pointer&&pointer<=kHeapEnd

typedef enum bool{
  false=0,
  true
} bool;

typedef struct list list;

struct list{
  int data;
  list *next;
};

/*
 * Check if the node occpy out of heap and if its next pointer point to leagl address.
 */





bool isListExist(){
  const void* const kHeapEnd=((char *)kHeapStart + kHeapSize - sizeof(list));
  // global const varibre could only initilizer by constant num.

  const char* pos;
  for(pos=kHeapStart;pos<=kHeapEnd;pos++){
      list *currentNode=pos;
      if(*(int*)currentNode==1&&isInHeap(currentNode->next)){
          currentNode=currentNode->next;
          if(*(int*)currentNode==2&&isInHeap(currentNode->next)){
              currentNode=currentNode->next;
              if(*(int*)currentNode==3&&currentNode->next==NULL)
                return true;
            }
        }
    }
  return false;
}

int main(){


  return 0;
}
