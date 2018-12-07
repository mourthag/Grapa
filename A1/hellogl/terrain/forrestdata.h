#ifndef FORRESTDATA_H
#define FORRESTDATA_H

struct ForrestData
{
    float maxHeight;
    float maxSlope;

    float maxScale;
    float minScale;

    int numTrees;

    float maxGeometryDistance;
    float maxImpostorDistance;


    ForrestData() {
        numTrees = 1;
        minScale = 2;
        maxScale = 4;
    }
};

#endif // FORRESTDATA_H
