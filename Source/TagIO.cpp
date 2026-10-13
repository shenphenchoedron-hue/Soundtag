#include "TagIO.h"

#include <fileref.h>
#include <tag.h>
#include <tstring.h>
#include <tvariant.h>

namespace
{
    TagLib::String toTL (const juce::String& s)
    {
        return TagLib::String (s.toRawUTF8(), TagLib::String::UTF8);
    }

    juce::String fromTL (const TagLib::String& s)
    {
        return juce::String::fromUTF8 (s.toCString (true)).trim();
    }

    TagLib::FileRef openRef (const juce::File& f)
    {
       #if JUCE_WINDOWS
        return TagLib::FileRef (f.getFullPathName().toWideCharPointer(), true, TagLib::AudioProperties::Fast);
       #else
        return TagLib::FileRef (f.getFullPathName().toRawUTF8(), true, TagLib::AudioProperties::Fast);
       #endif
    }

    juce::String sniffMime (const void* data, size_t size)
    {
        auto* b = static_cast<const unsigned char*> (data);
        if (size >= 8 && b[0] == 0x89 && b[1] == 'P' && b[2] == 'N' && b[3] == 'G') return "image/png";
        if (size >= 3 && b[0] == 0xff && b[1] == 0xd8)                               return "image/jpeg";
        if (size >= 6 && b[0] == 'G' && b[1] == 'I' && b[2] == 'F')                   return "image/gif";
        return "image/jpeg";
    }
}

const juce::StringArray& TagIO::supportedExtensions()
{
    static const juce::StringArray exts { ".mp3", ".flac", ".wav", ".aif", ".aiff", ".m4a", ".mp4",
                                          ".aac", ".ogg", ".oga", ".opus", ".wma", ".ape", ".wv", ".mpc", ".dsf" };
    return exts;
}

bool TagIO::isSupported (const juce::File& f)
{
    return supportedExtensions().contains (f.getFileExtension().toLowerCase());
}

bool TagIO::read (const juce::File& file, TagData& out, juce::String& error)
{
    out = {};
    auto ref = openRef (file);

    if (ref.isNull() || ref.tag() == nullptr)
    {
        error = juce::String::fromUTF8 ("Filen kunne ikke l\xc3\xa6ses: ") + file.getFileName();
        return false;
    }

    auto* t = ref.tag();
    out.title   = fromTL (t->title());
    out.artist  = fromTL (t->artist());
    out.album   = fromTL (t->album());
    out.genre   = fromTL (t->genre());
    out.comment = fromTL (t->comment());
    out.year    = (int) t->year();
    out.track   = (int) t->track();

    const auto pictures = ref.complexProperties ("PICTURE");
    if (! pictures.isEmpty())
    {
        // Prefer the front cover if there are several pictures.
        auto chosen = pictures.front();
        for (const auto& p : pictures)
            if (p.contains ("pictureType") && p["pictureType"].toString() == "Front Cover")
                { chosen = p; break; }

        const auto bytes = chosen["data"].toByteVector();
        if (! bytes.isEmpty())
        {
            out.cover.replaceAll (bytes.data(), bytes.size());
            out.coverMime = fromTL (chosen["mimeType"].toString());
            if (out.coverMime.isEmpty())
                out.coverMime = sniffMime (bytes.data(), bytes.size());
        }
    }

    return true;
}

bool TagIO::write (const juce::File& file, const TagData& d, juce::String& error)
{
    if (! file.hasWriteAccess())
    {
        error = juce::String::fromUTF8 ("Filen er skrivebeskyttet: ") + file.getFileName();
        return false;
    }

    auto ref = openRef (file);

    if (ref.isNull() || ref.tag() == nullptr)
    {
        error = juce::String::fromUTF8 ("Filen kunne ikke \xc3\xa5" "bnes: ") + file.getFileName();
        return false;
    }

    auto* t = ref.tag();
    t->setTitle   (toTL (d.title));
    t->setArtist  (toTL (d.artist));
    t->setAlbum   (toTL (d.album));
    t->setGenre   (toTL (d.genre));
    t->setComment (toTL (d.comment));
    t->setYear    ((unsigned int) juce::jmax (0, d.year));
    t->setTrack   ((unsigned int) juce::jmax (0, d.track));

    TagLib::List<TagLib::VariantMap> pictures;
    if (! d.cover.isEmpty())
    {
        TagLib::VariantMap pic;
        pic["data"]        = TagLib::ByteVector (static_cast<const char*> (d.cover.getData()), (unsigned int) d.cover.getSize());
        pic["mimeType"]    = toTL (d.coverMime.isNotEmpty() ? d.coverMime : sniffMime (d.cover.getData(), d.cover.getSize()));
        pic["description"] = TagLib::String ("Cover");
        pic["pictureType"] = TagLib::String ("Front Cover");
        pictures.append (pic);
    }

    const bool hadPictures = ! ref.complexProperties ("PICTURE").isEmpty();
    if (! pictures.isEmpty() || hadPictures)
        ref.setComplexProperties ("PICTURE", pictures);

    if (! ref.save())
    {
        error = juce::String::fromUTF8 ("Kunne ikke gemme: ") + file.getFileName();
        return false;
    }

    return true;
}
