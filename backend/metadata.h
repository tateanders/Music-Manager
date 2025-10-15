#ifndef __EDITMETADATA_H
#define __EDITMETADATA_H

#include "ID3v2dot3/readID3v2dot3.h"
#include "ID3v2dot3/editID3v2dot3.h"
#include "ID3v2dot4/readID3v2dot4.h"
#include "ID3v2dot4/editID3v2dot4.h"
#include "data.h"

int getSongData(struct dirent* entry, struct song* song, char* comment);

#endif