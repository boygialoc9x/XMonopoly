#include "Map.h"
#include "Arena/Arena.h"
#include "Arena/Property/Property.h"
#include "Arena/SpecialArena/SpecialArena.h"

#include "GameMaster/GameMaster.h"

MapManager* MapManager::sp_pInstance = nullptr;
//cocos2d::TMXTiledMap* MapManager::p_pTileMap = nullptr;

///// MAP MANAGER

//Constructor

MapManager::MapManager() :
p_pTileMap(nullptr),
p_pArenaGroup(nullptr),
p_pClientPlayer(nullptr)
{
    p_vArenas.reserve(2);
}

//Public

bool MapManager::init()
{
	if(!Node::init()) return false;
	auto ls = EventListenerTouchOneByOne::create();
	ls->onTouchBegan = CC_CALLBACK_2(MapManager::onTouch, this);
	ls->onTouchEnded = CC_CALLBACK_2(MapManager::endTouch, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(ls, this);
	this->scheduleUpdate();
	return true;
}

cocos2d::TMXTiledMap* MapManager::getTileMap()
{
	return p_pTileMap;
}
cocos2d::TMXLayer* MapManager::getLayer(std::string sLayerId)
{
	auto it = p_mLayers.find(sLayerId);
	if(it == p_mLayers.end())
	{
		return nullptr;
	}
	return it->second;
}
void MapManager::loadTileMap(std::string sMapName)
{
	//Load the global tile map
	this->p_pTileMap = cocos2d::TMXTiledMap::create(sMapName);

	//Load the Arena group
	p_pArenaGroup = p_pTileMap->getObjectGroup("arena");

	//Load the tile layer
	LOAD_LAYER(p_pTileMap, metaLayer, p_mLayers);
	LOAD_LAYER(p_pTileMap, backgroundLayer, p_mLayers);

	//Calculate Angle Horizon
    p_fAngleHorizon = atan(this->p_pTileMap->getTileSize().height/this->p_pTileMap->getTileSize().width);
    p_fAngleVertical = PI/2 - p_fAngleHorizon;

	//Calculate the tile's size
	auto world = p_pTileMap->getObjectGroup("border")->getObject("object_tile_size");
	p_objectTileSize = Size(world["width"].asFloat(), world.at("height").asFloat());
	p_world = Size(p_objectTileSize.width * p_pTileMap->getMapSize().width, p_objectTileSize.height * p_pTileMap->getMapSize().height);

	//Calculate the true tile's object size
	auto sizeT = p_pTileMap->getTileSize().height/(2*sin(p_fAngleHorizon));
	p_tObjectTile = Size(sizeT, sizeT);
	p_tWorld = getTrueObjectSize(Size(p_world));

	//Self config
	this->config();
	//Generate the map's arena
	//this->generateArenas();
	//_eventDispatcher->addEventListenerWithSceneGraphPriority(ls, this);
	//auto button = ui::Button::create("NULL/null.png");
	//button->getEventDispatcher()->addEventListenerWithSceneGraphPriority(ls, button);
	//this->addChild(button);
}

void MapManager::setScale(cocos2d::Point cScale)
{
	cScale = ZYDirector::getInstance()->recalculateScale(cScale);

	this->p_pTileMap->setMapSize(Size(p_pTileMap->getMapSize().width*cScale.x, p_pTileMap->getMapSize().height*cScale.y));
    p_objectTileSize.width *= cScale.x;
    p_objectTileSize.height *= cScale.y;
}

void MapManager::setAnchorPoint(cocos2d::Point anchor)
{
	this->p_pTileMap->setAnchorPoint(anchor);
}

Point MapManager::getPosition()
{
	return this->p_pTileMap->getPosition();
}

void MapManager::config()
{
	this->addChild(p_pTileMap, 0);
	//this->setPosition(Point(x->getPositionX(), (ZYDirector::getInstance()->getTrueVisibleSize().height + x->getContentPositionMiddleTop().y )/2));
	this->p_pTileMap->setPosition(Point(ZYDR_GVS.width/2, ZYDR_GVS.height - p_pTileMap->getContentSize().height/2));

	this->setScale(Point(ZYDirector::getInstance()->getContentScaleWidth(), ZYDirector::getInstance()->getContentScaleHeight()));
	this->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
}

void MapManager::setPosition(Point pos)
{
	this->p_pTileMap->setPosition(pos);
}

cocos2d::Size MapManager::getMapPixelSize()
{
	return cocos2d::Size(this->p_pTileMap->getMapSize().width * this->p_pTileMap->getTileSize().width, this->p_pTileMap->getMapSize().height * this->p_pTileMap->getTileSize().height);
}

void MapManager::log()
{
	ZY_EZ_LOG;
}

std::string MapManager::toString(int nTab)
{
	std::string ts;
	std::string tab = ZY_SP_TAB(nTab);
	std::string tab1 = ZY_SP_TAB(nTab + 1)

	std::string layerToString;
	for (auto it = p_mLayers.begin(); it != p_mLayers.end(); it++)
	{
		layerToString += (tab1 + " > " + it->second->getLayerName());
	}

	ts += (tab + " + Position: " + ZYSP_VTS(p_pTileMap->getPosition()));
	ts += (tab + " + Size in Pixel: " + ZYSP_STS(getMapPixelSize()));
	ts += (tab + " + Size in Tile: " + ZYSP_STS(p_pTileMap->getMapSize()));
	ts += (tab + " + Tile Size: " + ZYSP_STS(p_pTileMap->getTileSize()));
	ts += (tab + " + Layer Map: " + ZYSP_NTS(p_mLayers.size()) + layerToString);
	ts += (tab + " + Angle Horizon: " + ZYSP_NTS(p_fAngleHorizon) + " (" + ZYSP_NTS(ZY_SUPPORT_GI->radToDegree(p_fAngleHorizon)) + ")");
	ts += (tab + " + Angle Vertical: " + ZYSP_NTS(p_fAngleVertical) + " (" + ZYSP_NTS(ZY_SUPPORT_GI->radToDegree(p_fAngleVertical)) + ")");
	ts += (tab + " + Object Tile Size: " + ZYSP_STS(p_objectTileSize) + "- True Object Tile Size: " + ZYSP_STS(p_tObjectTile));
	ts += (tab + " + Object World Size: " + ZYSP_STS(p_world) + "- True World Size: " + ZYSP_STS(p_tWorld));

	return ts;
}

void MapManager::loadMapFromFile(std::string sFileName)
{

}

void MapManager::update(float dt)
{
}

Size MapManager::getTrueObjectSize(const float& width, const float& height)
{
	Size size(width, height);
	return this->getTrueObjectSize(size);
}

Size MapManager::getTrueObjectSize(const Size& objectSize)
{
    Size size(objectSize.width/p_objectTileSize.width*p_tObjectTile.width,
                objectSize.height/p_objectTileSize.height*p_tObjectTile.height);
    return size;
}

Point MapManager::getTrueObjectPoint(const Point& objectPoint)
{
	return getTrueObjectPoint(objectPoint.x, objectPoint.y);
}

Point MapManager::getTrueObjectPoint(const float& xx, const float &yy)
{
	auto s = getTrueObjectSize(xx, yy);
	auto yyy = sin(p_fAngleHorizon) * (p_tWorld.width - s.width + s.height) + (p_pTileMap->getPositionY() - p_pTileMap->getContentSize().height/2);
	auto xxx = cos(p_fAngleHorizon) * (s.height + s.width) + p_pTileMap->getPositionX() - p_pTileMap->getContentSize().width/2;
	Point point(xxx, yyy);
	//CCLOG("MAP - %f", p_pTileMap->getPositionY() - p_pTileMap->getContentSize().height/2);
	return point;
}

void MapManager::generateArenas()
{
	auto obj = p_pArenaGroup->getObjects();
	if(!obj.empty())
	{
		for(auto i : obj)
		{
			ValueMap dict = i.asValueMap();
			if(dict.find("type") != dict.end())
			{
				if(dict.at("type").asString() == obj_property ) this->generatePropertyArenas(dict);
				else if(dict.at("type").asString() == obj_special ) this->generateSpecialArenas(dict);
			}
		}

		//// Sort Arena
		std::sort(p_vArenas.begin(), p_vArenas.end(), Arena::SortArena());
	}
}


void MapManager::generatePropertyArenas(ValueMap obj)
{
	std::string name = obj.at("name").asString();
	Coordinate coord((Dir)obj.at("dir").asInt(), obj.at("index").asInt());
	Size size = getTrueObjectSize(obj.at("width").asFloat(), obj.at("height").asFloat());
	Point point = getTrueObjectPoint(obj.at("x").asFloat(), obj.at("y").asFloat());
	float price = obj.at("price").asFloat();
	float sm = obj.at("sm").asFloat();
	float im = obj.at("im").asFloat();

	auto property = Property::createWithProperties(name, coord, size, point, price, sm, im);
	if(property)
	{
		this->addChild(property, 2);
		p_vArenas.push_back(property);
	}
}

void MapManager::generateSpecialArenas(ValueMap obj)
{
	std::string name = obj.at("name").asString();
	Coordinate coord((Dir)obj.at("dir").asInt(), obj.at("index").asInt());
	Size size = getTrueObjectSize(obj.at("width").asFloat(), obj.at("height").asFloat());
	Point point = getTrueObjectPoint(obj.at("x").asFloat(), obj.at("y").asFloat());

	auto sa = SpecialArenaFactory::getInstance()->create(name, name, coord, size, point);
	if(sa)
	{
		this->addChild(sa, 2);
		p_vArenas.push_back(sa);
	}
}
bool MapManager::onTouch(Touch *touch, Event *event)
{
	return true;
}

bool MapManager::endTouch(Touch *touch, Event *event)
{
	auto anchorPoint = this->getTileMap()->getPosition();
	auto touchPoint = touch->getLocation();
	for(int i = 0; i < p_vArenas.size(); i++)
	{
		if(SmartAlgorithm::isPointInside4Point(p_vArenas[i]->getLeftPoint(),
											   p_vArenas[i]->getTopPoint(),
											   p_vArenas[i]->getRightPoint(),
											   p_vArenas[i]->getBottomPoint(),
											   touchPoint))
		{
			p_pClientPlayer->setSelectObject(p_vArenas[i]);
			p_pClientPlayer->setSelectType(Player::SelectType::ARENA);
			return true;
		}
	}

	p_pClientPlayer->setSelectObject(nullptr);
	p_pClientPlayer->setSelectType(Player::SelectType::NONE);

	//if(touchPoint.y < anchorPoint.y)
	//{
	//	if(touchPoint.x <= anchorPoint.x)
	//	{
	//		for(int i = 0; i <= p_vArenas.size()/4; i++)
	//		{
	//			if(SmartAlgorithm::isPointInside4Point(p_vArenas[i]->getLeftPoint(),
	//												   p_vArenas[i]->getTopPoint(),
	//												   p_vArenas[i]->getRightPoint(),
	//												   p_vArenas[i]->getBottomPoint(),
	//												   touchPoint))
	//			{
	//				p_pClientPlayer->setSelectObject(p_vArenas[i]);
	//				p_pClientPlayer->setSelectType(Player::SelectType::ARENA);
	//				break;
	//			}
	//		}
	//	}
	//}

	return true;
}

Arena* MapManager::getArenaByCoord(Coordinate coord)
{
	return p_vArenas[coord.g_nIndex];
}