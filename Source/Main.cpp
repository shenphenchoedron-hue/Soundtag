#include "MainComponent.h"

class SoundtagApplication final : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override    { return JUCE_APPLICATION_NAME_STRING; }
    const juce::String getApplicationVersion() override { return JUCE_APPLICATION_VERSION_STRING; }
    bool moreThanOneInstanceAllowed() override          { return true; }

    void initialise (const juce::String& commandLine) override
    {
        mainWindow = std::make_unique<MainWindow> (getApplicationName());
        openFromCommandLine (commandLine);
    }

    void anotherInstanceStarted (const juce::String& commandLine) override
    {
        openFromCommandLine (commandLine);
    }

    /** Opens folders / files given on the command line (or via "Open with"). */
    void openFromCommandLine (const juce::String& commandLine)
    {
        juce::StringArray paths;
        for (auto& arg : juce::StringArray::fromTokens (commandLine, true))
        {
            auto p = arg.unquoted();
            if (juce::File::isAbsolutePath (p) && juce::File (p).exists())
                paths.add (p);
        }

        if (! paths.isEmpty() && mainWindow != nullptr)
            if (auto* mc = dynamic_cast<MainComponent*> (mainWindow->getContentComponent()))
                mc->filesDropped (paths, 0, 0);
    }

    void shutdown() override { mainWindow = nullptr; }

    void systemRequestedQuit() override
    {
        if (mainWindow != nullptr)
            if (auto* mc = dynamic_cast<MainComponent*> (mainWindow->getContentComponent()))
            {
                mc->requestQuit();
                return;
            }

        quit();
    }

    class MainWindow final : public juce::DocumentWindow
    {
    public:
        explicit MainWindow (const juce::String& name)
            : DocumentWindow (name, juce::Colour (0xffeef2f8), DocumentWindow::allButtons)
        {
           #if JUCE_MAC
            setUsingNativeTitleBar (true);
           #else
            // App-drawn title bar on Windows / Linux - see MainComponent.
            setUsingNativeTitleBar (false);
            setTitleBarHeight (0);
           #endif

            setContentOwned (new MainComponent(), true);
            setResizable (true, false);
            setResizeLimits (640, getHeight(), 4000, 4000);
            centreWithSize (getWidth(), getHeight());
            setVisible (true);
        }

        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }

       #if ! JUCE_MAC
        int getDesktopWindowStyleFlags() const override
        {
            return DocumentWindow::getDesktopWindowStyleFlags()
                 | juce::ComponentPeer::windowHasMinimiseButton
                 | juce::ComponentPeer::windowHasMaximiseButton
                 | juce::ComponentPeer::windowHasCloseButton;
        }
       #endif

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION (SoundtagApplication)
