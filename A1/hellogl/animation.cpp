#include "animation.h"

Animation::Animation(Node *targetNode, tinygltf::Model *gltf_model, int animationIndex, int channelIndex) : finished(false)
{
    node = targetNode;

    tinygltf::Animation anim = gltf_model->animations[animationIndex];
    tinygltf::AnimationChannel channel = anim.channels[channelIndex];
    tinygltf::AnimationSampler sampler = anim.samplers[channel.sampler];

    int numComponents = 1;
    if(channel.target_path == "rotation") {
        animationType = AnimationType::Rotation;
        numComponents = 4;
    }
    if(channel.target_path == "translation") {
        animationType = AnimationType::Translation;
        numComponents = 3;
    }
    if(channel.target_path == "scale") {
        animationType = AnimationType::Scaling;
        numComponents = 3;
    }

    tinygltf::Accessor timeAccessor = gltf_model->accessors[sampler.input];
    tinygltf::Accessor outputAccessor = gltf_model->accessors[sampler.output];
    tinygltf::BufferView timeBV = gltf_model->bufferViews[timeAccessor.bufferView];
    tinygltf::BufferView outputBV = gltf_model->bufferViews[outputAccessor.bufferView];
    tinygltf::Buffer *timeBuffer = &gltf_model->buffers[timeBV.buffer];
    tinygltf::Buffer *outputBuffer = &gltf_model->buffers[outputBV.buffer];

    int byteStride = timeBV.byteStride;
    if(byteStride == 0)
        byteStride = sizeof(float);

    std::vector<float> keys;
    std::vector<std::vector<float>> values;

    size_t offset = timeBV.byteOffset + timeAccessor.byteOffset;

    for(int keyIndex = 0; keyIndex < timeAccessor.count; keyIndex += 1) {
        float *time = (float*)&timeBuffer->data[offset + byteStride * keyIndex];
        keys.push_back(*time);
    }

    byteStride = outputBV.byteStride;
    if(byteStride == 0)
        byteStride = numComponents * sizeof(float);

    offset = outputBV.byteOffset + outputAccessor.byteOffset;

    for(int valueIndex = offset; valueIndex < offset + outputBV.byteLength; valueIndex += byteStride) {
        std::vector<float> value;
        for(int i = 0; i < numComponents; i++) {
            float *val = (float*)&outputBuffer->data[valueIndex + i * sizeof(float)];
            value.push_back(*val);
        }
        values.push_back(value);
    }

    for(int index = 0; index < keys.size(); index++) {
        animationData[keys[index]] = values[index];
    }

}

void Animation::updateNode(QTime *time) {
    float seconds = time->elapsed() / 1000.0;
    std::map<float, std::vector<float>>::iterator upper, lower;

    finished = false;

    upper = animationData.lower_bound(seconds);

    float alpha ;

    if(upper == animationData.end()) {

        upper = std::prev(animationData.end());
        lower = std::prev(animationData.end());

        alpha = 1;

        finished=true;

    }
    else if(upper == animationData.begin()) {

        upper = animationData.begin();
        lower = animationData.begin();

        alpha = 0;

    }

    else {
        lower = std::prev(upper);
        float tb = upper->first;
        float ta = lower->first;
        alpha = seconds - ta / (tb - ta);
    }


    std::vector<float> frameA = lower->second;
    std::vector<float> frameB = upper->second;

    QMatrix4x4 newMat;

    switch(animationType) {

    case AnimationType::Rotation:
    {
        QQuaternion rotA = QQuaternion(frameA[0], frameA[1],
                frameA[2], frameA[3]);
        QQuaternion rotB = QQuaternion(frameB[0], frameB[1],
                frameB[2], frameB[3]);
        QQuaternion rot = QQuaternion::slerp(rotA, rotB, alpha);
        newMat.rotate(rot);
        break;
    }
    case AnimationType::Translation:
    {
        QVector3D translationA = QVector3D (frameA[0], frameA[1], frameA[2]);
        QVector3D translationB = QVector3D(frameB[0], frameB[1], frameB[2]);
        newMat.translate((1-alpha)*translationA + alpha * translationB);
        break;
    }
    case AnimationType::Scaling:
    {
        QVector3D scaleA = QVector3D (frameA[0], frameA[1], frameA[2]);
        QVector3D scaleB = QVector3D(frameB[0], frameB[1], frameB[2]);
        newMat.scale((1-alpha) * scaleA + alpha * scaleB);
    }
    }
    node->updateModelMatrix( newMat);
}

bool Animation::isFinished() {
    return finished;
}

