#include <Core.h>
#if STEAM
#include <steam_api.h>
#endif

namespace SPF
{
	void ShowMessageBox(const char* title, const char* message, bool isError);

	namespace Steam
	{
#if STEAM
		class Callbacks
		{
		public:
			bool IsOverlayActive = false;
		private:
			STEAM_CALLBACK(Callbacks, OnGameOverlayActivated, GameOverlayActivated_t);
		};

		void Callbacks::OnGameOverlayActivated(GameOverlayActivated_t* pCallback) 
		{
			IsOverlayActive = pCallback->m_bActive;
		}

		struct
		{
			bool Initialized = false;
			Callbacks* Callbacks = nullptr;
		} SteamData;

#endif

		void Init(uint32_t appID)
		{
#if STEAM
			if (appID == 0)
				return; // No AppID = no Steam support

			if (SteamAPI_RestartAppIfNecessary(appID))
			{
				exit(1); // Let Steam restart the app
				return;
			}

			if (!SteamAPI_Init())
			{
				ShowMessageBox("Steam integration issue", 
					"The Steam integration failed: achievements will not be unlocked during gameplay!\nMake sure to launch the game from Steam.",
					true);
				return;
			}

			SteamData.Initialized = true;
			SteamData.Callbacks = new Callbacks();
			SteamUserStats()->RequestUserStats(SteamUser()->GetSteamID());
#endif
		}

		void Update()
		{
#if STEAM
			if (SteamData.Initialized)
			{
				SteamAPI_RunCallbacks();
			}
#endif
		}

		bool IsOverlayActive()
		{
#if STEAM
			return SteamData.Initialized && SteamData.Callbacks->IsOverlayActive;
#else
			return false;
#endif
		}

		void UnlockAchievement(const char* id)
		{
#if STEAM
			if (!SteamData.Initialized)
				return;

			const bool success1 = SteamUserStats()->SetAchievement(id);
			const bool success2 = SteamUserStats()->StoreStats();
			if (!success1 || !success2)
			{
				// TODO: feedback
			}
#endif
		}

		void Dispose()
		{
#if STEAM
			if (SteamData.Initialized)
			{
				delete SteamData.Callbacks;
				SteamData.Callbacks = nullptr;
				SteamAPI_Shutdown();
				SteamData.Initialized = false;
			}
#endif
		}
	}
}

extern "C"
{
	DLLExport void SPF_UnlockAchievement(const char* id)
	{
		SPF::Steam::UnlockAchievement(id);
	}
}