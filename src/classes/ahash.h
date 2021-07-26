#ifndef AHASH_H
#define AHASH_H
#include <QtCore>
#include <QByteArray>
#include <QCryptographicHash>

/*!
 * \brief The AHash class is responsible for calculating cryptographic hashes of files (used to verify downloads)
 */
class AHash
{
public:
    /*!
     * \brief AHash - calculates the MD5-checksum for the parameter given in the constructor and
     * saves the result in the checksum member variable
     */
    AHash(QFile &file);

    AHash(QFileInfo &file_info);

    QByteArray checksum;

    /*!
     * \brief hashString returns a hex representation of the hash
     */
    inline const QString hashToHex()
    {
            return QString(checksum.toHex());
    };

    /*!
     * \brief compare reads the pre-calculated md5-sum from a checkfile and compares the hashes.
     * \param md5_file - the checkfile containing the md5 checksum in hex format
     * \return true if hashes match
     */
    bool compare(QFileInfo &md5_file);
};

#endif // AHASH_H
