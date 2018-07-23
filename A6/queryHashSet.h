#ifndef _queryhashset_
#define _queryhashset_

#include"vector.h"
#include<assert.h>
#include<string.h>


struct wordUnit {
    char* word;
    vector *posList;
};

struct pos{
    char* title;
    char* url;
    int count;
};

#endif
