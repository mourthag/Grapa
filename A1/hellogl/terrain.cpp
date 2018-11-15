#include "terrain.h"

Terrain::Terrain(QFile *pgmFile)
{
    pgmFile->open(QIODevice::ReadOnly);
    QDataStream stream(pgmFile);

    char *format = new char[2];
    unsigned short width;

    stream.readRawData(format, 2);
    stream >> width;
    qDebug() << format[0] << format[1] << width;

}
