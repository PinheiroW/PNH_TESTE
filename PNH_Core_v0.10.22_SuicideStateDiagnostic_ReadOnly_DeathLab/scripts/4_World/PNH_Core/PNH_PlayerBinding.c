// v0.7 - opt-in SteamID64 account binding LAB, server-only.
// It NEVER changes player state, inventário, health, native save, or vanilla respawn.
// FIRST EVENT per account may create baseline if WriteEnabled=true. No life transition yet.
class PNH_PlayerBinding
{
    protected static const string ROOT = "$profile:PNH";
    protected static const string CORE = "$profile:PNH/Core";
    protected static const string PLAYERS = "$profile:PNH/Players";
    protected static const string SETTINGS = "$profile:PNH/Core/PlayerBinding.json";
    protected static bool s_Enabled = false;
    protected static bool s_WriteEnabled = false;

    static bool Initialize()
    {
        s_Enabled = false;
        s_WriteEnabled = false;
        if (!GetGame() || !GetGame().IsServer()) return false;

        if (!FileExist(ROOT)) MakeDirectory(ROOT);
        if (!FileExist(CORE)) MakeDirectory(CORE);

        ref PNH_PlayerBindingConfig cfg = new PNH_PlayerBindingConfig();
        string err;
        if (!FileExist(SETTINGS))
        {
            if (JsonFileLoader<PNH_PlayerBindingConfig>.SaveFile(SETTINGS, cfg, err))
                Print("[PNH_CORE][BIND] PlayerBinding.json created (OFF)");
            else
                Print("[PNH_CORE][BIND][ERROR] Cannot create config: " + err);
            return false;
        }
        if (!JsonFileLoader<PNH_PlayerBindingConfig>.LoadFile(SETTINGS, cfg, err))
        {
            Print("[PNH_CORE][BIND][ERROR] Invalid config; blocked: " + err);
            return false;
        }
        if (!cfg || cfg.SchemaVersion != 1)
        {
            Print("[PNH_CORE][BIND][ERROR] Unsupported config version; blocked");
            return false;
        }
        s_Enabled = cfg.Enabled;
        s_WriteEnabled = cfg.Enabled && cfg.WriteEnabled;
        Print("[PNH_CORE][BIND] Enabled=" + s_Enabled.ToString() + " WriteEnabled=" + s_WriteEnabled.ToString());
        if (s_WriteEnabled && !FileExist(PLAYERS)) MakeDirectory(PLAYERS);
        return s_Enabled;
    }

    // SteamID64 validation before using the value in a filesystem path.
    protected static bool ValidSteamId(string id)
    {
        if (id.Length() != 17) return false;
        string digits = "0123456789";
        for (int i = 0; i < id.Length(); i++)
        {
            if (digits.IndexOf(id.Substring(i, 1)) == -1) return false;
        }
        return true;
    }

    protected static bool ValidRecord(PNH_PlayerBindingRecord record, string steamId)
    {
        if (!record || record.SchemaVersion != 1) return false;
        if (record.SteamID64 != steamId) return false;
        if (record.LifeId < 1 || record.Revision < 1) return false;
        if (record.State != "SURVIVOR" && record.State != "PENDING" && record.State != "INFECTED" && record.State != "FINAL_DEAD") return false;
        return true;
    }

    static void Observe(string eventName, CF_EventArgs args)
    {
        if (!s_Enabled || !GetGame() || !GetGame().IsServer()) return;
        CF_EventPlayerArgs e = CF_EventPlayerArgs.Cast(args);
        if (!e)
        {
            Print("[PNH_CORE][BIND][WARN] " + eventName + " missing CF player args");
            return;
        }
        PlayerIdentity identity = e.Identity;
        if (!identity && e.Player) identity = e.Player.GetIdentity();
        if (!identity)
        {
            Print("[PNH_CORE][BIND][WARN] " + eventName + " identity unavailable");
            return;
        }
        // The only source is the server's PlayerIdentity, never a client RPC.
        string steamId = identity.GetPlainId();
        if (!ValidSteamId(steamId))
        {
            Print("[PNH_CORE][BIND][WARN] " + eventName + " unexpected SteamID format; no filesystem access");
            return;
        }
        // Only show suffix in logs. Complete ID appears in server-only filename by admin choice.
        string suffix = steamId.Substring(13, 4);
        string path = PLAYERS + "/" + steamId + ".json";
        if (!FileExist(path))
        {
            if (!s_WriteEnabled)
            {
                Print("[PNH_CORE][BIND] " + eventName + " steamSuffix=" + suffix + " baseline missing; DRY_RUN no write");
                return;
            }
            if (!FileExist(PLAYERS)) MakeDirectory(PLAYERS);
            ref PNH_PlayerBindingRecord fresh = new PNH_PlayerBindingRecord();
            fresh.SteamID64 = steamId;
            string writeError;
            if (!JsonFileLoader<PNH_PlayerBindingRecord>.SaveFile(path, fresh, writeError))
            {
                Print("[PNH_CORE][BIND][ERROR] Baseline write failed: " + writeError);
                return;
            }
            ref PNH_PlayerBindingRecord confirmed = new PNH_PlayerBindingRecord();
            if (!JsonFileLoader<PNH_PlayerBindingRecord>.LoadFile(path, confirmed, writeError) || !ValidRecord(confirmed, steamId))
            {
                Print("[PNH_CORE][BIND][ERROR] Baseline post-write validation failed; inspect file manually");
                return;
            }
            Print("[PNH_CORE][BIND] " + eventName + " steamSuffix=" + suffix + " BASELINE_CREATED life=1 state=SURVIVOR");
            return;
        }

        ref PNH_PlayerBindingRecord loaded = new PNH_PlayerBindingRecord();
        string readError;
        if (!JsonFileLoader<PNH_PlayerBindingRecord>.LoadFile(path, loaded, readError) || !ValidRecord(loaded, steamId))
        {
            Print("[PNH_CORE][BIND][ERROR] Existing record invalid; NEVER overwrite; " + eventName);
            return;
        }
        if (eventName == "CLIENT_NEW")
        {
            // An existing record plus CLIENT_NEW may mean an actual new life.
            // DO NOT inherit old infection or increment until verified life-transition protocol exists.
            Print("[PNH_CORE][BIND][WARN] CLIENT_NEW with existing account record; LIFE_TRANSITION_NOT_IMPLEMENTED; life=" + loaded.LifeId.ToString() + " state=" + loaded.State);
            return;
        }
        Print("[PNH_CORE][BIND] " + eventName + " steamSuffix=" + suffix + " RECORD_FOUND life=" + loaded.LifeId.ToString() + " state=" + loaded.State);
    }
};
