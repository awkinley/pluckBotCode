typedef struct{
    int midiNote;
    unsigned int length_ms;
} Note;


typedef struct{
    const char* name;
    Note* notes;
    uint16_t numNotes;
} Song;


//#include "sandstorm2.h"
#include "sevenNationArmy.h"
#include "axelF.h"
#include "smashmouth.h"
#include "mii.h"
