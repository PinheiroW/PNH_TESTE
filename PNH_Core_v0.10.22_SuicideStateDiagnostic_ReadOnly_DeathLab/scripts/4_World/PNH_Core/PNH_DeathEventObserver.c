// v0.10.15 - server-only death EVENT OBSERVER; no journal writes or player mutations.
// EEKilled observes a FINALIZED vanilla death; it is not an infected transformation trigger.
class PNH_DeathEventObserver
{
    protected static const string CONFIG = "$profile:PNH/Core/DeathEventObserver.json";
    protected static bool s_Enabled = false;

    static void Initialize()
    {
        s_Enabled = false;
        if (!GetGame() || !GetGame().IsServer()) return;
        if (!FileExist("$profile:PNH")) MakeDirectory("$profile:PNH");
        if (!FileExist("$profile:PNH/Core")) MakeDirectory("$profile:PNH/Core");
        ref PNH_DeathEventObserverConfig cfg = new PNH_DeathEventObserverConfig();
        string err;
        if (!FileExist(CONFIG))
        {
            if (JsonFileLoader<PNH_DeathEventObserverConfig>.SaveFile(CONFIG, cfg, err))
                Print("[PNH_CORE][DEO] DeathEventObserver.json created (OFF)");
            else
                Print("[PNH_CORE][DEO][ERROR] Config creation failed");
            return;
        }
        if (!JsonFileLoader<PNH_DeathEventObserverConfig>.LoadFile(CONFIG, cfg, err) || !cfg || cfg.SchemaVersion != 1)
        {
            Print("[PNH_CORE][DEO][ERROR] Invalid config; OFF");
            return;
        }
        s_Enabled = cfg.Enabled;
        Print("[PNH_CORE][DEO] Enabled=" + s_Enabled.ToString() + " mode=EVENT_OBSERVATION_ONLY hook=EEKilled journal=NONE commit=NONE NO_WRITE");
    }

    protected static bool ValidId(string id)
    {
        if (id.Length() != 17) return false;
        for (int i = 0; i < 17; i++)
            if ("0123456789".IndexOf(id.Substring(i, 1)) == -1) return false;
        return true;
    }

    static void OnPlayerKilled(PlayerBase player)
    {
        if (!s_Enabled || !GetGame() || !GetGame().IsServer() || !player) return;
        PlayerIdentity identity = player.GetIdentity();
        if (!identity)
        {
            Print("[PNH_CORE][DEO] EEKILLED_IDENTITY_UNAVAILABLE NO_WRITE");
            return;
        }
        string id = identity.GetPlainId();
        if (!ValidId(id))
        {
            Print("[PNH_CORE][DEO][WARN] EEKILLED_INVALID_ID NO_WRITE");
            return;
        }
        string suffix = id.Substring(13, 4);
        string playerPath = "$profile:PNH/Players/" + id + ".json";
        if (!FileExist(playerPath))
        {
            Print("[PNH_CORE][DEO] EEKILLED_PLAYER_RECORD_ABSENT steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        ref PNH_PlayerBindingRecord record = new PNH_PlayerBindingRecord();
        string err;
        if (!JsonFileLoader<PNH_PlayerBindingRecord>.LoadFile(playerPath, record, err) || !record)
        {
            Print("[PNH_CORE][DEO][WARN] EEKILLED_PLAYER_RECORD_INVALID steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        if (record.SteamID64 != id || record.SchemaVersion != 1)
        {
            Print("[PNH_CORE][DEO][WARN] EEKILLED_PLAYER_RECORD_MISMATCH steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        Print("[PNH_CORE][DEO] VANILLA_EEKILLED_OBSERVED steamSuffix=" + suffix + " life=" + record.LifeId.ToString() + " revision=" + record.Revision.ToString() + " state=" + record.State + " TRIGGER_NOT_AUTHORIZED NO_WRITE");
        PNH_DeathTransitionCandidate.OnFinalDeath(id, record);
    }
};
