#include "strictregularexpressionvalidator.h"

QValidator::State StrictRegularExpressionValidator::validate(QString &txt, int &pos) const
{
    auto validation = QRegularExpressionValidator::validate(txt, pos);
    if(validation == QValidator::Intermediate) {
        return QValidator::Invalid;
    }
    return validation;
}
