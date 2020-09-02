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
