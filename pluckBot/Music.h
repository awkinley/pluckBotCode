typedef struct{
    int midiNote;
    unsigned int length_ms;
} Note;


typedef struct{
    const char* name;
    Note* notes;
    uint16_t numNotes;
} Song;


#include "sevenNationArmy.h"
#include "marryLamb.h"