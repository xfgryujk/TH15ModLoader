#include "stdafx.h"
#include "ModManager.h"


namespace tml
{
	ModManager g_modManager;


	ModManager::~ModManager()
	{
		UnloadAll();
	}

	bool ModManager::LoadMod(LPCSTR path)
	{
		std::unique_ptr<Mod> mod(new Mod);
		mod->m_module = LoadLibrary(path);
		if (mod->m_module == NULL)
			return false;
		
		m_mods.push_back(std::move(mod));
		return true;
	}

	bool ModManager::LoadDir(LPCSTR dir)
	{
		char fileName[MAX_PATH];
		strcpy_s(fileName, dir);
		strcat_s(fileName, "\\*.dll");

		WIN32_FIND_DATA findFileData;
		HANDLE find = FindFirstFile(fileName, &findFileData);
		if (find != INVALID_HANDLE_VALUE)
		{
			do
			{
				strcpy_s(fileName, dir);
				strcat_s(fileName, "\\");
				strcat_s(fileName, findFileData.cFileName);
				if (!LoadMod(fileName))
				{
					FindClose(find);
					return false;
				}
			} while (FindNextFile(find, &findFileData));
			FindClose(find);
		}

		return true;
	}

	bool ModManager::Unload(int index)
	{
		if (!FreeLibrary(m_mods[index]->m_module))
			return false;
		m_mods.erase(m_mods.begin() + index);
		return true;
	}

	bool ModManager::UnloadAll()
	{
		bool res = true;
		for (int i = (int)m_mods.size() - 1; i >= 0; --i)
			res = res && Unload(i);
		return res;
	}
}
