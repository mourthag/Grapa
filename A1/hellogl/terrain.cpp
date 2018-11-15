#include "terrain.h"

void Terrain::readInt(QDataStream *stream, int *result)
{
    char widthText[10];

    for( int i = 0 ; i < 10; i++) {
        stream->readRawData(&widthText[i], 1);
        if(widthText[i] == '\0' || widthText[i] == '\n')
            break;
    }

    sscanf(widthText, "%d", result);
}

Terrain::Terrain(QFile *pgmFile)
{
    initializeOpenGLFunctions();

    pgmFile->open(QIODevice::ReadOnly);
    QDataStream stream(pgmFile);

    char format[2];

    stream.readRawData(format, 2);

    //skip \n
    stream.skipRawData(1);

    int width;
    readInt(&stream, &width);

    int height;
    readInt(&stream, &height);

    int maxVal;
    readInt(&stream, &maxVal);

    if( maxVal  != 65535) {
        qDebug() << "Invalid Values";
        return;
    }

    std::vector<char> data(2 * width * height);
    stream.readRawData(&data.at(0), 2 * width * height);

    std::vector<unsigned short> heights;

    for(int i = 0; i < width * height; i = i + 2) {
        unsigned short value;
        value = (unsigned short)data[i];
        value <<=  CHAR_BIT;
        value += (unsigned short)data[i+1];
        heights.push_back(value);
    }

    glGenTextures(1, &heightMap);
    glBindTexture(GL_TEXTURE_2D, heightMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16I, width, height, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, &heights.at(0));
    glGenerateMipmap(GL_TEXTURE_2D);

}
