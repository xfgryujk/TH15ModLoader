// 加载、卸载MOD

#pragma once
#include <string>
#include <vector>
#include <memory>


namespace tml
{
	class Mod
	{
	public:
		std::string m_path;
		HMODULE m_module;
	};

	class ModManager final
	{
	private:
		std::vector<std::unique_ptr<Mod> > m_mods;

		ModManager() = default;
		~ModManager();

	public:
		static ModManager& GetInstance()
		{
			static ModManager s_instance;
			return s_instance;
		}

		bool LoadMod(LPCSTR path);
		void LoadDir(LPCSTR dir);
		bool UnloadMod(int index);
		void UnloadAll();
	};
}
