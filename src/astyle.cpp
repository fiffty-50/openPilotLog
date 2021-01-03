#include "astyle.h"
#include <QStyle>
#include <QStyleFactory>
#include <QApplication>
#include "src/testing/adebug.h"

// [G]: Kvantum crashes
// Are there leaks when style changes?
void AStyle::setup()
{
    QApplication::setStyle(QStyleFactory::create("Windows"));
}

void AStyle::setStyle(const QString& style)
{
    QApplication::setStyle(QStyleFactory::create(style));
}

void AStyle::setStyleSheet(const QString &qss_file)
{

}
