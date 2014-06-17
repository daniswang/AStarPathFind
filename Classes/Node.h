#ifndef _NODE_H_
#define _NODE_H_

#include "cocos2d.h"

USING_NS_CC;

enum NODE_TYPE
{
	NODE_NORMAL = 0,
	NODE_SOURCE,
	NODE_DES,
	NODE_WALL,
};

class Node:public CCSprite
{
public:
	Node();
	~Node();
	
	static Node* Create(int row, int col);
	static float GetContentWidth();
	bool IsNodeInfoNormal();
private:
	CC_SYNTHESIZE(int, m_row, Row);
	CC_SYNTHESIZE(int, m_col, Col);

	//
	CC_SYNTHESIZE(int, m_Info, Info);

};
#endif