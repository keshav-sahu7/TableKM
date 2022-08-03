#ifndef SWITCHLOGHANDLERHELPER__H
#define SWITCHLOGHANDLERHELPER__H

#include <kmt/ErrorHandler.hpp>

class SwitchLogHandlerHelper_
{
    using ErrorHandler = km::err::ErrorHandler;
public:
    SwitchLogHandlerHelper_(km::err::ErrorHandler new_hdlr)
        : m_old_hdlr(km::err::getErrorHandler()),
          m_new_hdlr(new_hdlr)
    {
        km::err::setErrorHandler(new_hdlr);
    }
    void resetToOld()
    {
        km::err::setErrorHandler(m_old_hdlr);
    }
    void setToNew()
    {
        km::err::setErrorHandler(m_new_hdlr);
    }
    ~SwitchLogHandlerHelper_()
    {
        km::err::setErrorHandler(m_old_hdlr);
    }
private:
    km::err::ErrorHandler m_old_hdlr;
    km::err::ErrorHandler m_new_hdlr;
};

#endif // SWITCHLOGHANDLERHELPER__H
