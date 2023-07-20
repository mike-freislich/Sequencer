#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <Arduino.h>
#include <vector>
#include <Ticker.h>
#include "note.h"
#include "pattern.h"

#define BPM_CONTROL_DELAY 150
#define START_BPM 120
#define MAX_BPM 2000
#define MIN_BPM 15

class Sequencer
{
public:
    enum SequencerLoopMode
    {
        LongestPattern,
        PolyRhythmic
    };

private:
    static uint16_t currentBeat;
    static uint16_t sequenceLength;
    PatternVector patterns;
    uint16_t bpm = START_BPM;
    uint16_t bpmLastSetValue = 120;

    uint8_t notesPerBeat = 4;
    float secondsPerNote = 60.0 / bpm / notesPerBeat;
    unsigned long lastBeatMillis = 0;
    unsigned long lastManualBPMupdate = 0;
    Ticker beatTimer;
    SequencerLoopMode loopMode;

    IRAM_ATTR static void beatISR() { currentBeat = (currentBeat + 1) % sequenceLength; }

    void setSequenceLength()
    {
        uint16_t numPatterns = patterns.size();
        uint16_t longest = 1;

        switch (loopMode)
        {
        // get the longest pattern as the starting point
        case LongestPattern:
            for (uint16_t i = 0; i < numPatterns; i++)
                longest = max((uint16_t)patterns[i].getPatternLength(), longest);
            sequenceLength = longest;
            break;

        case PolyRhythmic:
            // find how long to make the sequence until all the pattern lengths loop to start at the same time
            uint16_t fakeBeat = 1;
            bool isZero = false;
            while (!isZero)
            {
                uint16_t i = 0;
                do
                {
                    isZero = (fakeBeat % patterns[i].getPatternLength() == 0);
                    i++;
                } while (isZero && i < numPatterns);

                fakeBeat++;
            }
            sequenceLength = fakeBeat - 1;
            break;
        }
    }

public:
    Sequencer(SequencerLoopMode loopMode = LongestPattern)
    {
        patterns.reserve(8);
        this->loopMode = loopMode;
    }

    void addPattern(Pattern pattern)
    {
        patterns.push_back(pattern);
        setSequenceLength();
    }

    uint8_t getPatternCount() { return patterns.size(); }
    uint16_t getSequenceLength() { return sequenceLength; }

    void setBpm(uint16_t bpm, uint8_t notesPerBeat = 4)
    {
        this->bpm = bpm;
        lastManualBPMupdate = millis();
    }

    void adjustBpm(int8_t amount)
    {
        uint16_t newBpm = bpm + amount;
        if (amount < 0)
        {
            setBpm(max(newBpm, (uint16_t)MIN_BPM));
        }
        else if (amount > 0)
        {
            setBpm(min(newBpm, (uint16_t)MAX_BPM));
        }
    }

    void updateBpmTicker()
    {
        unsigned long currentTime = millis();
        if ((bpm != bpmLastSetValue) && (currentTime - lastManualBPMupdate > BPM_CONTROL_DELAY))
        {
            secondsPerNote = 60.0 / bpm / notesPerBeat;
            lastManualBPMupdate = currentTime;
            bpmLastSetValue = bpm;
            if (beatTimer.active())
                beatTimer.attach(secondsPerNote, beatISR);
        }
    }

    void play() { beatTimer.attach(secondsPerNote, beatISR); }
    void pause() { beatTimer.detach(); }
    uint16_t getCurrentBeat() { return currentBeat; }

    Note getNoteAtBeat(uint8_t patternIndex, uint16_t beat)
    {
        Pattern p = patterns[patternIndex];
        return p.getNoteAt(beat % p.getPatternLength());        
    }    
};

uint16_t Sequencer::currentBeat = 0;
uint16_t Sequencer::sequenceLength = 0;

#endif