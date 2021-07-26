#include "ahash.h"
#include "src/opl.h"

AHash::AHash(QFile &file)
{
    if (file.open(QFile::ReadOnly)) {
        QCryptographicHash hash(QCryptographicHash::Md5);
        if (hash.addData(&file)) {
            checksum = hash.result();
            DEB << "File: " << file;
            DEB << "Hash: " << hash.result();
        } else {
            checksum = QByteArray();
        }
    } else {
        checksum = QByteArray();
    }
    file.close();
}

AHash::AHash(QFileInfo &file_info)
{
    QFile f(file_info.absoluteFilePath());
    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(QCryptographicHash::Md5);
        if (hash.addData(&f)) {
            checksum = hash.result();
            DEB << "File: " << f;
            DEB << "Hash: " << hash.result().toHex();
        } else {
            checksum = QByteArray();
        }
    } else {
        checksum = QByteArray();
    }
    f.close();
}

bool AHash::compare(QFileInfo &md5_file)
{
    // Open the file and read the first 32 characters
    QFile f(md5_file.absoluteFilePath());
    if (f.open(QFile::ReadOnly)) {
        QTextStream in(&f);
        const QString hash_string = in.read(32);
        DEB << "Checksum:" << hash_string;

        // Verify checksum is not empty and compare to md5 read from file
        if (checksum == QByteArray())
            return false;
        else
            if (checksum.toHex() == hash_string)
                return true;
            else
                return false;
    }
    return false;
}
