#ifndef _NODE_H_
#define _NODE_H_

#include "cocos2d.h"

USING_NS_CC;

class Node:public CCSprite
{
public:
	Node();
	~Node();
	
	static Node* Create(int row, int col);
	static float GetContentWidth();
private:
	int m_row;
	int m_col;

};
#endif