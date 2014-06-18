#ifndef __PlayLayer_H__
#define __PlayLayer_H__

#include "cocos2d.h"
#include "Node.h"
#include <list>

USING_NS_CC;

//
static const char* stepRes[4] =
{
	"step1.png",
	"step2.png",
	"step3.png",
	"step4.png",
};

typedef std::list<Node*> Node_List;
typedef int Direction_Int;
//×´Ì¬
enum FIND_STATE
{
	FIND_NORMAL = -1,
	FIND_RUNNING = 0,
	FIND_END,
	FIND_PRINT,
};

//
enum FIND_RESULT
{
	RESULT_ERROR = -1,
	RESULT_GET = 0,
	RESULT_NOT = 1,
};

//·½Ïò
enum DIRECTION
{
	DIR_RIGHT,
	DIR_LEFT,
	DIR_UP,
	DIR_DOWN,
	DIR_RIGHT_UP,
	DIR_LEFT_UP,
	DIR_RIGHT_DOWN,
	DIR_LEFT_DOWN,
	DIR_ALL,
};

class PlayLayer : public CCLayer
{
public:
	PlayLayer();
	~PlayLayer();

	static CCScene* CreatScene();
	CREATE_FUNC(PlayLayer);
    
    virtual bool init() override;
    virtual void update(float dt);

	//touch event
	virtual void registerWithTouchDispatcher(void);
    virtual bool ccTouchBegan(CCTouch *touch, CCEvent *unused);
    virtual void ccTouchMoved(CCTouch *touch, CCEvent *unused);
	virtual void ccTouchEnded(CCTouch *touch, CCEvent *unused);
	virtual void ccTouchCancelled(CCTouch *touch, CCEvent *unused);

	void InitMatrix();

	CCPoint PositionOfItem(int row, int col);
	Node* NodeOfPoint(CCPoint* Point);
	Node* NodeOfIndex(int row, int col);

	//set
	bool SetSourcePos(Node* touch_Node);
	bool SetDesPos(Node* touch_Node);
	bool SetWallPos(Node* touch_Node);
	bool DeleteWall(Node* touch_Node);

	//A Star
	int AStarFindPath();
	void Add2OpenList(Node* p_node);
	void AddNeighbor2Openlist(Node* p_node);
	bool CheckAddNeighborNode(Node* neighbor_Node, Node* father_Node, Direction_Int dir);
	int Add2CloseList(Node* p_node);
	Node* FindMin_F_OpenList();
	void DeleteFromOpen(Node* pNode);
	void ComputeNodeGH(Node* pNode, Node* pFather, Direction_Int dir);
	void PrintWay();
private:
	CCSpriteBatchNode *spriteSheet;
	//openlist
	Node_List m_openList;
	//closelist
	Node_List m_closeList;

	//source
	Node* m_SourceNode;
	//des
	Node* m_DesNode;

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

	//has began
	CC_SYNTHESIZE(int, m_hasBegan, HasBegan);

};
#endif