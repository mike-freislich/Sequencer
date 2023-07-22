#ifndef NM_PATTERN_H
#define NM_PATTERN_H

#include <Arduino.h>
#include <vector>
#include "note.h"

class Pattern
{
private:
    NoteVector notes;

public:
    Pattern() {}
    Pattern(NoteVector notes) { this->notes = notes; }
    void setPatternLength(size_t length) { notes.resize(length); }
    size_t getPatternLength() { return notes.size(); }
    NoteVector getNotes(uint16_t index, uint16_t length) {return notes; }
    Note getNoteAt(int8_t position) { return notes[position]; }
};
typedef vector<Pattern> PatternVector;

#endif