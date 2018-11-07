#ifndef NODE_H
#define NODE_H

#include <QMatrix4x4>

class Node
{
public:
    Node();

private:
    std::vector<Node> childs;
    QMatrix4x4 modelMatrix;
};

#endif // NODE_H
