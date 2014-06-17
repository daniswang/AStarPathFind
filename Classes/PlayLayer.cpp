#include "PlayLayer.h"
#include "Node.h"

#define MAX_WIDETH  (7);
#define MAX_HEIGTH (9);
#define NODE_GAP (0.5)

static const char * nodeTypeStr[4] = 
{
	"nouse",
	"nodetype1.png",
	"nodetype2.png",
	"nodetype3.png",
};

PlayLayer::PlayLayer():m_width(0),m_height(0),m_IsTouchAble(true),m_State(0)
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
	//SpriteFrameCache::getInstance()->addSpriteFramesWithFile("sushi.plist");
    //spriteSheet = SpriteBatchNode::create("sushi.pvr.ccz");
    //addChild(spriteSheet);

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
	if(m_State == FIND_RUNNING)
	{//begin A Star
		AStarFindPath();
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

		//bool scuess = SetSourcePos(touchedNode);
		//bool sucess =SetDesPos(touchedNode);
		bool sucess = SetWallPos(touchedNode);
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
			if((*(m_matrix + i * m_width + l))->getInfo() == NODE_SOURCE)
			{
				(*(m_matrix + i * m_width + l))->removeAllChildren();
				(*(m_matrix + i * m_width + l))->setInfo(NODE_NORMAL);
				searched = true;
				break;
			}
			if(searched)
				break;
		}
	}
	//add new
	touch_Node->setInfo(NODE_SOURCE);

	CCSprite* addNode = CCSprite::create(nodeTypeStr[NODE_SOURCE]);
	CCPoint addlocation = PositionOfItem(touch_Node->getRow(), touch_Node->getCol());
	addNode->setPosition(CCPoint(touch_Node->getContentSize().width/2 - 0.5, touch_Node->getContentSize().height/2 + 0.5));
	touch_Node->addChild(addNode);

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
			if((*(m_matrix + i * m_width + l))->getInfo() == NODE_DES)
			{
				(*(m_matrix + i * m_width + l))->removeAllChildren();
				(*(m_matrix + i * m_width + l))->setInfo(NODE_NORMAL);
				searched = true;
				break;
			}
			if(searched)
				break;
		}
	}

	//add new
	touch_Node->setInfo(NODE_DES);

	CCSprite* addNode = CCSprite::create(nodeTypeStr[NODE_DES]);
	CCPoint addlocation = PositionOfItem(touch_Node->getRow(), touch_Node->getCol());
	addNode->setPosition(CCPoint(touch_Node->getContentSize().width/2 - 0.5, touch_Node->getContentSize().height/2 + 0.5));
	touch_Node->addChild(addNode);

	return true;
}

//set wall
bool PlayLayer::SetWallPos(Node* touch_Node)
{
	// 重复点击取消
	if(touch_Node->getInfo() == NODE_WALL)
	{
		DeleteWall(touch_Node);
		return true;
	}

	if(!touch_Node->IsNodeInfoNormal())
		return false;

	//add new
	if(touch_Node->getInfo() == NODE_NORMAL)
	{
		touch_Node->setInfo(NODE_WALL);
	}
	CCSprite* addNode = CCSprite::create(nodeTypeStr[NODE_WALL]);
	CCPoint addlocation = PositionOfItem(touch_Node->getRow(), touch_Node->getCol());
	addNode->setPosition(CCPoint(touch_Node->getContentSize().width/2 - 0.5, touch_Node->getContentSize().height/2 + 0.5));
	touch_Node->addChild(addNode);

	return true;
}

//delete wall
bool PlayLayer::DeleteWall(Node* touch_Node)
{
	if(touch_Node->getInfo() != NODE_WALL)
		return false;

	touch_Node->removeAllChildren();
	touch_Node->setInfo(NODE_NORMAL);

	return true;
}
//set pos end

//AStarFind
void PlayLayer::AStarFindPath()
{
}