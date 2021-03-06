//
// Created by Behzad Haki on 2022-02-11.
//

#include "settings.h"
#include "queue62.hpp"

// can be used in processor to pass the messages received in a MidiBuffer as is,
// sequentially in a queue to be shared with other threads
void place_midi_message_in_queue(
    juce::MidiBuffer& midiMessages,
    spsc_queue<juce::MidiMessage, settings::midi_queue_size>* midi_message_que)
{
    for (auto m: midiMessages)
    {
        auto message = m.getMessage();
        midi_message_que->push(message);
    }
}

void place_playhead_info_in_queue(
    juce::AudioPlayHead* playheadP,
    spsc_queue<juce::AudioPlayHead::CurrentPositionInfo, settings::playhead_queue_size>* playhead_position_queP)
{
    if (playheadP)
    {
        juce::AudioPlayHead::CurrentPositionInfo position;
        if (playheadP->getCurrentPosition (position))
        {
            playhead_position_queP->push(position);
        }
    }
}

void place_note_in_queue(
    juce::MidiBuffer& midiMessages,
    juce::AudioPlayHead* playheadP,
    spsc_queue<Note, settings::note_queue_size>* note_que)
{
    double frameStartPpq = 0;
    double qpm = 0;

    if (playheadP)
    {
        juce::AudioPlayHead::CurrentPositionInfo position;
        if (playheadP->getCurrentPosition (position))
        {
            if (position.isPlaying){
                // https://forum.juce.com/t/messagemanagerlock-and-thread-shutdown/353/4
                // read from midi_message_que
                frameStartPpq = position.ppqPosition;
                qpm = position.bpm;
            }
        }
    }

    for (auto m: midiMessages)
    {
        auto message = m.getMessage();
        if (message.isNoteOn())
        {
            Note note(message.getNoteNumber(), frameStartPpq, message.getTimeStamp(), qpm);
            note_que->push(note);
        }
    }
}


void place_midiMessagePlayhead_in_queue(
    juce::MidiBuffer& midiMessages,
    juce::AudioPlayHead* playheadP,
    spsc_queue<MidiMsgPlayHead, settings::midi_queue_size>* midiMsgPlayhead_que)
{
    for (auto m: midiMessages)
    {
        if (playheadP)
        {
            juce::AudioPlayHead::CurrentPositionInfo position;
            if (playheadP->getCurrentPosition (position))
            {
                auto message = m.getMessage();
                if (message.isNoteOn())
                {
                    MidiMsgPlayHead MidiMsgPlayhead(message, position);
                    midiMsgPlayhead_que->push(MidiMsgPlayhead);
                }
            }
        }

    }
}