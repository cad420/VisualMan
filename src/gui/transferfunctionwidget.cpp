
#include "transferfunctionwidget.h"

namespace ysl
{
	namespace imgui
	{
		TransferFunctionWidget::TransferFunctionWidget(const std::string& fileName):
		TransferFunction(fileName)
		{
			
		}

		void TransferFunctionWidget::BindTexture(const std::shared_ptr<OpenGLTexture>& tex)
		{
			texFunc = tex;
			FetchData(funcData.data(), 256);
			texFunc->SetData(OpenGLTexture::RGBA32F,
				OpenGLTexture::RGBA,
				OpenGLTexture::Float32,
				256, 0, 0, funcData.data());
		}
		void TransferFunctionWidget::Draw()
		{
			ImGui::Begin("Transfer Function");

			// Draw Transfer function widgets
			ImDrawList * drawList = ImGui::GetWindowDrawList();
			const ImVec2 canvasPos(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
			const ImVec2 canvasSize(ImGui::GetContentRegionAvail().x, (std::min)(ImGui::GetContentRegionAvail().y, 255.0f));
			const ImVec2 canvasBottomRight(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y);
			drawList->AddRect(canvasPos, canvasBottomRight, IM_COL32_WHITE);

			ImGui::InvisibleButton("canvas", canvasSize);
			auto & io = ImGui::GetIO();
			static int selectedKeyIndex = -1;
			// Handle click and drag event

			if (ImGui::IsItemHovered())
			{
				auto xpos = ImGui::GetIO().MousePos.x, ypos = ImGui::GetIO().MousePos.y;
				if (ImGui::GetIO().MouseClicked[0])
				{
					lastMousePos = { io.MousePos.x - canvasPos.x, -io.MousePos.y + canvasPos.y + canvasSize.y };
				}
				if (ImGui::GetIO().MouseDown[0])		//
				{
					UpdateMappingKey({ lastMousePos.x,lastMousePos.y },
						{ io.MousePos.x - canvasPos.x,-io.MousePos.y + canvasPos.y + canvasSize.y },
						{ canvasSize.x,canvasSize.y });
					lastMousePos = { io.MousePos.x - canvasPos.x, 
						-io.MousePos.y + canvasPos.y + canvasSize.y };
				}
				if (ImGui::GetIO().MouseDoubleClicked[0])		// left button double click
				{
					const auto x = io.MousePos.x - canvasPos.x;
					const auto y = -io.MousePos.y + canvasPos.y + canvasSize.y;
					selectedKeyIndex = HitAt({ x,y }, { canvasSize.x,canvasSize.y });
					if (selectedKeyIndex != -1)
						spec = (*this)[selectedKeyIndex].leftColor;
				}

			}
			// Draw The transfer function 
			const auto keyCount = KeysCount();
			if (keyCount != 0)
			{
				ysl::Float sx = canvasSize.x, sy = canvasSize.y;
				auto p = KeyPosition(0, sx, sy);
				ImVec2 first{ p.x,p.y };
				p = KeyPosition(keyCount - 1, sx, sy);
				ImVec2 last{ p.x,p.y };

				ysl::Float x = canvasPos.x, y = canvasPos.y;
				// Draw the left-most horizontal line
				drawList->AddLine({ x,y + sy - first.y }, { first.x + x,sy - first.y + y }, IM_COL32_WHITE, 2);

				for (auto i = 0; i < keyCount - 1; i++)
				{
					const auto pos1 =KeyPosition(i, sx, sy);
					const auto pos2 = KeyPosition(i + 1, sx, sy);
					const auto c1 = (*this)[i].leftColor;
					const auto c2 = (*this)[i + 1].leftColor;
					drawList->AddCircleFilled({ x + pos1.x,y + sy - pos1.y }, 5.f, IM_COL32(c1[0] * 255, c1[1] * 255, c1[2] * 255,255));
					drawList->AddCircleFilled({ x + pos2.x,y + sy - pos2.y }, 5.f, IM_COL32(c2[0] * 255, c2[1] * 255, c2[2] * 255,255));
					drawList->AddLine({ x + pos1.x ,y + sy - pos1.y }, { x + pos2.x,y + sy - pos2.y }, IM_COL32_WHITE, 2);
				}
				//Draw The right-most horizontal line
				drawList->AddLine({ last.x + x,sy - last.y + y }, { x + canvasSize.x,y + sy - last.y }, IM_COL32_WHITE, 2);
			}
			/*TransferFunction Widget End*/
			ImGui::SetCursorScreenPos(ImVec2(canvasPos.x, canvasPos.y + canvasSize.y + 10));

			if (selectedKeyIndex != -1)
			{
				if (ImGui::ColorEdit4("Edit Color", (reinterpret_cast<float*>(&spec)), ImGuiColorEditFlags_Float));
				UpdateMappingKey(selectedKeyIndex, spec);
			}
			ImGui::End();
		}
	}
}