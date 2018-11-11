#ifndef ANIMATION_H
#define ANIMATION_H

#include <node.h>
#include <QTime>
#include <iterator>
#include "tiny_gltf.h"

class Animation
{

public:


    Animation(Node *targetNode, tinygltf::Model *gltf_model, int animationIndex, int channelIndex);

    enum AnimationType {
        Rotation,
        Translation,
        Scaling
    };

    void updateNode(QTime *time);
    bool isFinished();

private:
    bool finished;

    AnimationType animationType;
    Node *node;
    std::map<float , std::vector<float>> animationData;
};

#endif // ANIMATION_H
