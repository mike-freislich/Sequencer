#ifndef NM_NOTE_H
#define NM_NOTE_H

#include <Arduino.h>
#include <vector>

using namespace std;

struct Note
{
private:
    uint8_t data;

public:
    Note(){};
    Note(uint8_t data) : data(data){};

    void setPitch(uint8_t data)
    {
        this->data = data;
    }

    void setGate()
    {
    }

    uint8_t getData() { return data; }
};
typedef vector<Note> NoteVector;

#endif