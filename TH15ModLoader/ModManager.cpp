#include "stdafx.h"
#include "ModManager.h"
#include "ModEvent.h"


namespace tml
{
	ModManager::~ModManager()
	{
		UnloadAll();
	}

	bool ModManager::LoadMod(LPCSTR path)
	{
		_RPTF1(_CRT_WARN, "加载MOD：%s\n", path);

		std::unique_ptr<Mod> mod(new Mod);
		mod->m_path = path;
		mod->m_module = LoadLibrary(path);
		if (mod->m_module == NULL)
		{
			_RPTF1(_CRT_ERROR, "加载MOD失败：%s\n", path);
			return false;
		}
		
		m_mods.push_back(std::move(mod));
		return true;
	}

	void ModManager::LoadDir(LPCSTR dir)
	{
		WIN32_FIND_DATA findFileData;
		HANDLE find = FindFirstFile(((std::string)dir + "\\*.dll").c_str(), &findFileData);
		if (find != INVALID_HANDLE_VALUE)
		{
			do
				LoadMod(((std::string)dir + "\\" + findFileData.cFileName).c_str());
			while (FindNextFile(find, &findFileData));
			FindClose(find);
		}
	}

	bool ModManager::UnloadMod(int index)
	{
		_RPTF1(_CRT_WARN, "卸载MOD：%s\n", m_mods[index]->m_path.c_str());

		g_eventBus.DeleteListenersOfModule(m_mods[index]->m_module);
		if (!FreeLibrary(m_mods[index]->m_module))
			return false;
		m_mods.erase(m_mods.begin() + index);
		return true;
	}

	void ModManager::UnloadAll()
	{
		for (int i = (int)m_mods.size() - 1; i >= 0; --i)
			UnloadMod(i);
	}
}
