#ifndef VALIDATORFACTORY_H
#define VALIDATORFACTORY_H
#include <QCompleter>

//#define OPL::CompleterProvider CompleterProvider::getInstance()
/*!
 * \brief The CompleterProvider class provides static QCompleter instances
 * that are set up with application-wide behaviour standards.
 * \details In order to facilitate correct and user-friendly input
 * throughout the application, a combination of different methods are
 * used. The QCompleter is used on QLineEdit input fields and provides
 * pattern-completion. The instances created by this factory are
 * set up with application-wide behaviour standards in mind to create
 * a consistent user experience. The QCompleters' models are based on
 * input from the database, so whenever the database content is modified,
 * it is important to call updateModel.
 */
class CompleterProvider
{
    CompleterProvider();

    QCompleter* pilotCompleter;
    QCompleter* airportCompleter;
    QCompleter* tailsCompleter;
public:
    static CompleterProvider& getInstance() {
        static CompleterProvider instance;
        return instance;
    }

    CompleterProvider(CompleterProvider const&) = delete;
    void operator=(CompleterProvider const&) = delete;
    ~CompleterProvider();

    enum CompleterTarget {Pilots, Tails, Airports};

    /*!
     * \brief Create a new QCompleter for the given CompleterTarget
     */
    //static QCompleter *newCompleter(CompleterTarget target, QObject* parent = nullptr);

    /*!
     * \brief updates the completion model for a given QCompleter
     */
    void updateModel(CompleterTarget target);

    /*!
     * \brief return a pointer to the static completer instance
     */
    QCompleter *getCompleter(CompleterTarget target) const;
};

#endif // VALIDATORFACTORY_H
