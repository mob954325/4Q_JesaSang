#pragma once
#include <iostream>
#include "Datas/EnumData.hpp"

/*
    [ IItem Interface ]
    음식재료, 지도조각 Item 인터페이스
    인벤토리에 추가됩니다.
*/

class IItem
{
public:
    ItemType itemType;
    std::string itemId;

    IItem(ItemType type, std::string id) 
        : itemType(type), itemId(id) {}
};

// 음식 재료 Item
class Ingredient : public IItem
{
public:
    Ingredient(ItemType type, std::string id) : IItem(type, id) {}
};

// 지도 조각 Item
class Piece : public IItem
{
public:
    Piece(ItemType type, std::string id) : IItem(type, id) {}
};
