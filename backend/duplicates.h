#ifndef __DUPLICATES_H
#define __DUPLICATES_H
#include "data.h"

struct duplicate {
    char* title;
    unsigned long hash;
    struct dynarray* locations;
};

void freeDups(struct dynarray* dups);
struct dynarray* findDuplicates(struct directory* directory);

#endif