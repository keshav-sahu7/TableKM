#include "MainWindow.hpp"
#include "WelcomePage.hpp"

WelcomePage::WelcomePage(MainWindow *main_window) :
    QTextBrowser(main_window),
    m_main_window(main_window)
{
    assert(m_main_window);
    setObjectName("WelcomePage");
    setOpenLinks(false);
    connect(this, &WelcomePage::anchorClicked, this, &WelcomePage::openUrl);
    setSource(QUrl("qrc:/docs/welcome/welcome.html"));
}

WelcomePage::~WelcomePage()
{
    //
}

void WelcomePage::openUrl(const QUrl &url)
{
    if(url.scheme() == "km")
    {
        if(url.host() == "create-new-table")
            m_main_window->createNewTable();
        else if(url.host() == "open-existing-table")
            m_main_window->openExistingTable();
        else if(url.host() == "open-documentation")
            m_main_window->openDocumentationPage();
    }
}
