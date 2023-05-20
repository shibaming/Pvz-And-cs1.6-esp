#pragma once
#include "imgui/imgui.h"
#include "class.h"
class Esp
{
public:
	Esp(Rect screenSize);
	Esp() :m_matrix{ 0.f }, col(0.f, 1.f, 0.f){};
	~Esp() {};
	void DrawLine(const ImVec2& p1, const ImVec2& p2);
	void DrawRect(const ImVec2& p_min, const ImVec2& p_max);
	void DrawBox(Vec3 pos);
	void DrawText(ImVec2 pos, const char* text);
	void GetMatrix(HANDLE hProcess);
	ImVec2 WorldToScreen(Vec3  playercoord);
	Rect m_screenSize;
private:
	float m_matrix[16];
	ImColor col;

};

