#pragma once

#ifdef TH15MODLOADER_EXPORTS
#define TML_API __declspec(dllexport)
#else
#define TML_API __declspec(dllimport)
#endif
