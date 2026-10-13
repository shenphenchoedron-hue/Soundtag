#include "MainComponent.h"

#include <algorithm>

//==============================================================================
namespace Palette
{
    const juce::Colour background   { 0xffeef2f8 };
    const juce::Colour card         { 0xfff7f9fd };
    const juce::Colour line         { 0xffdce1ea };
    const juce::Colour field        { 0xffffffff };
    const juce::Colour fieldLine    { 0xffcfd6e2 };
    const juce::Colour text         { 0xff141a26 };
    const juce::Colour textSoft     { 0xff3c4454 };
    const juce::Colour textMuted    { 0xff6b7383 };
    const juce::Colour blue         { 0xff1a66f5 };
    const juce::Colour blueHover    { 0xff0f56dc };
    const juce::Colour orange       { 0xffff9a1f };
    const juce::Colour rowWhite     { 0xffffffff };
    const juce::Colour rowBlue      { 0xffe8eefb };
    const juce::Colour rowSelected  { 0xfffdecc2 };
    const juce::Colour grey         { 0xffc3c8d0 };
    const juce::Colour folderGrey   { 0xff8e96a3 };
    const juce::Colour icon         { 0xff1f2430 };
    const juce::Colour secondary    { 0xfff3f5f9 };
    const juce::Colour coverButton  { 0xffe6ebf3 };
}

static juce::Font font (float size, bool bold = false)
{
    return juce::Font (juce::FontOptions (size, bold ? juce::Font::bold : juce::Font::plain));
}

/** UTF-8 literal -> juce::String (sources are UTF-8). */
static juce::String U (const char* s) { return juce::String::fromUTF8 (s); }

//==============================================================================
namespace Icons
{
    using R = juce::Rectangle<float>;

    static void logo (juce::Graphics& g, R r)
    {
        auto s = juce::jmin (r.getWidth(), r.getHeight());
        r = r.withSizeKeepingCentre (s, s);

        // Price tag pointing to the lower left
        juce::Path tag;
        tag.startNewSubPath (0.00f, 0.50f);
        tag.lineTo (0.34f, 0.12f);
        tag.lineTo (1.00f, 0.12f);
        tag.lineTo (1.00f, 0.88f);
        tag.lineTo (0.34f, 0.88f);
        tag.closeSubPath();
        tag = tag.createPathWithRoundedCorners (0.12f);
        tag.addEllipse (0.24f, 0.42f, 0.16f, 0.16f);
        tag.setUsingNonZeroWinding (false);
        tag.applyTransform (juce::AffineTransform::translation (-0.5f, -0.5f)
                              .rotated (-juce::MathConstants<float>::pi * 0.25f)
                              .scaled (s * 0.74f)
                              .translated (r.getX() + s * 0.44f, r.getY() + s * 0.58f));
        g.setColour (Palette::blue);
        g.fillPath (tag);

        // Note
        auto px = [&] (float fx) { return r.getX() + fx * s; };
        auto py = [&] (float fy) { return r.getY() + fy * s; };
        g.setColour (Palette::orange);
        g.fillEllipse (px (0.50f), py (0.26f), s * 0.20f, s * 0.16f);
        g.fillRect (px (0.635f), py (0.00f), s * 0.065f, s * 0.34f);
        juce::Path flag;
        flag.startNewSubPath (px (0.635f), py (0.00f));
        flag.lineTo (px (0.86f), py (0.10f));
        flag.lineTo (px (0.86f), py (0.21f));
        flag.lineTo (px (0.70f), py (0.14f));
        flag.closeSubPath();
        g.fillPath (flag);
    }

    static void folder (juce::Graphics& g, R r, juce::Colour c)
    {
        juce::Path p;
        auto w = r.getWidth(), h = r.getHeight();
        p.startNewSubPath (r.getX(), r.getY() + h * 0.08f);
        p.lineTo (r.getX() + w * 0.38f, r.getY() + h * 0.08f);
        p.lineTo (r.getX() + w * 0.50f, r.getY() + h * 0.24f);
        p.lineTo (r.getRight(), r.getY() + h * 0.24f);
        p.lineTo (r.getRight(), r.getBottom());
        p.lineTo (r.getX(), r.getBottom());
        p.closeSubPath();
        g.setColour (c);
        g.fillPath (p.createPathWithRoundedCorners (w * 0.12f));
    }

    static void disk (juce::Graphics& g, R r, juce::Colour c, juce::Colour bg)
    {
        g.setColour (c);
        juce::Path body;
        body.startNewSubPath (r.getX(), r.getY());
        body.lineTo (r.getRight() - r.getWidth() * 0.22f, r.getY());
        body.lineTo (r.getRight(), r.getY() + r.getHeight() * 0.22f);
        body.lineTo (r.getRight(), r.getBottom());
        body.lineTo (r.getX(), r.getBottom());
        body.closeSubPath();
        g.fillPath (body.createPathWithRoundedCorners (r.getWidth() * 0.12f));

        g.setColour (bg);
        auto w = r.getWidth(), h = r.getHeight();
        g.fillRoundedRectangle (r.getX() + w * 0.22f, r.getY() + h * 0.10f, w * 0.46f, h * 0.24f, w * 0.04f);
        g.fillEllipse (r.getCentreX() - w * 0.15f, r.getY() + h * 0.50f, w * 0.30f, w * 0.30f);
    }

    static void note (juce::Graphics& g, R r, juce::Colour c)
    {
        auto s = juce::jmin (r.getWidth(), r.getHeight());
        r = r.withSizeKeepingCentre (s, s);
        g.setColour (c);
        g.fillEllipse (r.getX() + s * 0.10f, r.getY() + s * 0.66f, s * 0.30f, s * 0.24f);
        g.fillEllipse (r.getX() + s * 0.60f, r.getY() + s * 0.56f, s * 0.30f, s * 0.24f);
        g.fillRect (r.getX() + s * 0.33f, r.getY() + s * 0.14f, s * 0.07f, s * 0.64f);
        g.fillRect (r.getX() + s * 0.83f, r.getY() + s * 0.04f, s * 0.07f, s * 0.64f);
        juce::Path beam;
        beam.startNewSubPath (r.getX() + s * 0.33f, r.getY() + s * 0.14f);
        beam.lineTo (r.getX() + s * 0.90f, r.getY() + s * 0.04f);
        beam.lineTo (r.getX() + s * 0.90f, r.getY() + s * 0.18f);
        beam.lineTo (r.getX() + s * 0.33f, r.getY() + s * 0.28f);
        beam.closeSubPath();
        g.fillPath (beam);
    }
}

//==============================================================================
SoundtagLookAndFeel::SoundtagLookAndFeel()
    : LookAndFeel_V4 (getLightColourScheme())
{
    setColour (juce::ResizableWindow::backgroundColourId, Palette::background);
    setColour (juce::TextEditor::textColourId,            Palette::text);
    setColour (juce::TextEditor::backgroundColourId,      Palette::field);
    setColour (juce::TextEditor::highlightColourId,       Palette::blue.withAlpha (0.22f));
    setColour (juce::TextEditor::highlightedTextColourId, Palette::text);
    setColour (juce::TextEditor::outlineColourId,         Palette::fieldLine);
    setColour (juce::TextEditor::focusedOutlineColourId,  Palette::blue);
    setColour (juce::CaretComponent::caretColourId,       Palette::blue);
    setColour (juce::ComboBox::textColourId,              Palette::text);
    setColour (juce::ComboBox::backgroundColourId,        Palette::field);
    setColour (juce::ComboBox::outlineColourId,           Palette::fieldLine);
    setColour (juce::ComboBox::arrowColourId,             Palette::textSoft);
    setColour (juce::Label::textWhenEditingColourId,      Palette::text);
    setColour (juce::PopupMenu::backgroundColourId,       juce::Colours::white);
    setColour (juce::PopupMenu::textColourId,             Palette::text);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, Palette::rowBlue);
    setColour (juce::PopupMenu::highlightedTextColourId,  Palette::text);
    setColour (juce::ScrollBar::thumbColourId,            Palette::grey);
    setColour (juce::TooltipWindow::backgroundColourId,   Palette::text);
    setColour (juce::TooltipWindow::textColourId,         juce::Colours::white);
    setColour (juce::TooltipWindow::outlineColourId,      juce::Colours::transparentBlack);
    setColour (juce::AlertWindow::backgroundColourId,     juce::Colours::white);
    setColour (juce::AlertWindow::textColourId,           Palette::text);
    setColour (juce::TextButton::buttonColourId,          Palette::secondary);
    setColour (juce::TextButton::textColourOffId,         Palette::text);
}

void SoundtagLookAndFeel::fillTextEditorBackground (juce::Graphics& g, int w, int h, juce::TextEditor& ed)
{
    g.setColour (ed.isEnabled() ? Palette::field : Palette::secondary);
    g.fillRoundedRectangle (juce::Rectangle<float> (0, 0, (float) w, (float) h).reduced (0.5f), 7.0f);
}

void SoundtagLookAndFeel::drawTextEditorOutline (juce::Graphics& g, int w, int h, juce::TextEditor& ed)
{
    auto focused = ed.hasKeyboardFocus (true) && ! ed.isReadOnly() && ed.isEnabled();
    g.setColour (focused ? Palette::blue : Palette::fieldLine);
    g.drawRoundedRectangle (juce::Rectangle<float> (0, 0, (float) w, (float) h).reduced (focused ? 1.0f : 0.5f),
                            7.0f, focused ? 2.0f : 1.0f);
}

void SoundtagLookAndFeel::drawComboBox (juce::Graphics& g, int w, int h, bool,
                                        int, int, int, int, juce::ComboBox& box)
{
    auto r = juce::Rectangle<float> (0, 0, (float) w, (float) h).reduced (0.5f);
    g.setColour (box.isEnabled() ? Palette::field : Palette::secondary);
    g.fillRoundedRectangle (r, 7.0f);

    auto focused = box.hasKeyboardFocus (true) && box.isEnabled();
    g.setColour (focused ? Palette::blue : Palette::fieldLine);
    g.drawRoundedRectangle (r.reduced (focused ? 0.5f : 0.0f), 7.0f, focused ? 2.0f : 1.0f);

    auto a = juce::Rectangle<float> ((float) w - 28.0f, (float) h * 0.5f - 3.0f, 11.0f, 6.0f);
    juce::Path chevron;
    chevron.startNewSubPath (a.getX(), a.getY());
    chevron.lineTo (a.getCentreX(), a.getBottom());
    chevron.lineTo (a.getRight(), a.getY());
    g.setColour (Palette::textSoft.withAlpha (box.isEnabled() ? 1.0f : 0.4f));
    g.strokePath (chevron, juce::PathStrokeType (1.7f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void SoundtagLookAndFeel::positionComboBoxText (juce::ComboBox& box, juce::Label& label)
{
    label.setBounds (5, 1, box.getWidth() - 40, box.getHeight() - 2);
    label.setFont (getComboBoxFont (box));
}

juce::Font SoundtagLookAndFeel::getComboBoxFont (juce::ComboBox&) { return font (14.5f); }
juce::Font SoundtagLookAndFeel::getPopupMenuFont()                { return font (14.5f); }

void SoundtagLookAndFeel::drawScrollbar (juce::Graphics& g, juce::ScrollBar& bar, int x, int y, int w, int h,
                                         bool isVertical, int thumbStart, int thumbSize, bool over, bool down)
{
    juce::ignoreUnused (bar);
    juce::Rectangle<float> thumb = isVertical
        ? juce::Rectangle<float> ((float) x + 2.0f, (float) thumbStart, (float) w - 4.0f, (float) thumbSize)
        : juce::Rectangle<float> ((float) thumbStart, (float) y + 2.0f, (float) thumbSize, (float) h - 4.0f);
    g.setColour (Palette::grey.withAlpha (down ? 1.0f : over ? 0.9f : 0.7f));
    g.fillRoundedRectangle (thumb, juce::jmin (thumb.getWidth(), thumb.getHeight()) * 0.5f);
}

//==============================================================================
void CoverView::paint (juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat();
    juce::Path clip;
    clip.addRoundedRectangle (r, 9.0f);

    if (image.isValid())
    {
        g.saveState();
        g.reduceClipRegion (clip);
        // Fill the square, cropping the longer side.
        auto placement = juce::RectanglePlacement (juce::RectanglePlacement::centred | juce::RectanglePlacement::fillDestination);
        g.setImageResamplingQuality (juce::Graphics::highResamplingQuality);
        g.drawImage (image, r, placement);
        g.restoreState();
        g.setColour (juce::Colours::black.withAlpha (0.08f));
        g.drawRoundedRectangle (r.reduced (0.5f), 9.0f, 1.0f);
    }
    else
    {
        g.setColour (isEnabled() ? Palette::rowBlue : Palette::secondary);
        g.fillPath (clip);
        g.setColour (Palette::fieldLine);
        g.drawRoundedRectangle (r.reduced (0.5f), 9.0f, 1.0f);

        auto c = r.reduced (r.getWidth() * 0.3f);
        Icons::note (g, c.withTrimmedBottom (c.getHeight() * 0.2f), Palette::blue.withAlpha (isEnabled() ? 0.45f : 0.2f));
        g.setColour (Palette::textMuted.withAlpha (isEnabled() ? 1.0f : 0.5f));
        g.setFont (font (12.5f));
        g.drawText ("Intet cover", r.removeFromBottom (r.getHeight() * 0.28f), juce::Justification::centred);
    }
}

void CoverView::mouseUp (const juce::MouseEvent& e)
{
    if (! isEnabled() || ! getLocalBounds().contains (e.getPosition()))
        return;

    if (e.mods.isPopupMenu())
    {
        juce::PopupMenu m;
        m.addItem (U ("Skift cover…"), [this] { if (onClick) onClick(); });
        m.addItem ("Fjern cover", image.isValid(), false, [this] { if (onRemove) onRemove(); });
        m.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (this));
    }
    else if (onClick)
    {
        onClick();
    }
}

//==============================================================================
namespace
{
    std::unique_ptr<DrawnButton> primaryButton (const juce::String& text, bool saveIcon)
    {
        return std::make_unique<DrawnButton> (text, [text, saveIcon] (juce::Graphics& g, juce::Rectangle<float> r,
                                                                      bool over, bool down, bool enabled)
        {
            auto c = down ? Palette::blueHover.darker (0.1f) : over ? Palette::blueHover : Palette::blue;
            g.setColour (enabled ? c : Palette::blue.withAlpha (0.4f));
            g.fillRoundedRectangle (r, 9.0f);

            auto tf = font (15.0f, true);
            auto textW = juce::GlyphArrangement::getStringWidth (tf, text);
            auto iconW = 18.0f, gap = 10.0f;
            auto content = r.withSizeKeepingCentre (juce::jmin (r.getWidth() - 16.0f, iconW + gap + textW + 2.0f), r.getHeight());
            auto ic = content.removeFromLeft (iconW).withSizeKeepingCentre (iconW, saveIcon ? iconW : iconW * 0.8f);
            if (saveIcon) Icons::disk (g, ic, juce::Colours::white, c);
            else          Icons::folder (g, ic, juce::Colours::white);
            content.removeFromLeft (gap);
            g.setColour (juce::Colours::white);
            g.setFont (tf);
            g.drawText (text, content, juce::Justification::centredLeft, false);
        });
    }

    std::unique_ptr<DrawnButton> secondaryButton (const juce::String& text, bool folderIcon, juce::Colour fill = Palette::secondary)
    {
        return std::make_unique<DrawnButton> (text, [text, folderIcon, fill] (juce::Graphics& g, juce::Rectangle<float> r,
                                                                             bool over, bool down, bool enabled)
        {
            auto bg = down ? fill.darker (0.08f) : over ? fill.darker (0.04f) : fill;
            g.setColour (bg);
            g.fillRoundedRectangle (r.reduced (0.5f), 9.0f);
            g.setColour (Palette::fieldLine);
            g.drawRoundedRectangle (r.reduced (0.5f), 9.0f, 1.0f);

            auto alpha = enabled ? 1.0f : 0.45f;
            auto tf = font (14.5f, folderIcon);
            auto textW = juce::GlyphArrangement::getStringWidth (tf, text);
            auto iconW = folderIcon ? 18.0f : 0.0f, gap = folderIcon ? 10.0f : 0.0f;
            auto content = r.withSizeKeepingCentre (juce::jmin (r.getWidth() - 12.0f, iconW + gap + textW + 2.0f), r.getHeight());
            if (folderIcon)
                Icons::folder (g, content.removeFromLeft (iconW).withSizeKeepingCentre (iconW, iconW * 0.8f), Palette::blue.withAlpha (alpha));
            content.removeFromLeft (gap);
            g.setColour (Palette::text.withAlpha (alpha));
            g.setFont (tf);
            g.drawText (text, content, juce::Justification::centredLeft, false);
        });
    }
}

//==============================================================================
MainComponent::MainComponent()
{
    setLookAndFeel (&lnf);
    juce::LookAndFeel::setDefaultLookAndFeel (&lnf);
    setWantsKeyboardFocus (true);

    juce::PropertiesFile::Options opts;
    opts.applicationName     = "Soundtag";
    opts.filenameSuffix      = ".settings";
    opts.folderName          = "Soundtag";
    opts.osxLibrarySubFolder = "Application Support";
    settings = std::make_unique<juce::PropertiesFile> (opts);
    lastFolder = juce::File (settings->getValue ("lastFolder"));

   #if JUCE_MAC
    headerHeight = 0;   // native title bar
   #else
    headerHeight = 50;
    auto windowButton = [] (const juce::String& name, int kind)
    {
        return std::make_unique<DrawnButton> (name, [kind] (juce::Graphics& g, juce::Rectangle<float> r, bool over, bool down, bool)
        {
            if (over || down)
            {
                g.setColour (kind == 2 ? juce::Colour (0xffe5484d) : juce::Colours::black.withAlpha (down ? 0.10f : 0.06f));
                g.fillRoundedRectangle (r.reduced (3.0f, 9.0f), 6.0f);
            }

            auto c = (kind == 2 && over) ? juce::Colours::white : Palette::icon;
            auto b = r.withSizeKeepingCentre (13.0f, 13.0f);
            g.setColour (c);
            if (kind == 0)       g.drawLine (b.getX(), b.getCentreY(), b.getRight(), b.getCentreY(), 1.6f);
            else if (kind == 1)  g.drawRoundedRectangle (b.reduced (1.0f), 1.5f, 1.5f);
            else
            {
                g.drawLine (b.getX(), b.getY(), b.getRight(), b.getBottom(), 1.6f);
                g.drawLine (b.getRight(), b.getY(), b.getX(), b.getBottom(), 1.6f);
            }
        });
    };

    minimiseButton = windowButton ("Minimer", 0);
    maximiseButton = windowButton ("Maksimer", 1);
    closeButton    = windowButton ("Luk", 2);

    minimiseButton->onClick = [this] { if (auto* w = dynamic_cast<juce::DocumentWindow*> (window())) w->setMinimised (true); };
    maximiseButton->onClick = [this] { if (auto* w = dynamic_cast<juce::DocumentWindow*> (window())) w->setFullScreen (! w->isFullScreen()); };
    closeButton->onClick    = []     { juce::JUCEApplication::getInstance()->systemRequestedQuit(); };

    for (auto* b : { minimiseButton.get(), maximiseButton.get(), closeButton.get() })
        addAndMakeVisible (b);
   #endif

    // Toolbar ----------------------------------------------------------------
    openFolderButton = primaryButton (U ("Åbn mappe"), false);
    openFolderButton->onClick = [this] { confirmDiscard ([this] { chooseFolder(); }); };
    openFolderButton->setTooltip (U ("Åbn en mappe med lydfiler (Ctrl/Cmd+O)"));
    addAndMakeVisible (*openFolderButton);

    openFilesButton = secondaryButton (U ("Åbn filer"), true, juce::Colours::white.withAlpha (0.6f));
    openFilesButton->onClick = [this] { confirmDiscard ([this] { chooseFiles(); }); };
    openFilesButton->setTooltip (U ("Vælg en eller flere lydfiler"));
    addAndMakeVisible (*openFilesButton);

    // File list --------------------------------------------------------------
    listBox.setRowHeight (40);
    listBox.setOutlineThickness (0);
    listBox.setColour (juce::ListBox::backgroundColourId, juce::Colours::transparentBlack);
    listBox.getViewport()->setScrollBarThickness (8);
    addAndMakeVisible (listBox);

    // Metadata fields -------------------------------------------------------
    for (auto* ed : { &titleEdit, &artistEdit, &albumEdit, &yearEdit, &trackEdit, &commentEdit })
    {
        ed->setFont (font (14.5f));
        ed->setIndents (10, 4);
        ed->setJustification (juce::Justification::centredLeft);
        ed->onTextChange = [this] { if (! loading) updateState(); };
        addAndMakeVisible (*ed);
    }

    yearEdit.setInputRestrictions (4, "0123456789");
    trackEdit.setInputRestrictions (4, "0123456789");

    commentEdit.setMultiLine (true, true);
    commentEdit.setReturnKeyStartsNewLine (true);
    commentEdit.setJustification (juce::Justification::topLeft);
    commentEdit.setIndents (10, 7);
    commentEdit.setScrollbarsShown (true);

    const char* genres[] = { "Acoustic", "Ambient", "Blues", "Children's", "Classical", "Country", "Dance",
                             "Electronic", "Folk", "Hip-Hop", "Instrumental", "Jazz", "Latin", "Meditative",
                             "New Age", "Pop", "R&B", "Rock", "Soundtrack", "Spoken Word", "Vocal", "World" };
    int id = 1;
    for (auto* gname : genres)
        genreBox.addItem (gname, id++);
    genreBox.setEditableText (true);
    genreBox.onChange = [this] { if (! loading) updateState(); };
    addAndMakeVisible (genreBox);

    coverView.onClick  = [this] { chooseCover(); };
    coverView.onRemove = [this] { setCover ({}, {}); };
    coverView.setMouseCursor (juce::MouseCursor::PointingHandCursor);
    coverView.setTooltip (U ("Klik for at skifte cover · højreklik for flere valg"));
    addAndMakeVisible (coverView);

    coverButton = secondaryButton ("Skift cover", false, Palette::coverButton);
    coverButton->onClick = [this] { chooseCover(); };
    addAndMakeVisible (*coverButton);

    // Footer ----------------------------------------------------------------
    revertButton = secondaryButton (U ("Fortryd ændringer"), false, juce::Colours::white);
    revertButton->onClick = [this] { revert(); };
    addAndMakeVisible (*revertButton);

    saveButton = primaryButton (U ("Gem ændringer"), true);
    saveButton->onClick = [this] { save(); };
    saveButton->setTooltip ("Gem metadata i filen (Ctrl/Cmd+S)");
    addAndMakeVisible (*saveButton);

    labels = { { "Titel", {} }, { "Kunstner", {} }, { "Album", {} }, { U ("År"), {} },
               { "Spor", {} }, { "Genre", {} }, { "Kommentar", {} } };

    setSize (720, headerHeight + 530);
    updateState();
}

MainComponent::~MainComponent()
{
    saveSettings();
    juce::LookAndFeel::setDefaultLookAndFeel (nullptr);
    setLookAndFeel (nullptr);
}

void MainComponent::saveSettings()
{
    if (settings != nullptr)
    {
        settings->setValue ("lastFolder", lastFolder.getFullPathName());
        settings->saveIfNeeded();
    }
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (Palette::background);

    // Title bar ------------------------------------------------------------
    if (headerHeight > 0)
    {
        auto h = headerArea().toFloat();
        Icons::logo (g, juce::Rectangle<float> (18.0f, h.getCentreY() - 15.0f, 30.0f, 30.0f));
        g.setColour (Palette::text);
        g.setFont (font (21.0f, true));
        g.drawText ("Soundtag", juce::Rectangle<float> (58.0f, h.getY(), 200.0f, h.getHeight()), juce::Justification::centredLeft);
        g.setColour (Palette::line);
        g.fillRect (h.removeFromBottom (1.0f));
    }

    // Toolbar info: separator + current folder ----------------------------
    {
        auto r = toolbarInfoArea.toFloat();
        g.setColour (Palette::line);
        g.fillRect (r.removeFromLeft (1.0f).withSizeKeepingCentre (1.0f, 26.0f));
        r.removeFromLeft (16.0f);

        // Small decoration, only when there is room
        if (r.getWidth() > 260.0f)
        {
            auto d = r.removeFromRight (62.0f);
            auto cy = d.getCentreY();
            g.setColour (Palette::orange);
            g.fillEllipse (d.getX(), cy - 10.0f, 12.0f, 12.0f);
            g.setColour (Palette::grey);
            g.fillRoundedRectangle (d.getX() + 22.0f, cy + 0.0f, 13.0f, 13.0f, 3.0f);
            juce::Path tri;
            tri.addTriangle (d.getX() + 52.0f, cy - 15.0f, d.getX() + 62.0f, cy + 2.0f, d.getX() + 42.0f, cy - 1.0f);
            g.setColour (Palette::blue);
            g.fillPath (tri.createPathWithRoundedCorners (2.5f));
        }

        Icons::folder (g, r.removeFromLeft (24.0f).withSizeKeepingCentre (24.0f, 19.0f), Palette::folderGrey);
        r.removeFromLeft (12.0f);
        g.setColour (sourceName.isEmpty() ? Palette::textMuted : Palette::text);
        g.setFont (font (15.5f, sourceName.isNotEmpty()));
        g.drawText (sourceName.isEmpty() ? U ("Ingen mappe åbnet") : sourceName, r, juce::Justification::centredLeft, true);
    }

    // Cards ------------------------------------------------------------------
    for (auto c : { listCard, metaCard })
    {
        g.setColour (Palette::card);
        g.fillRoundedRectangle (c.toFloat(), 12.0f);
        g.setColour (juce::Colours::white.withAlpha (0.7f));
        g.drawRoundedRectangle (c.toFloat().reduced (0.5f), 12.0f, 1.0f);
    }

    g.setColour (Palette::text);
    g.setFont (font (19.0f, true));
    g.drawText ("Lydfiler", listCard.reduced (16, 0).withTop (listCard.getY() + 10).withHeight (30), juce::Justification::centredLeft);

    if (files.isEmpty())
    {
        g.setColour (Palette::textMuted);
        g.setFont (font (13.5f));
        g.drawFittedText (U ("Åbn en mappe, vælg filer\neller træk dem herind."),
                          listBox.getBounds().reduced (10).withHeight (60), juce::Justification::centred, 3);
    }

    {
        auto r = metaTitleArea;
        g.setColour (Palette::text);
        g.setFont (font (19.0f, true));
        g.drawText ("Metadata", r.removeFromTop (24), juce::Justification::centredLeft);
        g.setColour (Palette::textMuted);
        g.setFont (font (13.5f));
        auto name = juce::isPositiveAndBelow (currentIndex, files.size()) ? files[currentIndex].getFileName()
                                                                          : U ("Ingen fil valgt");
        g.drawText (name, r, juce::Justification::centredLeft, true);
    }

    g.setFont (font (13.0f));
    for (auto& l : labels)
    {
        g.setColour (Palette::textSoft.withAlpha (titleEdit.isEnabled() ? 1.0f : 0.55f));
        g.drawText (l.text, l.area, juce::Justification::bottomLeft);
    }

    // Footer -----------------------------------------------------------------
    g.setColour (Palette::line);
    g.fillRect (footerArea.withHeight (1));
    g.setColour (Palette::textMuted);
    g.setFont (font (13.5f));

    juce::String status;
    if (files.isEmpty())
        status = "Ingen filer";
    else
        status << files.size() << (files.size() == 1 ? " fil" : " filer") << U (" · ")
               << (currentIndex >= 0 ? "1 valgt" : "0 valgt");
    if (isDirty())
        status << U (" · ikke gemt");
    else if (statusNote.isNotEmpty())
        status << U (" · ") << statusNote;

    g.drawText (status, statusArea, juce::Justification::centredLeft, true);
}

void MainComponent::resized()
{
    auto r = getLocalBounds();
    auto header = r.removeFromTop (headerHeight);

   #if ! JUCE_MAC
    header.removeFromRight (8);
    for (auto* b : { closeButton.get(), maximiseButton.get(), minimiseButton.get() })
        b->setBounds (header.removeFromRight (44));
   #else
    juce::ignoreUnused (header);
   #endif

    r = r.reduced (14, 0);

    // Toolbar
    r.removeFromTop (12);
    auto toolbar = r.removeFromTop (40);
    openFolderButton->setBounds (toolbar.removeFromLeft (148));
    toolbar.removeFromLeft (8);
    openFilesButton->setBounds (toolbar.removeFromLeft (122));
    toolbar.removeFromLeft (14);
    toolbarInfoArea = toolbar;
    r.removeFromTop (12);

    // Footer
    footerArea = r.removeFromBottom (56).expanded (14, 0);
    {
        auto f = footerArea.reduced (14, 0).withTrimmedTop (1);
        auto buttons = f.withSizeKeepingCentre (f.getWidth(), 36);
        saveButton->setBounds (buttons.removeFromRight (168));
        buttons.removeFromRight (10);
        revertButton->setBounds (buttons.removeFromRight (160));
        statusArea = buttons.withTrimmedLeft (4);
    }
    r.removeFromBottom (12);

    // Cards
    listCard = r.removeFromLeft (juce::jlimit (200, 300, (int) (r.getWidth() * 0.37f)));
    r.removeFromLeft (12);
    metaCard = r;

    listBox.setBounds (listCard.reduced (8, 0).withTrimmedTop (46).withTrimmedBottom (8));

    // Metadata
    auto m = metaCard.reduced (16, 12);
    metaTitleArea = m.removeFromTop (42);
    m.removeFromTop (2);

    const int labelH = 17, fieldH = 28, gap = 4;
    int li = 0;
    auto field = [&] (juce::Rectangle<int>& area, juce::Component& c, int h = -1)
    {
        if (h < 0) h = fieldH;
        labels[(size_t) li++].area = area.removeFromTop (labelH).withTrimmedLeft (1);
        area.removeFromTop (2);
        c.setBounds (area.removeFromTop (h));
        area.removeFromTop (gap);
    };

    const int coverSize = 3 * (labelH + 2 + fieldH + gap) - 36 - gap;
    auto top = m.removeFromTop (3 * (labelH + 2 + fieldH + gap));
    auto coverCol = top.removeFromRight (coverSize);
    top.removeFromRight (14);
    field (top, titleEdit);
    field (top, artistEdit);
    field (top, albumEdit);

    coverCol.removeFromTop (labelH - 6);
    coverView.setBounds (coverCol.removeFromTop (coverSize - (labelH - 6)).withSizeKeepingCentre (coverSize - (labelH - 6), coverSize - (labelH - 6)));
    coverCol.removeFromTop (8);
    coverButton->setBounds (coverCol.removeFromTop (30).withSizeKeepingCentre (coverView.getWidth(), 30));

    auto row = m.removeFromTop (labelH + 2 + fieldH + gap);
    auto left = row.removeFromLeft ((row.getWidth() - 14) / 2);
    row.removeFromLeft (14);
    field (left, yearEdit);
    field (row, trackEdit);

    field (m, genreBox);
    field (m, commentEdit, juce::jmax (fieldH, m.getHeight() - labelH - 2));
}

bool MainComponent::keyPressed (const juce::KeyPress& key)
{
    if (key == juce::KeyPress ('s', juce::ModifierKeys::commandModifier, 0)) { save(); return true; }
    if (key == juce::KeyPress ('o', juce::ModifierKeys::commandModifier, 0)) { confirmDiscard ([this] { chooseFolder(); }); return true; }
    return false;
}

void MainComponent::mouseDown (const juce::MouseEvent& e)
{
   #if ! JUCE_MAC
    if (headerArea().contains (e.getPosition()))
        if (auto* w = dynamic_cast<juce::ResizableWindow*> (window()); w != nullptr && ! w->isFullScreen())
            dragger.startDraggingComponent (w, e.getEventRelativeTo (w));
   #else
    juce::ignoreUnused (e);
   #endif
}

void MainComponent::mouseDrag (const juce::MouseEvent& e)
{
   #if ! JUCE_MAC
    if (headerArea().contains (e.getMouseDownPosition()))
        if (auto* w = dynamic_cast<juce::ResizableWindow*> (window()); w != nullptr && ! w->isFullScreen())
            dragger.dragComponent (w, e.getEventRelativeTo (w), nullptr);
   #else
    juce::ignoreUnused (e);
   #endif
}

void MainComponent::mouseDoubleClick (const juce::MouseEvent& e)
{
   #if ! JUCE_MAC
    if (headerArea().contains (e.getPosition()))
        if (auto* w = dynamic_cast<juce::DocumentWindow*> (window()))
            w->setFullScreen (! w->isFullScreen());
   #else
    juce::ignoreUnused (e);
   #endif
}

//==============================================================================
void MainComponent::paintListBoxItem (int row, juce::Graphics& g, int width, int height, bool selected)
{
    if (! juce::isPositiveAndBelow (row, files.size()))
        return;

    auto r = juce::Rectangle<float> (0, 0, (float) width, (float) height).reduced (2.0f, 2.5f);
    juce::Path shape;
    shape.addRoundedRectangle (r, 8.0f);

    g.setColour (selected ? Palette::rowSelected : (row % 2 == 0 ? Palette::rowWhite : Palette::rowBlue));
    g.fillPath (shape);

    if (selected)
    {
        g.saveState();
        g.reduceClipRegion (shape);
        g.setColour (Palette::orange);
        g.fillRect (r.withWidth (4.0f));
        g.restoreState();
    }

    auto content = r.reduced (14.0f, 0.0f);
    g.setColour (Palette::textSoft);
    g.setFont (font (14.0f));
    g.drawText (juce::String (row + 1), content.removeFromLeft (28.0f), juce::Justification::centredLeft);

    if (selected && isDirty())
    {
        g.setColour (Palette::orange);
        g.fillEllipse (content.removeFromRight (8.0f).withSizeKeepingCentre (8.0f, 8.0f));
        content.removeFromRight (6.0f);
    }

    g.setColour (Palette::text);
    g.setFont (font (14.5f, true));
    g.drawText (files[row].getFileName(), content, juce::Justification::centredLeft, true);
}

void MainComponent::selectedRowsChanged (int row)
{
    if (ignoreSelection || row == currentIndex)
        return;

    // Keep the current selection until we know the user may leave the file.
    {
        const juce::ScopedValueSetter<bool> svs (ignoreSelection, true);
        listBox.selectRow (currentIndex);
    }

    if (row < 0)
        return;

    confirmDiscard ([this, row] { selectFile (row); });
}

//==============================================================================
void MainComponent::chooseFolder()
{
    auto start = lastFolder.isDirectory() ? lastFolder : juce::File::getSpecialLocation (juce::File::userMusicDirectory);
    chooser = std::make_unique<juce::FileChooser> (U ("Vælg en mappe med lydfiler"), start);
    chooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories,
                          [this] (const juce::FileChooser& fc)
                          {
                              auto f = fc.getResult();
                              if (f.isDirectory())
                                  loadFolder (f);
                          });
}

void MainComponent::chooseFiles()
{
    auto start = lastFolder.isDirectory() ? lastFolder : juce::File::getSpecialLocation (juce::File::userMusicDirectory);
    juce::StringArray patterns;
    for (auto& e : TagIO::supportedExtensions())
        patterns.add ("*" + e);

    chooser = std::make_unique<juce::FileChooser> (U ("Vælg lydfiler"), start, patterns.joinIntoString (";"));
    chooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles
                            | juce::FileBrowserComponent::canSelectMultipleItems,
                          [this] (const juce::FileChooser& fc)
                          {
                              juce::Array<juce::File> chosen;
                              for (auto& f : fc.getResults())
                                  if (f.existsAsFile() && TagIO::isSupported (f))
                                      chosen.add (f);

                              if (chosen.isEmpty())
                                  return;

                              lastFolder = chosen.getFirst().getParentDirectory();
                              saveSettings();
                              setFiles (chosen, chosen.size() == 1 ? chosen.getFirst().getParentDirectory().getFileName()
                                                                   : U ("Valgte filer"));
                          });
}

void MainComponent::loadFolder (const juce::File& folder)
{
    lastFolder = folder;
    saveSettings();

    juce::Array<juce::File> found;
    for (auto& f : folder.findChildFiles (juce::File::findFiles, false, "*"))
        if (TagIO::isSupported (f) && ! f.getFileName().startsWith ("._") && ! f.isHidden())
            found.add (f);

    setFiles (found, folder.getFileName().isNotEmpty() ? folder.getFileName() : folder.getFullPathName());
}

void MainComponent::setFiles (juce::Array<juce::File> newFiles, const juce::String& name)
{
    std::sort (newFiles.begin(), newFiles.end(), [] (const juce::File& a, const juce::File& b)
    {
        return a.getFileName().compareNatural (b.getFileName()) < 0;
    });

    files = std::move (newFiles);
    sourceName = name;
    currentIndex = -1;
    original = {};
    editedCover = {};
    statusNote = {};

    {
        const juce::ScopedValueSetter<bool> svs (ignoreSelection, true);
        listBox.updateContent();
        listBox.deselectAllRows();
    }

    if (files.isEmpty())
    {
        showTags ({});
        updateState();
        repaint();
        return;
    }

    selectFile (0);
}

void MainComponent::selectFile (int index)
{
    if (! juce::isPositiveAndBelow (index, files.size()))
        return;

    currentIndex = index;
    {
        const juce::ScopedValueSetter<bool> svs (ignoreSelection, true);
        listBox.selectRow (index);
    }

    statusNote = {};
    TagData data;
    juce::String error;
    fileOk = TagIO::read (files[index], data, error);
    if (! fileOk)
        statusNote = error;

    original = data;
    editedCover = data;
    showTags (data);
    updateState();
    repaint();
}

//==============================================================================
void MainComponent::showTags (const TagData& d)
{
    const juce::ScopedValueSetter<bool> svs (loading, true);

    titleEdit.setText   (d.title,   false);
    artistEdit.setText  (d.artist,  false);
    albumEdit.setText   (d.album,   false);
    yearEdit.setText    (d.year  > 0 ? juce::String (d.year)  : juce::String(), false);
    trackEdit.setText   (d.track > 0 ? juce::String (d.track) : juce::String(), false);
    commentEdit.setText (d.comment, false);
    genreBox.setText    (d.genre, juce::dontSendNotification);

    coverView.setImage (d.cover.isEmpty() ? juce::Image()
                                          : juce::ImageFileFormat::loadFrom (d.cover.getData(), d.cover.getSize()));
}

TagData MainComponent::gatherTags() const
{
    TagData d;
    d.title   = titleEdit.getText().trim();
    d.artist  = artistEdit.getText().trim();
    d.album   = albumEdit.getText().trim();
    d.genre   = genreBox.getText().trim();
    d.comment = commentEdit.getText().trim();
    d.year    = yearEdit.getText().getIntValue();
    d.track   = trackEdit.getText().getIntValue();
    d.cover     = editedCover.cover;
    d.coverMime = editedCover.coverMime;
    return d;
}

bool MainComponent::isDirty() const
{
    return currentIndex >= 0 && fileOk && gatherTags() != original;
}

void MainComponent::updateState()
{
    const bool editable = currentIndex >= 0 && fileOk;
    for (juce::Component* c : { (juce::Component*) &titleEdit, (juce::Component*) &artistEdit, (juce::Component*) &albumEdit,
                                (juce::Component*) &yearEdit, (juce::Component*) &trackEdit, (juce::Component*) &commentEdit,
                                (juce::Component*) &genreBox, (juce::Component*) &coverView, (juce::Component*) coverButton.get() })
        c->setEnabled (editable);

    const bool dirty = isDirty();
    if (dirty)
        statusNote = {};
    saveButton->setEnabled (dirty);
    revertButton->setEnabled (dirty);

    listBox.repaintRow (currentIndex);
    repaint (footerArea);
}

bool MainComponent::save()
{
    if (! isDirty())
        return true;

    auto data = gatherTags();
    juce::String error;
    if (! TagIO::write (files[currentIndex], data, error))
    {
        juce::AlertWindow::showMessageBoxAsync (juce::MessageBoxIconType::WarningIcon, "Soundtag", error, "OK", this);
        return false;
    }

    // Re-read so the editor shows exactly what is now stored in the file.
    TagData stored;
    juce::String readError;
    if (TagIO::read (files[currentIndex], stored, readError))
    {
        original = stored;
        editedCover = stored;
        showTags (stored);
    }
    else
    {
        original = data;
    }

    statusNote = "gemt";
    updateState();
    return true;
}

void MainComponent::revert()
{
    editedCover = original;
    showTags (original);
    updateState();
}

void MainComponent::chooseCover()
{
    if (currentIndex < 0 || ! fileOk)
        return;

    auto start = files[currentIndex].getParentDirectory();
    chooser = std::make_unique<juce::FileChooser> (U ("Vælg et billede til cover"), start, "*.jpg;*.jpeg;*.png;*.gif;*.bmp");
    chooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                          [this] (const juce::FileChooser& fc)
                          {
                              auto f = fc.getResult();
                              if (f.existsAsFile())
                                  setCoverFromFile (f);
                          });
}

void MainComponent::setCoverFromFile (const juce::File& f)
{
    juce::MemoryBlock data;
    if (! f.loadFileAsData (data) || data.isEmpty())
        return;

    auto img = juce::ImageFileFormat::loadFrom (data.getData(), data.getSize());
    if (! img.isValid())
    {
        juce::AlertWindow::showMessageBoxAsync (juce::MessageBoxIconType::WarningIcon, "Soundtag",
                                                U ("Billedet kunne ikke læses: ") + f.getFileName(), "OK", this);
        return;
    }

    auto* b = static_cast<const unsigned char*> (data.getData());
    const bool isPng  = data.getSize() > 4 && b[0] == 0x89 && b[1] == 'P';
    const bool isJpeg = data.getSize() > 2 && b[0] == 0xff && b[1] == 0xd8;
    const int maxSide = 1200;

    if ((! isPng && ! isJpeg) || juce::jmax (img.getWidth(), img.getHeight()) > maxSide)
    {
        // Convert to a reasonably sized JPEG so the audio file does not grow needlessly.
        const float scale = juce::jmin (1.0f, (float) maxSide / (float) juce::jmax (img.getWidth(), img.getHeight()));
        auto scaled = img.rescaled (juce::jmax (1, juce::roundToInt (img.getWidth() * scale)),
                                    juce::jmax (1, juce::roundToInt (img.getHeight() * scale)),
                                    juce::Graphics::highResamplingQuality)
                         .convertedToFormat (juce::Image::RGB);

        juce::MemoryOutputStream out;
        juce::JPEGImageFormat jpeg;
        jpeg.setQuality (0.9f);
        if (! jpeg.writeImageToStream (scaled, out))
            return;

        setCover (out.getMemoryBlock(), "image/jpeg");
        return;
    }

    setCover (std::move (data), isPng ? "image/png" : "image/jpeg");
}

void MainComponent::setCover (juce::MemoryBlock data, const juce::String& mime)
{
    if (currentIndex < 0 || ! fileOk)
        return;

    editedCover.cover = std::move (data);
    editedCover.coverMime = mime;
    coverView.setImage (editedCover.cover.isEmpty()
                          ? juce::Image()
                          : juce::ImageFileFormat::loadFrom (editedCover.cover.getData(), editedCover.cover.getSize()));
    updateState();
}

void MainComponent::filesDropped (const juce::StringArray& paths, int x, int y)
{
    juce::Array<juce::File> audio;
    juce::File folder, image;

    for (auto& p : paths)
    {
        juce::File f (p);
        if (f.isDirectory())                    folder = f;
        else if (TagIO::isSupported (f))        audio.add (f);
        else if (f.hasFileExtension ("jpg;jpeg;png;gif;bmp")) image = f;
    }

    if (image.existsAsFile() && audio.isEmpty() && folder == juce::File())
    {
        juce::ignoreUnused (x, y);
        setCoverFromFile (image);
        return;
    }

    confirmDiscard ([this, audio, folder]
    {
        if (folder.isDirectory())
            loadFolder (folder);
        else if (audio.size() == 1)
        {
            // A single file: show its whole folder with that file selected.
            loadFolder (audio.getFirst().getParentDirectory());
            auto index = files.indexOf (audio.getFirst());
            if (index > 0)
                selectFile (index);
        }
        else if (! audio.isEmpty())
        {
            lastFolder = audio.getFirst().getParentDirectory();
            saveSettings();
            setFiles (audio, audio.size() == 1 ? lastFolder.getFileName() : U ("Valgte filer"));
        }
    });
}

//==============================================================================
void MainComponent::confirmDiscard (std::function<void()> then)
{
    if (! isDirty())
    {
        then();
        return;
    }

    if (messageShowing)
        return;

    messageShowing = true;
    auto options = juce::MessageBoxOptions::makeOptionsYesNoCancel (
        juce::MessageBoxIconType::QuestionIcon,
        U ("Ikke gemte ændringer"),
        U ("Vil du gemme ændringerne i \"") + files[currentIndex].getFileName() + "\"?",
        "Gem", U ("Kassér"), U ("Annullér"), this);

    juce::Component::SafePointer<MainComponent> safe (this);
    juce::AlertWindow::showAsync (options, [safe, then] (int result)
    {
        if (safe == nullptr)
            return;

        safe->messageShowing = false;
        if (result == 1)
        {
            if (safe->save())
                then();
        }
        else if (result == 2)
        {
            safe->revert();
            then();
        }
    });
}

void MainComponent::requestQuit()
{
    confirmDiscard ([] { juce::JUCEApplication::getInstance()->quit(); });
}
