#include "Node.h"
static const char * nodestr = "blue.png";

Node::Node():m_row(0),m_col(0),m_Info(0)
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

	node->initWithFile("blue.png");
	node->autorelease();

	return node;
}

float Node::GetContentWidth()
{
	static float itemWidth = 0;
    if (0 == itemWidth) {
        CCSprite *sprite = CCSprite::create();
		sprite->initWithFile("blue.png");
        itemWidth = sprite->getContentSize().width;
    }
    return itemWidth;
}

bool Node::IsNodeInfoNormal()
{
	if(m_Info == NODE_NORMAL)
		return true;
	return false;
}