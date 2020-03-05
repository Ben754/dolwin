// Plugin API for emulator
#include "DVD.h"

DVD dvd;

// ---------------------------------------------------------------------------
// select current dvd 

long DVDSetCurrent(char *file)
{
    // close previously selected file
    GCMClose();

    // try to open file
    FILE *f = fopen(file, "rb");
    if(!f) return FALSE;
    fclose(f);

    // select current DVD
    BOOL res = GCMSelectFile(file);

    // init filesystem
    if(res)
    {
        res = dvd_fs_init();
    }

    if (!res)
        return res;

    dvd.selected = true;
    return res;
}

// ---------------------------------------------------------------------------
// dvd operations on current dvd

void DVDSeek(int position)
{
    // DVD is not selected
    if (!dvd.selected) return;

    GCMSeek(position);
}

void DVDRead(void *buffer, int length)
{
    // DVD is not selected
    if(!dvd.selected) return;

    GCMRead((uint8_t *)buffer, length);
}

long DVDOpenFile(char *dvdfile)
{
    // DVD is not selected
    if(!dvd.selected) return 0;

    // call DVD filesystem open
    return dvd_open(dvdfile);
}
