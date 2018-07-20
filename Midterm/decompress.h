#ifndef DECOMPRESS_H
#define DECOMPRESS_H
#include<string.h>
#include<stdlib.h>


typedef struct {
char *name;
char **friends;
int numfriends;
} person;

/**
* Accepts the address of the full data image storing
* all of the friendship information, and constructs
* and returns a dynamically allocated array of person
* structs storing exactly the same information.
*
* @param image the base address of the entire data image,
* as described on the previous page.
* @return the address of a dynamically allocated array
* of person records, where each record stores
* all of the friendship information about one
* person in the original data image.
*/

void *decompress(void *friendshipInfo){  // use [] is clean and good code.
  int totalPerson=*(int*)friendshipInfo;
  void *currentDecompressPos=(int*)friendshipInfo+1;
  void *personsRecord=malloc(sizeof(per)*totalPerson);
  person *currentPerson=personsRecord;
  for(int i=0;i<totalPerson;i++){
      currentPerson->name=strdup(currentDecompressPos);
      currentDecompressPos=(char*)currentDecompressPos+strlen(currentDecompressPos)/4 +4;

      int friendsNum=*(int*)currentDecompressPos;
      currentDecompressPos=(int*)currentDecompressPos+1;
      currentPerson->numfriends=friendsNum;
      currentPerson->friends=malloc(sizeof(char*)*friendsNum);

      char **currenPersonFriend=currentPerson->friends;
      for(int i=0;i<friendsNum;i++){
          *currenPersonFriend=strdup(currentDecompressPos);
          currenPersonFriend++;
          currentDecompressPos=(char**)currentDecompressPos+1;
        }
    }
  return personsRecord;
}


#endif // DECOMPRESS_H
