// Round-trip test for TagIO: writes tags + cover into copies of the given files and reads them back.
// usage: SoundtagTests <audio files...>
#include "../Source/TagIO.h"

#include <iostream>

int main (int argc, char** argv)
{
    int failures = 0;

    // Tiny valid PNG (1x1)
    const unsigned char png[] = { 0x89,'P','N','G',0x0d,0x0a,0x1a,0x0a,0,0,0,0x0d,'I','H','D','R',0,0,0,1,0,0,0,1,8,6,0,0,0,0x1f,0x15,0xc4,0x89,
                                  0,0,0,0x0d,'I','D','A','T',0x78,0x9c,0x63,0xf8,0xcf,0xc0,0xf0,0x1f,0,0x05,0x00,0x01,0xff,0x5b,0x0e,0x7d,0x83,
                                  0,0,0,0,'I','E','N','D',0xae,0x42,0x60,0x82 };

    for (int i = 1; i < argc; ++i)
    {
        juce::File src (juce::String::fromUTF8 (argv[i]));
        auto tmp = juce::File::getSpecialLocation (juce::File::tempDirectory)
                       .getChildFile ("soundtag-test-" + src.getFileName());
        src.copyFileTo (tmp);

        TagData in;
        in.title   = juce::String::fromUTF8 ("Morgenlys æøå");
        in.artist  = "Lone Hansen";
        in.album   = juce::String::fromUTF8 ("Små fortællinger");
        in.genre   = "Instrumental";
        in.comment = "Klaver og kontrabas";
        in.year    = 2026;
        in.track   = 2;
        in.cover.replaceAll (png, sizeof (png));
        in.coverMime = "image/png";

        juce::String err;
        TagData out;
        bool ok = TagIO::write (tmp, in, err) && TagIO::read (tmp, out, err) && out == in;

        // Remove cover again
        TagData noCover = in; noCover.cover.reset();
        TagData out2;
        ok = ok && TagIO::write (tmp, noCover, err) && TagIO::read (tmp, out2, err) && out2.cover.isEmpty() && out2.title == in.title;

        std::cout << (ok ? "OK   " : "FAIL ") << src.getFileName() << " " << err << " [" << out.title << " | " << out.album
                  << " | " << out.year << " | cover " << (int) out.cover.getSize() << "]" << std::endl;
        failures += ok ? 0 : 1;
        tmp.deleteFile();
    }

    return failures == 0 ? 0 : 1;
}
