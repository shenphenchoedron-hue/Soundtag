#pragma once

#include <juce_core/juce_core.h>

/** Metadata for one audio file (the fields Soundtag can edit). */
struct TagData
{
    juce::String title, artist, album, genre, comment;
    int year  = 0;
    int track = 0;

    juce::MemoryBlock cover;   // raw image bytes (JPEG/PNG), empty = no cover
    juce::String coverMime;

    bool operator== (const TagData& o) const
    {
        return title == o.title && artist == o.artist && album == o.album
            && genre == o.genre && comment == o.comment
            && year == o.year && track == o.track
            && cover == o.cover;
    }
    bool operator!= (const TagData& o) const { return ! operator== (o); }
};

namespace TagIO
{
    /** File extensions Soundtag lists (lower case, with dot). */
    const juce::StringArray& supportedExtensions();
    bool isSupported (const juce::File&);

    /** Reads tags. Returns false and fills 'error' on failure. */
    bool read  (const juce::File&, TagData& out, juce::String& error);

    /** Writes tags (and cover). Returns false and fills 'error' on failure. */
    bool write (const juce::File&, const TagData& data, juce::String& error);
}
