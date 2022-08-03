#ifndef VALIDATORFACTORY_HPP
#define VALIDATORFACTORY_HPP

#include <QValidator>
#include <kmt/Core.hpp>

class ValidatorFactory : public QObject
{
private:
    ValidatorFactory();
public:
    static ValidatorFactory &instance();
    QValidator *validator(km::DataType data_type);
    QValidator *validator(km::IndexType data_type_index);
private:
    std::array<QValidator*,9> m_validator_list;
};

using VldFactory = ValidatorFactory;
#endif // VALIDATORFACTORY_HPP
