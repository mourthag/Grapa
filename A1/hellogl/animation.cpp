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
    tinygltf::Accessor outputAccessor = gltf_model->accessors[sampler.input];
    tinygltf::BufferView timeBV = gltf_model->bufferViews[timeAccessor.bufferView];
    tinygltf::BufferView outputBV = gltf_model->bufferViews[outputAccessor.bufferView];
    tinygltf::Buffer *timeBuffer = gltf_model->buffers[timeBV.buffer];
    tinygltf::Buffer *outputBuffer = gltf_model->buffers[outputBV.buffer];

    int byteStride = timeBV.byteStride;
    if(byteStride = 0)
        byteStride = sizeof(float);

    std::vector<float> keys;
    std::vector<std::vector<float>> values;

    for(int keyIndex = timeBV.byteOffset + timeAccessor.byteOffset; keyIndex < timeBV.byteLength; keyIndex += byteStride) {
        float *time = (float*)&timeBuffer->data[keyIndex];
        keys.push_back(*time);
    }

    byteStride = outputBV.byteStride;
    if(byteStride = 0)
        byteStride = numComponents * sizeof(float);

    for(int valueIndex = outputBV.byteOffset + outputAccessor.byteOffset; valueIndex < outputBV.byteLength; valueIndex += byteStride) {
        std::vector value;
        for(int i = 0; i < numComponents; i++) {
            float *val = (float*)&outputBuffer->data[valueIndex + i * sizeof(float)];
            value.push_back(*val);
        }
        values.push_back(*value);
    }

    for(int index = 0; index < keys.size(); index++) {
        animationData[keys[index]] = values[index];
    }

}

void Animation::updateNode(QTime *time) {
    float seconds = time->elapsed() / 1000.0;
    std::map<float, std::vector<float>>::iterator upper, lower;

    upper = animationData.lower_bound(seconds);
    if(upper == animationData.end()) {
        qDebug() << "anim finished?";
        finished=true;
    }
    else if(upper == animationData.begin())
        qDebug() << "hä?";
    else {
        lower = std::prev(upper);
    }
    float tb = upper->first;
    float ta = lower->first;

    float alpha = seconds - ta / (tb - ta);

    std::vector<float> frameA = lower->second;
    std::vector<float> frameB = upper->second;


    switch(animationType) {

    case AnimationType::Rotation:
        QQuaternion rot = QQuaternion((1- alpha)
    }
}

bool Animation::isFinished() {
    return finished;
}

