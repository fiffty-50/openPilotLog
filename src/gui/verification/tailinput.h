#ifndef TAILINPUT_H
#define TAILINPUT_H

#include "userinput.h"

class TailInput : public UserInput
{
public:
    TailInput() = delete;
    TailInput(const QString &input) : UserInput(input) {}
public:
    bool isValid() const override;
    QString fixup() const override;
};

#endif // TAILINPUT_H
