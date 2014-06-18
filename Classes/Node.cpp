#include "Node.h"
#include "PlayLayer.h"
//static const char * nodestr = "blue.png";

Node::Node():m_row(0),m_col(0),m_nodeType(0),m_G(0),m_H(0),m_fatherNode(NULL)
{
}

Node::~Node()
{
}

Node* Node::Create(int row, int col)
{
	Node* node = new Node();
	if(node == NULL)
		return NULL;

	node->m_row = row;
	node->m_col = col;

	node->initWithSpriteFrameName(stepRes[0]);
	//node->initWithFile("step1.png");
	node->autorelease();

	return node;
}

float Node::GetContentWidth()
{
	static float itemWidth = 0;
    if (0 == itemWidth) {
        CCSprite *sprite = CCSprite::create();
		sprite->initWithFile("step1.png");
        itemWidth = sprite->getContentSize().width;
    }
    return itemWidth;
}

bool Node::IsNodeInfoNormal()
{
	if(m_nodeType == NODE_NORMAL)
		return true;
	return false;
}