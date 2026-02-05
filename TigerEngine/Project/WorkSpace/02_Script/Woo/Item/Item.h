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
    Ingredient(std::string id) : IItem(ItemType::Ingredient, id) {}
    
    /*
    * [ID]
        Ingredient_Apple
        Ingredient_Pear
        Ingredient_Batter
        Ingredient_Tofu
        Ingredient_Sanjeok
        Ingredient_Donggeurangttaeng
    */
};

// 지도 조각 Item
class Piece : public IItem
{
public:
    Piece(std::string id) : IItem(ItemType::Piece, id) {}
    /*
        [ID]
        TODO..
    */
};


// 완성된 음식 Item
class Food : public IItem
{
public:
    Food(std::string id) : IItem(ItemType::Food, id) {}
    
    /*
    * [ID]
        Apple
        Pear
        Batter
        Tofu
        Sanjeok
        Donggeurangttaeng
    */
};
