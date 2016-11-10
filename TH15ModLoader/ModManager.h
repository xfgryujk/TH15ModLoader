#pragma once
#include <vector>
#include <memory>


namespace ModManager
{
	class ModManager;
	extern ModManager g_modManager;


	class Mod
	{
	public:
		HMODULE m_module;
	};

	class ModManager final
	{
	public:
		~ModManager();

		bool LoadMod(LPCSTR path);
		bool LoadDir(LPCSTR dir);
		bool Unload(int index);
		bool UnloadAll();

	private:
		std::vector<std::unique_ptr<Mod> > m_mods;
	};
}
