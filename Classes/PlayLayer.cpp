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

static const char* dirResource[] = 
{
	"arrow1.png",
	"arrow2.png",
	"arrow3.png",
	"arrow4.png",
	"arrow5.png",
	"arrow6.png",
	"arrow7.png",
	"arrow8.png",
};
//step resouce
const char* stepRes[]=
{
	"step1.png",
	"step2.png",
	"step3.png",
	"step4.png",
};

static const char* setWallButton = "setwall.png";
static const char* restartButton = "restart.png";
static const char* stepButton = "step.png";
static const char* runButton = "run.png";

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

PlayLayer::PlayLayer():m_width(0),m_height(0),m_IsTouchAble(true),m_State(FIND_NOT_START),m_hasBegan(-1),m_SourceNode(NULL),m_DesNode(NULL),m_DisplayStep(false),
	m_FoundPathSize(0)
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
	
	//init spriteSheet;
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("AStarPathFind.plist");
    spriteSheet = CCSpriteBatchNode::create("AStarPathFind.pvr.ccz");
    addChild(spriteSheet);


	//background
	CCSize WinSize =CCDirector::sharedDirector()->getWinSize();
	auto background = CCSprite::createWithSpriteFrameName("bg.png");
	background->setAnchorPoint(CCPoint(0, 1));
	background->setPosition(CCPoint(0, WinSize.height));
	addChild(background);

	

	m_width = MAX_WIDETH;
	m_height = MAX_HEIGTH;

	//
	setDisplayStep(true);
	//
	m_matrixLeftBottomX = (WinSize.width - Node::GetContentWidth() * m_width - (m_width - 1) * NODE_GAP) / 2;
    m_matrixLeftBottomY = (WinSize.height - Node::GetContentWidth() * m_height - (m_height - 1) * NODE_GAP) / 2;
	
	int arraySize = sizeof(Node*) * m_width * m_height;
	m_matrix = (Node**)malloc(arraySize);
	memset((void*)m_matrix, 0, arraySize);

	InitMatrix();

	InitButton();

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

void PlayLayer::InitButton()
{
	CCSprite* setWallB = CCSprite::createWithSpriteFrameName(setWallButton);
	setWallB->setPosition(CCPoint(40, 460));
	addChild(setWallB);
	m_SetWallButton = setWallB;

	CCSprite* restartB = CCSprite::createWithSpriteFrameName(restartButton);
	restartB->setPosition(CCPoint(100, 460));
	addChild(restartB);
	m_RestartButton = restartB;

	CCSprite* stepB = CCSprite::createWithSpriteFrameName(stepButton);
	stepB->setPosition(CCPoint(190, 460));
	addChild(stepB);
	m_StepButton= stepB;

	CCSprite* runB = CCSprite::createWithSpriteFrameName(runButton);
	runB->setPosition(CCPoint(250, 460));
	addChild(runB);
	m_RunButton= runB;
}

void PlayLayer::update(float dt)
{
	//if(m_SourceNode && m_DesNode && m_State != FIND_END && m_State != FIND_PRINT)
		//m_State = FIND_STEPBYSTEP;

	if(m_State == FIND_RUNNING)
	{//begin A Star
		int findResult = AStarFindPath();
	}
	
	if(m_State == FIND_END)
	{
		InitWay();
	}
	/*if(m_State == FIND_PRINT)
	{
		BeginStep();
	}*/
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
		if(touchedNode)
		{
			if(!m_SourceNode)
				bool scuess = SetSourcePos(touchedNode);
			if(m_SourceNode && !m_DesNode)
				bool sucess =SetDesPos(touchedNode);
			if(getState() == FIND_REDAY && m_SourceNode && m_DesNode)
				bool sucess = SetWallPos(touchedNode);
		}

		CCSprite* ButtonSprite = ButtonOfPoint(&location);
		if(ButtonSprite)
		{
			if(ButtonSprite == m_SetWallButton)
			{
				if(m_State != FIND_STEPBYSTEP && m_State != FIND_RUNNING)
					setState(FIND_REDAY);

			}
			if(ButtonSprite == m_RestartButton)
			{//clear
				setState(FIND_NOT_START);
				Node_List::iterator it = m_openList.begin();
				for(;it != m_openList.end();it ++)
				{
					CCSpriteFrame *frame;
					frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(stepRes[STEP_BASE]);
					(*it)->setDisplayFrame(frame);
					(*it)->removeAllChildren();
					(*it)->setPFatherNode(NULL);
				}
				m_openList.clear();

				Node_List::iterator its = m_closeList.begin();
				for(;its != m_closeList.end();its ++)
				{
					CCSpriteFrame *frame;
					frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(stepRes[STEP_BASE]);
					(*its)->setDisplayFrame(frame);
					(*its)->removeAllChildren();
					(*its)->setPFatherNode(NULL);
				}
				m_closeList.clear();

				for(int i = 0;i < m_height;i ++)
				{
					for(int l = 0;l < m_width;l ++)
					{
						if((*(m_matrix + i * m_width + l))->getNodeType() == NODE_WALL)
						{
							(*(m_matrix + i * m_width + l))->removeAllChildren();
							(*(m_matrix + i * m_width + l))->setNodeType(NODE_NORMAL);
						}
					}
				}
				//m_FoundPath.clear();

				m_SourceNode = NULL;
				m_DesNode = NULL;
				setHasBegan(FIND_NOT_START);
			}
			if(ButtonSprite == m_StepButton)
			{
				setState(FIND_STEPBYSTEP);
			}
			if(ButtonSprite == m_RunButton)
			{
				setState(FIND_RUNNING);
			}
		}
	}
	if(m_State == FIND_STEPBYSTEP)
	{
		AStarFindPath();
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

	CCSprite* addNode = CCSprite::createWithSpriteFrameName(nodeTypeStr[NODE_SOURCE]);
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

	CCSprite* addNode = CCSprite::createWithSpriteFrameName(nodeTypeStr[NODE_DES]);
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
		Add2OpenList(m_SourceNode,0);
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
	if(AddNeighbor2Openlist(min_Node))
	{
		setState(FIND_END);
		return RESULT_GET;
	}


}

bool PlayLayer::Add2OpenList(Node* p_node, Direction_Int dir)
{
	m_openList.push_back(p_node);
	if(getDisplayStep())
	{
		DisPlayFindPathStep(stepRes[STEP_OPEN], p_node, dir);
	}
	if(p_node == m_DesNode)
	{
		Add2CloseList(p_node);
		return true;
	}
	return false;
	//p_node->setPosition(CCPoint(p_node->getContentSize().width/2 - 0.5, p_node->getContentSize().height/2 + 0.5));
	//p_node->addChild(p_find);
}

void PlayLayer::DeleteFromOpen(Node* pNode)
{
	if(!pNode)
		return;
	m_openList.remove(pNode);
}

bool PlayLayer::AddNeighbor2Openlist(Node* p_node)
{
	if(!p_node)
		return false;

	//right
	Node* neighborRight = NodeOfIndex(p_node->getRow(), p_node->getCol() + 1);
	if(CheckAddNeighborNode(neighborRight, p_node, DIR_RIGHT))
	{
		ComputeNodeGH(neighborRight, p_node, DIR_RIGHT);
		if(Add2OpenList(neighborRight, DIR_RIGHT))
			return true;
	}
	//left
	Node* neighborLeft = NodeOfIndex(p_node->getRow(), p_node->getCol() - 1);
	if(CheckAddNeighborNode(neighborLeft, p_node, DIR_LEFT))
	{
		ComputeNodeGH(neighborLeft, p_node, DIR_LEFT);
		if(Add2OpenList(neighborLeft, DIR_LEFT))
			return true;
	}
	//up
	Node* neighborUp = NodeOfIndex(p_node->getRow() + 1, p_node->getCol());
	if(CheckAddNeighborNode(neighborUp, p_node, DIR_UP))
	{
		ComputeNodeGH(neighborUp, p_node, DIR_UP);
		if(Add2OpenList(neighborUp, DIR_UP))
			return true;
	}
	//down
	Node* neighborDown = NodeOfIndex(p_node->getRow() - 1, p_node->getCol());
	if(CheckAddNeighborNode(neighborDown, p_node, DIR_DOWN))
	{
		ComputeNodeGH(neighborDown, p_node, DIR_DOWN);
		if(Add2OpenList(neighborDown, DIR_DOWN))
			return true;
	}

	//right up
	Node* neighborRightUp = NodeOfIndex(p_node->getRow() + 1, p_node->getCol() + 1);
	if(CheckAddNeighborNode(neighborRightUp, p_node, DIR_RIGHT_UP))
	{
		ComputeNodeGH(neighborRightUp, p_node, DIR_RIGHT_UP);
		if(Add2OpenList(neighborRightUp, DIR_RIGHT_UP))
			return true;
	}
	//left up
	Node* neighborLeftUp = NodeOfIndex(p_node->getRow() + 1, p_node->getCol() - 1);
	if(CheckAddNeighborNode(neighborLeftUp, p_node, DIR_LEFT_UP))
	{
		ComputeNodeGH(neighborLeftUp, p_node, DIR_LEFT_UP);
		if(Add2OpenList(neighborLeftUp, DIR_LEFT_UP))
			return true;
	}
	//right down
	Node* neighborRightDown = NodeOfIndex(p_node->getRow() - 1, p_node->getCol() + 1);
	if(CheckAddNeighborNode(neighborRightDown, p_node, DIR_RIGHT_DOWN))
	{
		ComputeNodeGH(neighborRightDown, p_node, DIR_RIGHT_DOWN);
		if(Add2OpenList(neighborRightDown, DIR_RIGHT_DOWN))
			return true;
	}
	//left down
	Node* neighborLeftDown = NodeOfIndex(p_node->getRow() - 1, p_node->getCol() - 1);
	if(CheckAddNeighborNode(neighborLeftDown, p_node, DIR_LEFT_DOWN))
	{
		ComputeNodeGH(neighborLeftDown, p_node, DIR_LEFT_DOWN);
		if( Add2OpenList(neighborLeftDown, DIR_LEFT_DOWN))
			return true;
	}

	return false;
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
	if(getDisplayStep())
	{
		DisPlayFindPathStep(stepRes[STEP_CLOSE], p_node);
	}

	return 0;
}

//init path
void PlayLayer::InitWay()
{
	//m_FoundPath.clear();

	Node* findnode = m_DesNode;
	//m_FoundPath.push_back(m_DesNode);
	for(;findnode != NULL;)
	{
		findnode = findnode->getPFatherNode();
		if(!findnode)
			break;

		//m_FoundPath.push_back(findnode);
		CCSpriteFrame *frame;
		frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(stepRes[STEP_PATH]);
		findnode->setDisplayFrame(frame);
	}
	//m_FoundPathSize = m_FoundPath.size();
	setState(FIND_PRINT);
}

//displaystep
void PlayLayer::DisPlayFindPathStep(const char* resouceStr, Node* stepNode, Direction_Int dir)
{
	//方向转换
	int desdir = 0;
	std::string dirstr;
	switch(dir)
	{
	case DIR_RIGHT:
		//desdir = DIR_LEFT;
		dirstr = "arrow2.png";
		break;
	case DIR_LEFT:
		//desdir = DIR_RIGHT;
		dirstr = "arrow1.png";
		break;
	case DIR_UP:
		//desdir = DIR_DOWN;
		dirstr = "arrow4.png";
		break;
	case DIR_DOWN:
		//desdir = DIR_UP;
		dirstr = "arrow3.png";
		break;
	case DIR_RIGHT_UP:
		//desdir = DIR_LEFT_DOWN;
		dirstr = "arrow8.png";
		break;
	case DIR_LEFT_UP:
		//desdir = DIR_RIGHT_DOWN;
		dirstr = "arrow7.png";
		break;
	case DIR_RIGHT_DOWN:
		//desdir = DIR_LEFT_UP;
		dirstr = "arrow6.png";
		break;
	case DIR_LEFT_DOWN:
		//desdir = DIR_RIGHT_UP;
		dirstr = "arrow5.png";
		break;
	default:
		break;
	}
	CCSpriteFrame *frame;
	frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(resouceStr);
	stepNode->setDisplayFrame(frame);

	if(stepNode != m_SourceNode && stepNode != m_DesNode)
	{
		std::string GStr = std::to_string(stepNode->getG());
		CCLabelTTF* plableG = CCLabelTTF::create(GStr.c_str(), "Arial", 10);
		plableG->setColor(ccc3(0,0,0));
		plableG->setPosition(CCPoint(stepNode->getContentSize().width - 9, stepNode->getContentSize().height - 7));
		stepNode->addChild(plableG);

		std::string HStr = std::to_string(stepNode->getH());
		CCLabelTTF* plableH = CCLabelTTF::create(HStr.c_str(), "Arial", 10);
		plableH->setColor(ccc3(0,0,0));
		plableH->setPosition(CCPoint(9, stepNode->getContentSize().height - 7));
		stepNode->addChild(plableH);

		std::string FStr = std::to_string(stepNode->getH() + stepNode->getG());
		CCLabelTTF* plableF = CCLabelTTF::create(FStr.c_str(), "Arial", 10);
		plableF->setColor(ccc3(0,0,0));
		plableF->setPosition(CCPoint(9, 7));
		stepNode->addChild(plableF);

		CCSprite* dirSpirit = CCSprite::createWithSpriteFrameName(dirstr.c_str());
		dirSpirit->setPosition(CCPoint(stepNode->getContentSize().width/2, stepNode->getContentSize().height/2));
		stepNode->addChild(dirSpirit);
	}
}

void PlayLayer::DisPlayFindPathStep(const char* resouceStr, Node* stepNode)
{
	CCSpriteFrame *frame;
	frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(resouceStr);
	stepNode->setDisplayFrame(frame);
}

void PlayLayer::BeginStep()
{
	if(m_FoundPathSize >= 2)
	{
		Node* wayNode = m_FoundPath[m_FoundPathSize -1];
		
		//last step
		wayNode->removeAllChildren();
		DisPlayFindPathStep(stepRes[STEP_PATH], wayNode);
			
		//new step
		Node* wayNode2 = m_FoundPath[m_FoundPathSize - 2];
		wayNode2->removeAllChildren();
		CCSprite* addNode = CCSprite::createWithSpriteFrameName(nodeTypeStr[NODE_SOURCE]);
		addNode->setPosition(CCPoint(wayNode2->getContentSize().width/2 - 0.5, wayNode2->getContentSize().height/2 + 0.5));
		wayNode2->addChild(addNode);

		m_FoundPathSize --;
	}

}

void PlayLayer::ReStartInit()
{
	//initmap

	//clear openlist

	//clear closelist

	//clear Foundpath
}

CCSprite* PlayLayer::ButtonOfPoint(CCPoint* point)
{
	Node* node = NULL;

	CCRect rect = CCRect(0, 0, 0, 0);

	rect.origin.x = m_SetWallButton->getPositionX() - (m_SetWallButton->getContentSize().width / 2);
	rect.origin.y = m_SetWallButton->getPositionY() - (m_SetWallButton->getContentSize().height / 2);
	rect.size = m_SetWallButton->getContentSize();
	if(rect.containsPoint(*point))
		return m_SetWallButton;

	rect.origin.x = m_RestartButton->getPositionX() - (m_RestartButton->getContentSize().width / 2);
	rect.origin.y = m_RestartButton->getPositionY() - (m_RestartButton->getContentSize().height / 2);
	rect.size = m_RestartButton->getContentSize();
	if(rect.containsPoint(*point))
		return m_RestartButton;

	rect.origin.x = m_StepButton->getPositionX() - (m_StepButton->getContentSize().width / 2);
	rect.origin.y = m_StepButton->getPositionY() - (m_StepButton->getContentSize().height / 2);
	rect.size = m_StepButton->getContentSize();
	if(rect.containsPoint(*point))
		return m_StepButton;
					
	rect.origin.x = m_RunButton->getPositionX() - (m_RunButton->getContentSize().width / 2);
	rect.origin.y = m_RunButton->getPositionY() - (m_RunButton->getContentSize().height / 2);
	rect.size = m_RunButton->getContentSize();
	if(rect.containsPoint(*point))
		return m_RunButton;

	return NULL;
	

	return NULL;
}