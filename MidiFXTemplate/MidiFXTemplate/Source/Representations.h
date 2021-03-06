//
// Created by Behzad Haki on 2022-02-11.
//
#pragma once

#include <utility>

#include "PluginProcessor.h"
#include "settings.h"

// the onset of midi message has two attributes: (1) the ppq of the beginning of the frame
// (2) the number of AUDIO samples from the beginning of the frame
// hence we need to use the sample rate and the qpm from the daw to calculate
// absolute ppq of the onset. Look into NppqPosB structure
struct onset_time{
    // exact position of onset with respect to ppq
    double ppq;

    // default constructor for empty instantiation
    onset_time(){}

    // constructor for desired timing of event in terms of absolute ppq
    onset_time(double ppq): ppq(ppq) {}

    // constructor for received timing of midi messages
    // Automatically calculates absolute timing of event in terms of ppq
    onset_time(double frameStartPpq, double audioSamplePos, double qpm):ppq()
    {
        ppq = calculate_absolute_ppq(frameStartPpq, audioSamplePos, qpm); // calculate ppq
    }

    // used usually for finding absolute timing of messages received in processor MIDIBuffer
    static double calculate_absolute_ppq(double frameStartPpq, double audioSamplePos, double qpm)
    {
        auto tmp_ppq = frameStartPpq + audioSamplePos * qpm / (60 * settings::sample_rate);
        return tmp_ppq;
    }

    // used usually for preparing for playback through the processor MIDI buffer
    juce::int64 calculate_num_samples_from_frame_ppq(double frameStartPpq, double qpm){
        auto tmp_audioSamplePos = (ppq - frameStartPpq) * 60.0 * settings::sample_rate / qpm;
        return juce::int64(tmp_audioSamplePos);
    }

};

// A note structure holding the note number for an onset along with
// ppq position -->  defined as the ration of quarter note
// onset_time (struct see above) --> structure holding absolute ppq with implemented
// utilities for quick conversions for buffer to processsor OR processor to buffer  cases
struct Note{
    int note;
    onset_time time;
    // default constructor for empty instantiation
    Note(){}

    // constructor for placing notes received in the processor from the MIDIBuffer
    Note(int note_number, double frameStartPpq, double audioSamplePos, double qpm):
        note(note_number), time(frameStartPpq, audioSamplePos, qpm){
    }
};

// todo the timings in the struct seem to not work

// structure holding MidiMessage and Playhead info together
struct MidiMsgPlayHead{
    juce::MidiMessage MidiMessage;
    juce::AudioPlayHead::CurrentPositionInfo playheadInfo;

    MidiMsgPlayHead(){}

    // constructor for placing midi message and playhead info in one structure
    MidiMsgPlayHead(juce::MidiMessage m, juce::AudioPlayHead::CurrentPositionInfo pinfo):
        MidiMessage(std::move(m)), playheadInfo(pinfo){}
};