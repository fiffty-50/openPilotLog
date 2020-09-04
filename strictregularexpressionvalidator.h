#ifndef STRICTREGULAREXPRESSIONVALIDATOR_H
#define STRICTREGULAREXPRESSIONVALIDATOR_H

#include <QRegularExpression>
#include <QValidator>

/*!
 * \brief The StrictRegularExpressionValidator class only returns Invalid or Acceptable
 */
class StrictRegularExpressionValidator : public QRegularExpressionValidator {
public:
    QValidator::State validate(QString& txt, int& pos) const;
};

#endif // STRICTREGULAREXPRESSIONVALIDATOR_H

/*class StrictRegularExpressionValidator : public QRegularExpressionValidator {
public:
    QValidator::State validate(QString& txt, int& pos) const {
        {
            auto validation = QRegularExpressionValidator::validate(txt, pos);
            if(validation == QValidator::Intermediate) {
                return QValidator::Invalid;
            }
            return validation;
        }
    }
};*/
