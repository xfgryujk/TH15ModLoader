#pragma once

#ifdef TH15MODLOADER_EXPORTS
#define THMOD_API __declspec(dllexport)
#else
#define THMOD_API __declspec(dllimport)
#endif
