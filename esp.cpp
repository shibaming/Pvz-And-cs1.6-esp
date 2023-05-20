#include "esp.h"

Esp::Esp(Rect screenSize)
{	
	m_screenSize = screenSize;
}

void Esp::DrawLine(const ImVec2& p1, const ImVec2& p2)
{	
	ImGui::GetForegroundDrawList()->AddLine(p1, p2, col);	
}

void Esp::DrawRect(const ImVec2& p_min, const ImVec2& p_max)
{
	ImGui::GetForegroundDrawList()->AddRect(p_min, p_max, col);
}

void Esp::DrawBox(Vec3 pos)
{
	//32,32,72	
	float length = 16.f, width = 16.f, height = 36.f;
	ImVec2 b1 = WorldToScreen({ pos.x - length,pos.y - width,pos.z - height });
	ImVec2 b2 = WorldToScreen({ pos.x - length,pos.y + width,pos.z - height });
	ImVec2 b3 = WorldToScreen({ pos.x + length,pos.y - width,pos.z - height });
	ImVec2 b4 = WorldToScreen({ pos.x + length,pos.y + width,pos.z - height });

	ImVec2 a1 = WorldToScreen({ pos.x - length,pos.y - width,pos.z + height });
	ImVec2 a2 = WorldToScreen({ pos.x - length,pos.y + width,pos.z + height });
	ImVec2 a3 = WorldToScreen({ pos.x + length,pos.y - width,pos.z + height });
	ImVec2 a4 = WorldToScreen({ pos.x + length,pos.y + width,pos.z + height });

	DrawLine(a1, a2);
	DrawLine(a2, a4);
	DrawLine(a4, a3);
	DrawLine(a3, a1);

	DrawLine(b1, b2);
	DrawLine(b2, b4);
	DrawLine(b4, b3);
	DrawLine(b3, b1);

	DrawLine(a1, b1);
	DrawLine(a2, b2);
	DrawLine(a3, b3);
	DrawLine(a4, b4);
}

void Esp::DrawText(ImVec2 pos,const char* text)
{
	ImGui::GetForegroundDrawList()->AddText(pos, col, text);
}

void Esp::GetMatrix(HANDLE hProcess)
{
	ReadProcessMemory(hProcess, (LPCVOID)0x2C20100, &m_matrix, sizeof m_matrix, nullptr);
}

ImVec2 Esp::WorldToScreen(Vec3 playercoord)
{
	ImVec4 clipCoords = {
			playercoord.x * m_matrix[0] + playercoord.y * m_matrix[4] + playercoord.z * m_matrix[8] + m_matrix[12],
			playercoord.x * m_matrix[1] + playercoord.y * m_matrix[5] + playercoord.z * m_matrix[9] + m_matrix[13],
			0.f,//playercoord.x * m_matrix[2] + playercoord.y * m_matrix[6] + playercoord.z * m_matrix[10] + m_matrix[14],
			playercoord.x * m_matrix[3] + playercoord.y * m_matrix[7] + playercoord.z * m_matrix[11] + m_matrix[15]
	};
	if (clipCoords.w < 0.f)
	{
		return { 0.f,0.f };
	}
	ImVec2 ndc = { clipCoords.x / clipCoords.w ,clipCoords.y / clipCoords.w };
	return { (1.0f + ndc.x) * 0.5f * m_screenSize.width() + m_screenSize.x(),(1.0f - ndc.y) * 0.5f * m_screenSize.height() + m_screenSize.y() };
}
