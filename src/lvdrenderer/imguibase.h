
#ifndef _IMGUIBASE_H_
#define _IMGUIBASE_H_
namespace ysl
{
	namespace imgui
	{
		class ImGuiInterface
		{
		public:
			ImGuiInterface() = default;
			virtual void Draw() = 0;
			virtual ~ImGuiInterface() = default;
		};
	}
}

#endif
