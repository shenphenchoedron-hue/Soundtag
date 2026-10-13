#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "TagIO.h"

#include <functional>

//==============================================================================
/** A button whose appearance is drawn by a lambda. */
class DrawnButton final : public juce::Button
{
public:
    using Drawer = std::function<void (juce::Graphics&, juce::Rectangle<float>, bool over, bool down, bool enabled)>;

    DrawnButton (const juce::String& name, Drawer d) : Button (name), drawer (std::move (d))
    {
        setMouseCursor (juce::MouseCursor::PointingHandCursor);
    }

    void paintButton (juce::Graphics& g, bool over, bool down) override
    {
        if (drawer)
            drawer (g, getLocalBounds().toFloat(), over && isEnabled(), down && isEnabled(), isEnabled());
    }

private:
    Drawer drawer;
};

//==============================================================================
class SoundtagLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    SoundtagLookAndFeel();

    void fillTextEditorBackground (juce::Graphics&, int w, int h, juce::TextEditor&) override;
    void drawTextEditorOutline (juce::Graphics&, int w, int h, juce::TextEditor&) override;

    void drawComboBox (juce::Graphics&, int w, int h, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox&) override;
    void positionComboBoxText (juce::ComboBox&, juce::Label&) override;
    juce::Font getComboBoxFont (juce::ComboBox&) override;
    juce::Font getPopupMenuFont() override;

    void drawScrollbar (juce::Graphics&, juce::ScrollBar&, int x, int y, int w, int h,
                        bool isVertical, int thumbStart, int thumbSize,
                        bool isMouseOver, bool isMouseDown) override;
    int getDefaultScrollbarWidth() override { return 8; }
};

//==============================================================================
/** Shows the album cover (or a placeholder). */
class CoverView final : public juce::Component,
                        public juce::SettableTooltipClient
{
public:
    void setImage (const juce::Image& img) { image = img; repaint(); }
    bool hasImage() const                  { return image.isValid(); }

    std::function<void()> onClick, onRemove;

    void paint (juce::Graphics&) override;
    void mouseUp (const juce::MouseEvent&) override;

private:
    juce::Image image;
};

//==============================================================================
class MainComponent final : public juce::Component,
                            public juce::FileDragAndDropTarget,
                            private juce::ListBoxModel
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    bool keyPressed (const juce::KeyPress&) override;

    void mouseDown (const juce::MouseEvent&) override;
    void mouseDrag (const juce::MouseEvent&) override;
    void mouseDoubleClick (const juce::MouseEvent&) override;

    // Drag & drop of folders, audio files or cover images.
    bool isInterestedInFileDrag (const juce::StringArray&) override { return true; }
    void filesDropped (const juce::StringArray&, int x, int y) override;

    /** Called by the app when the user wants to quit (asks about unsaved changes). */
    void requestQuit();

private:
    // ListBoxModel
    int  getNumRows() override { return files.size(); }
    void paintListBoxItem (int row, juce::Graphics&, int width, int height, bool selected) override;
    void selectedRowsChanged (int lastRowSelected) override;

    // Files
    void chooseFolder();
    void chooseFiles();
    void loadFolder (const juce::File& folder);
    void setFiles (juce::Array<juce::File> newFiles, const juce::String& sourceName);
    void selectFile (int index);

    // Editing
    void showTags (const TagData&);
    TagData gatherTags() const;
    bool isDirty() const;
    void updateState();
    bool save();
    void revert();
    void chooseCover();
    void setCoverFromFile (const juce::File&);
    void setCover (juce::MemoryBlock data, const juce::String& mime);

    /** Runs 'then' once unsaved changes have been saved or discarded (or right away). */
    void confirmDiscard (std::function<void()> then);

    void saveSettings();

    juce::Rectangle<int> headerArea() const { return getLocalBounds().removeFromTop (headerHeight); }
    juce::Component* window() { return getTopLevelComponent(); }

    SoundtagLookAndFeel lnf;
    juce::TooltipWindow tooltipWindow { this, 600 };

    std::unique_ptr<DrawnButton> minimiseButton, maximiseButton, closeButton;
    std::unique_ptr<DrawnButton> openFolderButton, openFilesButton, coverButton, revertButton, saveButton;

    juce::ListBox listBox { "files", this };

    juce::TextEditor titleEdit, artistEdit, albumEdit, yearEdit, trackEdit, commentEdit;
    juce::ComboBox genreBox;
    CoverView coverView;

    struct FieldLabel { juce::String text; juce::Rectangle<int> area; };
    std::vector<FieldLabel> labels;

    juce::ComponentDragger dragger;
    juce::Rectangle<int> toolbarInfoArea, listCard, metaCard, metaTitleArea, footerArea, statusArea;
    int headerHeight = 0;

    juce::Array<juce::File> files;
    juce::String sourceName;
    int currentIndex = -1;
    TagData original;      // tags as loaded from disk
    TagData editedCover;   // only cover/coverMime used: current (possibly changed) cover
    bool loading = false;
    bool ignoreSelection = false;
    bool messageShowing = false;
    bool fileOk = false;
    juce::String statusNote;

    juce::File lastFolder;
    std::unique_ptr<juce::PropertiesFile> settings;
    std::unique_ptr<juce::FileChooser> chooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
