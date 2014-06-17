#ifndef __PlayLayer_H__
#define __PlayLayer_H__

#include "cocos2d.h"
#include "Node.h"

USING_NS_CC;
enum FIND_STATE
{
	FIND_NORMAL = 0,
	FIND_SOURCE,
	FIND_DES,
	FIND_WALL,
	FIND_RUNNING,
};
class PlayLayer : public CCLayer
{
public:
	PlayLayer();
	~PlayLayer();

	static CCScene* CreatScene();
	CREATE_FUNC(PlayLayer);
    
    virtual bool init() override;
    virtual void update(float dt) override;

	//touch event
	virtual void registerWithTouchDispatcher(void);
    virtual bool ccTouchBegan(CCTouch *touch, CCEvent *unused);
    virtual void ccTouchMoved(CCTouch *touch, CCEvent *unused);
	virtual void ccTouchEnded(CCTouch *touch, CCEvent *unused);
	virtual void ccTouchCancelled(CCTouch *touch, CCEvent *unused);

	void InitMatrix();

	CCPoint PositionOfItem(int row, int col);
	Node* NodeOfPoint(CCPoint* Point);

private:
	//matrix
	Node** m_matrix;
	CC_SYNTHESIZE(int, m_width, Width);
	CC_SYNTHESIZE(int, m_height, Height);

	// for rapid count position
	CC_SYNTHESIZE(float, m_matrixLeftBottomX, LeftBottomX);
	CC_SYNTHESIZE(float, m_matrixLeftBottomY, LeftBottomY);

	//is touchable
	CC_SYNTHESIZE(bool, m_IsTouchAble, IsTouchAble);

	//state
	CC_SYNTHESIZE(int, m_State, State);

};
#endif