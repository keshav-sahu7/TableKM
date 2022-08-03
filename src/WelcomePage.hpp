#ifndef WELCOMEPAGE_HPP
#define WELCOMEPAGE_HPP

#include <QTextBrowser>

class MainWindow;

class WelcomePage : public QTextBrowser
{
    Q_OBJECT
public:
    explicit WelcomePage(MainWindow *main_window);
    ~WelcomePage();
private slots:
    void openUrl(const QUrl &url);
private:
    MainWindow *m_main_window;
};

#endif // WELCOMEPAGE_HPP
