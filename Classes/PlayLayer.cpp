#include "PlayLayer.h"
#include "Node.h"

#define MAX_WIDETH  (7);
#define MAX_HEIGTH (9);
#define NODE_GAP (0.5)

//nodetyp source
static const char * nodeTypeStr[4] = 
{
	"nouse",
	"nodetype1.png",
	"nodetype2.png",
	"nodetype3.png",
};

//方向 权值对照
static const Direction_Int DirWayRight[DIR_ALL] =
{
	10, 
	10, 
	10, 
	10, 
	14, 
	14, 
	14, 
	14
};

PlayLayer::PlayLayer():m_width(0),m_height(0),m_IsTouchAble(true),m_State(FIND_NORMAL),m_hasBegan(-1),m_SourceNode(NULL),m_DesNode(NULL)
{
}

PlayLayer::~PlayLayer()
{
	if(m_matrix != NULL)
		free(m_matrix);
}

CCScene* PlayLayer::CreatScene()
{
	auto scene = CCScene::create();
	auto playLayer = create();
	scene->addChild(playLayer);
	return scene;
}

bool PlayLayer::init()
{
	if(!CCLayer::init())
		return false;
	
	//background
	CCSize WinSize =CCDirector::sharedDirector()->getWinSize();
	auto background = CCSprite::create("bg.png");
	background->setAnchorPoint(CCPoint(0, 1));
	background->setPosition(CCPoint(0, WinSize.height));
	addChild(background);

	//init spriteSheet;
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("AStarPathFind.plist");
    spriteSheet = CCSpriteBatchNode::create("AStarPathFind.pvr.ccz");
    addChild(spriteSheet);

	m_width = MAX_WIDETH;
	m_height = MAX_HEIGTH;

	m_matrixLeftBottomX = (WinSize.width - Node::GetContentWidth() * m_width - (m_width - 1) * NODE_GAP) / 2;
    m_matrixLeftBottomY = (WinSize.height - Node::GetContentWidth() * m_height - (m_height - 1) * NODE_GAP) / 2;
	
	int arraySize = sizeof(Node*) * m_width * m_height;
	m_matrix = (Node**)malloc(arraySize);
	memset((void*)m_matrix, 0, arraySize);

	InitMatrix();
	CCDirector::sharedDirector()->getScheduler()->scheduleUpdateForTarget(this, 0, false);

	//register touchevent
	registerWithTouchDispatcher();

	return true;
}

void PlayLayer::InitMatrix()
{
	for(int i = 0;i < m_height;i ++)
	{
		for(int l = 0;l < m_width;l ++)
		{
			Node* node = Node::Create(i, l);
			CCPoint point = PositionOfItem(i, l);
			node->setPosition(point);
			addChild(node);
			*(m_matrix + i * m_width + l) = node;
		}
	}
}

void PlayLayer::update(float dt)
{
	 //CCLOG("baibai");
	if(m_SourceNode && m_DesNode && m_State != FIND_END && m_State != FIND_PRINT)
		m_State = FIND_RUNNING;

	if(m_State == FIND_RUNNING)
	{//begin A Star
		int findResult = AStarFindPath();
	}
	
	if(m_State == FIND_END)
	{
		PrintWay();
	}
}

/************** touch event************************/
void PlayLayer::registerWithTouchDispatcher(void)
{//register touch event
	CCLayer::registerWithTouchDispatcher();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
}
 
bool PlayLayer::ccTouchBegan(CCTouch *touch, CCEvent *unused)
{
	if(m_IsTouchAble)
	{
		auto location = touch->getLocation();
		Node* touchedNode = NodeOfPoint(&location);
		if(!touchedNode)
			return false;
		if(!m_SourceNode)
			bool scuess = SetSourcePos(touchedNode);
		if(m_SourceNode && !m_DesNode)
			bool sucess =SetDesPos(touchedNode);
		//bool sucess = SetWallPos(touchedNode);
	}
	return m_IsTouchAble;
}

void PlayLayer::ccTouchMoved(CCTouch *touch, CCEvent *unused)
{
	if(!m_IsTouchAble)
		return;
	//m_IsTouchAble = false;
}

void PlayLayer::ccTouchEnded(CCTouch *touch, CCEvent *unused)
{
	if(m_IsTouchAble)
	{
		//m_IsTouchAble = false;
		
	}
	return;
}

void PlayLayer::ccTouchCancelled(CCTouch *touch, CCEvent *unused)
{
	return;
}
// touch end

CCPoint PlayLayer::PositionOfItem(int row, int col)
{
	 float x = m_matrixLeftBottomX + (Node::GetContentWidth() + NODE_GAP) * col + Node::GetContentWidth() / 2;
    float y = m_matrixLeftBottomY + (Node::GetContentWidth() + NODE_GAP) * row + Node::GetContentWidth() / 2;
    return CCPoint(x, y);
}

Node* PlayLayer::NodeOfPoint(CCPoint* point)
{
	Node* node = NULL;

	CCRect rect = CCRect(0, 0, 0, 0);
	for(int i = 0;i < m_height;i ++)
	{
		for(int l = 0;l < m_width;l ++)
		{
			Node* node = NULL;
			node = *(m_matrix+ i * m_width + l);
			if(node)
			{
				rect.origin.x = node->getPositionX() - (node->getContentSize().width / 2);
				rect.origin.y = node->getPositionY() - (node->getContentSize().height / 2);
				rect.size = node->getContentSize();
				if(rect.containsPoint(*point))
					return node;
			}
		}
	}

	return NULL;
}

Node* PlayLayer::NodeOfIndex(int row, int col)
{
	if(row >= m_height || col >= m_width || row < 0 || col < 0)
		return NULL;

	return *(m_matrix + row * m_width + col);
}

/*************set pos*******************/
//set source
bool PlayLayer::SetSourcePos(Node* touch_Node)
{
	if(!touch_Node->IsNodeInfoNormal())
		return false;

	//delete old
	bool searched = false;
	for(int i = 0;i < m_height;i ++)
	{
		for(int l = 0;l < m_width;l ++)
		{
			if((*(m_matrix + i * m_width + l))->getNodeType() == NODE_SOURCE)
			{
				(*(m_matrix + i * m_width + l))->removeAllChildren();
				(*(m_matrix + i * m_width + l))->setNodeType(NODE_NORMAL);
				searched = true;
				break;
			}
			if(searched)
				break;
		}
	}
	//add new
	touch_Node->setNodeType(NODE_SOURCE);

	CCSprite* addNode = CCSprite::create(nodeTypeStr[NODE_SOURCE]);
	CCPoint addlocation = PositionOfItem(touch_Node->getRow(), touch_Node->getCol());
	addNode->setPosition(CCPoint(touch_Node->getContentSize().width/2 - 0.5, touch_Node->getContentSize().height/2 + 0.5));
	touch_Node->addChild(addNode);

	m_SourceNode = touch_Node;
	return true;
}

//set des
bool PlayLayer::SetDesPos(Node* touch_Node)
{
	if(!touch_Node->IsNodeInfoNormal())
		return false;

	//delete old
	bool searched = false;
	for(int i = 0;i < m_height;i ++)
	{
		for(int l = 0;l < m_width;l ++)
		{
			if((*(m_matrix + i * m_width + l))->getNodeType() == NODE_DES)
			{
				(*(m_matrix + i * m_width + l))->removeAllChildren();
				(*(m_matrix + i * m_width + l))->setNodeType(NODE_NORMAL);
				searched = true;
				break;
			}
			if(searched)
				break;
		}
	}

	//add new
	touch_Node->setNodeType(NODE_DES);

	CCSprite* addNode = CCSprite::create(nodeTypeStr[NODE_DES]);
	CCPoint addlocation = PositionOfItem(touch_Node->getRow(), touch_Node->getCol());
	addNode->setPosition(CCPoint(touch_Node->getContentSize().width/2 - 0.5, touch_Node->getContentSize().height/2 + 0.5));
	touch_Node->addChild(addNode);

	m_DesNode = touch_Node;
	return true;
}

//set wall
bool PlayLayer::SetWallPos(Node* touch_Node)
{
	// 重复点击取消
	if(touch_Node->getNodeType() == NODE_WALL)
	{
		DeleteWall(touch_Node);
		return true;
	}

	if(!touch_Node->IsNodeInfoNormal())
		return false;

	//add new
	if(touch_Node->getNodeType() == NODE_NORMAL)
	{
		touch_Node->setNodeType(NODE_WALL);
	}
	CCSprite* addNode = CCSprite::create(nodeTypeStr[NODE_WALL]);
	//CCPoint addlocation = PositionOfItem(touch_Node->getRow(), touch_Node->getCol());
	addNode->setPosition(CCPoint(touch_Node->getContentSize().width/2 - 0.5, touch_Node->getContentSize().height/2 + 0.5));
	touch_Node->addChild(addNode);

	return true;
}

//delete wall
bool PlayLayer::DeleteWall(Node* touch_Node)
{
	if(touch_Node->getNodeType() != NODE_WALL)
		return false;

	touch_Node->removeAllChildren();
	touch_Node->setNodeType(NODE_NORMAL);

	return true;
}
//set pos end

//AStarFind
int PlayLayer::AStarFindPath()
{
	if(!m_SourceNode || !m_DesNode)
	{
		setState(FIND_END);
		return RESULT_ERROR;
	}

	//first time
	if(getHasBegan() < 0)
	{
		Add2OpenList(m_SourceNode);
		setHasBegan(FIND_RUNNING);
	}

	//find min F
	Node* min_Node = FindMin_F_OpenList();
	if(!min_Node)
	{//can not find
		setState(FIND_END);
		return RESULT_NOT;
	}

	//add closelist
	if(Add2CloseList(min_Node) > 0)
	{// find des
		setState(FIND_END);
		return RESULT_GET;
	}
	//add neigthbor
	AddNeighbor2Openlist(min_Node);


}

void PlayLayer::Add2OpenList(Node* p_node)
{
	m_openList.push_back(p_node);
	CCSpriteFrame *frame;
	//CCSprite* p_find = CCSprite::create("green.png");
	frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(stepRes[1]);
	p_node->setDisplayFrame(frame);
	//p_node->setPosition(CCPoint(p_node->getContentSize().width/2 - 0.5, p_node->getContentSize().height/2 + 0.5));
	//p_node->addChild(p_find);
}

void PlayLayer::DeleteFromOpen(Node* pNode)
{
	if(!pNode)
		return;
	m_openList.remove(pNode);
}

void PlayLayer::AddNeighbor2Openlist(Node* p_node)
{
	if(!p_node)
		return;

	//right
	Node* neighborRight = NodeOfIndex(p_node->getRow(), p_node->getCol() + 1);
	if(CheckAddNeighborNode(neighborRight, p_node, DIR_RIGHT))
	{
		ComputeNodeGH(neighborRight, p_node, DIR_RIGHT);
		Add2OpenList(neighborRight);
	}
	//left
	Node* neighborLeft = NodeOfIndex(p_node->getRow(), p_node->getCol() - 1);
	if(CheckAddNeighborNode(neighborLeft, p_node, DIR_LEFT))
	{
		ComputeNodeGH(neighborLeft, p_node, DIR_LEFT);
		Add2OpenList(neighborLeft);
	}
	//up
	Node* neighborUp = NodeOfIndex(p_node->getRow() + 1, p_node->getCol());
	if(CheckAddNeighborNode(neighborUp, p_node, DIR_UP))
	{
		ComputeNodeGH(neighborUp, p_node, DIR_UP);
		Add2OpenList(neighborUp);
	}
	//down
	Node* neighborDown = NodeOfIndex(p_node->getRow() - 1, p_node->getCol() + 1);
	if(CheckAddNeighborNode(neighborDown, p_node, DIR_DOWN))
	{
		ComputeNodeGH(neighborDown, p_node, DIR_DOWN);
		Add2OpenList(neighborDown);
	}

	right up
	Node* neighborRightUp = NodeOfIndex(p_node->getRow() + 1, p_node->getCol() + 1);
	if(CheckAddNeighborNode(neighborRightUp, p_node, DIR_RIGHT_UP))
	{
		ComputeNodeGH(neighborRightUp, p_node, DIR_RIGHT_UP);
		Add2OpenList(neighborRightUp);
	}
	//left up
	Node* neighborLeftUp = NodeOfIndex(p_node->getRow() + 1, p_node->getCol() - 1);
	if(CheckAddNeighborNode(neighborLeftUp, p_node, DIR_LEFT_UP))
	{
		ComputeNodeGH(neighborLeftUp, p_node, DIR_LEFT_UP);
		Add2OpenList(neighborLeftUp);
	}
	//right down
	Node* neighborRightDown = NodeOfIndex(p_node->getRow() - 1, p_node->getCol() + 1);
	if(CheckAddNeighborNode(neighborRightDown, p_node, DIR_RIGHT_DOWN))
	{
		ComputeNodeGH(neighborRightDown, p_node, DIR_RIGHT_DOWN);
		Add2OpenList(neighborRightDown);
	}
	//left down
	Node* neighborLeftDown = NodeOfIndex(p_node->getRow() - 1, p_node->getCol() - 1);
	if(CheckAddNeighborNode(neighborLeftDown, p_node, DIR_LEFT_DOWN))
	{
		ComputeNodeGH(neighborLeftDown, p_node, DIR_LEFT_DOWN);
		Add2OpenList(neighborLeftDown);
	}

}

void PlayLayer::ComputeNodeGH(Node* pNode, Node* pFather, Direction_Int dir)
{
	if(!pNode || !pFather)
		return;

	pNode->setPFatherNode(pFather);

	pNode->setG(pFather->getG() + DirWayRight[dir]);
	
	int prow = pNode->getRow() - m_DesNode->getRow() > 0 ? (pNode->getRow() - m_DesNode->getRow()) :  (m_DesNode->getRow() - pNode->getRow());
	int pcol = pNode->getCol() - m_DesNode->getCol() > 0 ? (pNode->getCol() - m_DesNode->getCol()) :  (m_DesNode->getCol() - pNode->getCol());

	pNode->setH(prow + pcol);
}

bool PlayLayer::CheckAddNeighborNode(Node* neighbor_Node, Node* father_Node, Direction_Int dir)
{
	if(!neighbor_Node || !father_Node)
		return false;

	//check wall 
	if(neighbor_Node->getNodeType() == NODE_WALL)
		return false;

	//check close list
	Node_List::iterator its = m_closeList.begin();
	for(;its != m_closeList.end();its ++)
	{
		if(*its == neighbor_Node)
			return false;
	}
	//check open list
	Node_List::iterator it = m_openList.begin();
	for(;it != m_openList.end();it ++)
	{
		if(*it == neighbor_Node)
		{
			if((*it)->getG() > father_Node->getG() + DirWayRight[dir])
			{
				(*it)->setPFatherNode(father_Node);
				(*it)->setG(father_Node->getG() + DirWayRight[dir]);
			}
			return false;
		}
	}

	return true;
}

Node* PlayLayer::FindMin_F_OpenList()
{
	if(m_openList.empty())
		return NULL;

	int F = 0;
	Node* minNode = NULL;
	std::list<Node*>::iterator it = m_openList.begin();
	for(;it != m_openList.end();it ++)
	{
		if(F == 0)
		{
			F = (*it)->getG() + (*it)->getH();
			minNode = (*it);
		}
		else
		{
			if(F > ((*it)->getG() + (*it)->getH()))
			{
				F = ((*it)->getG() + (*it)->getH());
				minNode = *it;
			}
		}
	}

	return minNode;
}

int PlayLayer::Add2CloseList(Node* p_node)
{
	if(!p_node)
		return -1;

	m_closeList.push_back(p_node);
	//delet from open
	DeleteFromOpen(p_node);

	if(p_node == m_DesNode)
		return 1;

	//CCSprite* way_Node = CCSprite::create("green.png");
	//p_node->setPosition(CCPoint(p_node->getContentSize().width/2 - 0.5, p_node->getContentSize().height/2 + 0.5));
	//p_node->addChild(way_Node);
	CCSpriteFrame *frame;
	frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(stepRes[2]);
	p_node->setDisplayFrame(frame);

	return 0;
}

void PlayLayer::PrintWay()
{
	Node* findnode = m_DesNode;
	for(;findnode != NULL;)
	{
		findnode = findnode->getPFatherNode();
		if(!findnode)
			break;
		CCSpriteFrame *frame;
		frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(stepRes[3]);
		findnode->setDisplayFrame(frame);
	}
	setState(FIND_PRINT);
}