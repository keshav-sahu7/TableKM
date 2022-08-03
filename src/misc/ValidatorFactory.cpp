#include "ValidatorFactory.hpp"

ValidatorFactory::ValidatorFactory()
{
    m_validator_list[0] = new QIntValidator(this);
    m_validator_list[1] = new QRegularExpressionValidator(QRegularExpression("[-\\+]?\\d{1,19}"));
    m_validator_list[2] = new QDoubleValidator(this);
    m_validator_list[3] = new QDoubleValidator(this);
    m_validator_list[4] = nullptr;
    m_validator_list[5] = new QRegularExpressionValidator(
                QRegularExpression("(True)|(False)"),this);
    m_validator_list[6] = new QRegularExpressionValidator(
                QRegularExpression("((0[1-9])|([12][0-9])|(3[01]))/"                    //day   dd
                                   "((0[1-9])|(1[0-2]))/"                               //month mm
                                   "\\d{4}"),this);                                     //year  yyyy
    m_validator_list[7] = new QRegularExpressionValidator(
                QRegularExpression("((0[1-9])|([12][0-9])|(3[01]))/"                    //day   dd
                                   "((0[1-9])|(1[0-2]))/"                               //month mm
                                   "\\d{4}\\s+"                                         //year  yyyy
                                   "([01]\\d+)|(2[0-3]):[0-5]\\d:[0-5]\\d"),this);      //time hh:mm:ss
    m_validator_list[8] = nullptr;
}

ValidatorFactory &ValidatorFactory::instance()
{
    static ValidatorFactory inst;
    return inst;
}

QValidator *ValidatorFactory::validator(km::DataType data_type)
{
    return m_validator_list[km::indexForDataType(data_type,8)];
}

QValidator *ValidatorFactory::validator(km::IndexType data_type_index)
{
    return m_validator_list[data_type_index < 8 ? data_type_index : 8];
}
