#ifndef HELPPAGE_HPP
#define HELPPAGE_HPP

#include <QWidget>

namespace Ui {
class HelpPage;
}


class HelpPage : public QWidget
{
    Q_OBJECT
public:
    explicit HelpPage(QWidget *parent = nullptr);
    ~HelpPage();
private slots:
    void gotoHome();
    void searchFunction();
private:
    Ui::HelpPage *m_ui;
    static QMap<std::string,std::string> map;
};

#endif // HELPPAGE_HPP
