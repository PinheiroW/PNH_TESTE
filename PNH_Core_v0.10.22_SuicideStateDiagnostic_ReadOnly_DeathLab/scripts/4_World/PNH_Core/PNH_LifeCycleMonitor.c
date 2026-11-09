// PNH_Core v0.8 - lifecycle observation LAB.
// Does not touch vanilla inventory, death, spawn, player state or account records.
// Correlation of EEKilled -> CLIENT_NEW is MEMORY-ONLY within the same server run.
// It is diagnostic evidence, NOT a permanent life identity or permission to increment LifeId.
class PNH_LifeCycleMonitor
{
    protected static const string ROOT = "$profile:PNH";
    protected static const string CORE = "$profile:PNH/Core";
    protected static const string SETTINGS = "$profile:PNH/Core/LifeCycleMonitor.json";
    protected static bool s_Enabled = false;
    protected static ref array<string> s_KilledAccounts;

    static bool Initialize()
    {
        s_Enabled = false;
        s_KilledAccounts = new array<string>();
        if (!GetGame() || !GetGame().IsServer()) return false;
        if (!FileExist(ROOT)) MakeDirectory(ROOT);
        if (!FileExist(CORE)) MakeDirectory(CORE);

        ref PNH_LifeCycleMonitorConfig config = new PNH_LifeCycleMonitorConfig();
        string error;
        if (!FileExist(SETTINGS))
        {
            if (JsonFileLoader<PNH_LifeCycleMonitorConfig>.SaveFile(SETTINGS, config, error))
                Print("[PNH_CORE][LIFEV2] LifeCycleMonitor.json created (OFF)");
            else
                Print("[PNH_CORE][LIFEV2][ERROR] Cannot create config: " + error);
            return false;
        }
        if (!JsonFileLoader<PNH_LifeCycleMonitorConfig>.LoadFile(SETTINGS, config, error) || !config || config.SchemaVersion != 1)
        {
            Print("[PNH_CORE][LIFEV2][ERROR] Invalid config; monitor OFF");
            return false;
        }
        s_Enabled = config.Enabled;
        Print("[PNH_CORE][LIFEV2] Enabled=" + s_Enabled.ToString() + " mode=OBSERVE_ONLY");
        return s_Enabled;
    }

    static bool IsEnabled()
    {
        return s_Enabled;
    }

    protected static bool GetSteamId(PlayerIdentity identity, out string id)
    {
        id = "";
        if (!identity) return false;
        id = identity.GetPlainId();
        if (id.Length() != 17) return false;
        string digits = "0123456789";
        for (int i = 0; i < 17; i++)
        {
            if (digits.IndexOf(id.Substring(i, 1)) == -1) return false;
        }
        return true;
    }

    // Called from the vanilla death callback; no changes to character or JSON.
    static void OnKilled(PlayerBase player, PlayerIdentity identity)
    {
        if (!s_Enabled || !GetGame() || !GetGame().IsServer()) return;
        string steamId;
        if (!GetSteamId(identity, steamId))
        {
            Print("[PNH_CORE][LIFEV2][WARN] EEKilled observed without usable SteamID64; no correlation");
            return;
        }
        if (s_KilledAccounts.Find(steamId) == -1)
            s_KilledAccounts.Insert(steamId);
        Print("[PNH_CORE][LIFEV2] DEATH_OBSERVED steamSuffix=" + steamId.Substring(13, 4) + " pendingNew=true");
    }

    // CF server event: do not equate New with death without additional evidence.
    static void OnCFEvent(string eventName, CF_EventArgs args)
    {
        if (!s_Enabled || !GetGame() || !GetGame().IsServer()) return;
        CF_EventPlayerArgs data = CF_EventPlayerArgs.Cast(args);
        if (!data)
        {
            Print("[PNH_CORE][LIFEV2][WARN] " + eventName + " missing CF args");
            return;
        }
        PlayerIdentity identity = data.Identity;
        if (!identity && data.Player) identity = data.Player.GetIdentity();
        string steamId;
        if (!GetSteamId(identity, steamId))
        {
            Print("[PNH_CORE][LIFEV2][WARN] " + eventName + " no valid SteamID64");
            return;
        }
        string suffix = steamId.Substring(13, 4);
        if (eventName == "CLIENT_NEW")
        {
            int found = s_KilledAccounts.Find(steamId);
            if (found >= 0)
            {
                s_KilledAccounts.Remove(found);
                Print("[PNH_CORE][LIFEV2] CLIENT_NEW_AFTER_DEATH steamSuffix=" + suffix + " correlated=true NO_WRITE");
            }
            else
            {
                Print("[PNH_CORE][LIFEV2] CLIENT_NEW_UNCONFIRMED steamSuffix=" + suffix + " correlated=false NO_WRITE");
            }
            return;
        }
        // CLIENT_READY includes reconnections. It must never create a life.
        Print("[PNH_CORE][LIFEV2] CLIENT_READY_EXISTING steamSuffix=" + suffix + " NO_WRITE");
    }
};
