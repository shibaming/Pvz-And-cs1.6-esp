#pragma once
#include <windows.h>
struct Vec3
{
    Vec3() :x(0), y(0), z(0) {}
    Vec3(float _x, float _y, float _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }
    float x;
    float y;
    float z;
};

struct Zombie
{
    Zombie():hp(0){}
    int hp;
    ImVec2 pos;
};
struct Rect : tagRECT
{
    float width()
    {
        return this->right - this->left;
    }
    float height()
    {
        return this->bottom - this->top;
    }
    float x()
    {
        return this->left;
    }
    float y()
    {
        return this->top;
    }
};


struct Person
{
    Vec3 pos;
    float Headupangle;
    float Rotationangle;
    bool isalive;
    int hp;
    int killcount;
    short flag;
};