// PNH_Core v0.9 A - read-only preflight for multiple lives.
// Does not rotate journals, update LifeId, change vanilla persistence, or create player records.
class PNH_MultiLifePreflight
{
    protected static const string ROOT = "$profile:PNH";
    protected static const string CORE = "$profile:PNH/Core";
    protected static const string SETTINGS = "$profile:PNH/Core/MultiLifePreflight.json";
    protected static const string PLAYERS = "$profile:PNH/Players";
    protected static const string JOURNALS = "$profile:PNH/LifeJournals";
    protected static bool s_Enabled = false;

    static bool Initialize()
    {
        s_Enabled = false;
        if (!GetGame() || !GetGame().IsServer()) return false;
        if (!FileExist(ROOT)) MakeDirectory(ROOT);
        if (!FileExist(CORE)) MakeDirectory(CORE);
        ref PNH_MultiLifePreflightConfig cfg = new PNH_MultiLifePreflightConfig();
        string err;
        if (!FileExist(SETTINGS))
        {
            if (JsonFileLoader<PNH_MultiLifePreflightConfig>.SaveFile(SETTINGS, cfg, err))
                Print("[PNH_CORE][MULTI] MultiLifePreflight.json created (OFF)");
            else
                Print("[PNH_CORE][MULTI][ERROR] Cannot create config: " + err);
            return false;
        }
        if (!JsonFileLoader<PNH_MultiLifePreflightConfig>.LoadFile(SETTINGS, cfg, err) || !cfg || cfg.SchemaVersion != 1)
        {
            Print("[PNH_CORE][MULTI][ERROR] Invalid config; OFF");
            return false;
        }
        s_Enabled = cfg.Enabled;
        Print("[PNH_CORE][MULTI] Enabled=" + s_Enabled.ToString() + " mode=READ_ONLY");
        return s_Enabled;
    }

    protected static bool ValidId(string id)
    {
        if (id.Length() != 17) return false;
        string digits = "0123456789";
        for (int i = 0; i < 17; i++)
            if (digits.IndexOf(id.Substring(i, 1)) == -1) return false;
        return true;
    }

    static void OnCFEvent(string name, CF_EventArgs args)
    {
        if (!s_Enabled || !GetGame() || !GetGame().IsServer()) return;
        CF_EventPlayerArgs e = CF_EventPlayerArgs.Cast(args);
        if (!e) return;
        PlayerIdentity identity = e.Identity;
        if (!identity && e.Player) identity = e.Player.GetIdentity();
        if (!identity) return;
        string id = identity.GetPlainId();
        if (!ValidId(id))
        {
            Print("[PNH_CORE][MULTI][WARN] Invalid SteamID64; no action");
            return;
        }
        string suffix = id.Substring(13, 4);
        string playerPath = PLAYERS + "/" + id + ".json";
        string journalPath = JOURNALS + "/" + id + ".json";
        if (!FileExist(playerPath) || !FileExist(journalPath))
        {
            Print("[PNH_CORE][MULTI] MISSING_BASELINE_OR_JOURNAL steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        ref PNH_PlayerBindingRecord p = new PNH_PlayerBindingRecord();
        ref PNH_TransitionRecord j = new PNH_TransitionRecord();
        string err;
        if (!JsonFileLoader<PNH_PlayerBindingRecord>.LoadFile(playerPath, p, err) || !p || !JsonFileLoader<PNH_TransitionRecord>.LoadFile(journalPath, j, err) || !j)
        {
            Print("[PNH_CORE][MULTI][ERROR] JSON unreadable; blocked NO_WRITE");
            return;
        }
        if (p.SchemaVersion != 1 || j.SchemaVersion != 1 || p.SteamID64 != id || j.SteamID64 != id || p.LifeId < 1 || p.Revision < 1 || j.SourceLifeId < 1 || j.SourceRevision < 1)
        {
            Print("[PNH_CORE][MULTI][ERROR] Invalid record or mismatched identity; blocked NO_WRITE");
            return;
        }
        if (p.State != "SURVIVOR" && p.State != "PENDING" && p.State != "INFECTED" && p.State != "FINAL_DEAD")
        {
            Print("[PNH_CORE][MULTI][ERROR] Unknown player state; blocked NO_WRITE");
            return;
        }
        if (j.SourceState != "SURVIVOR" && j.SourceState != "PENDING" && j.SourceState != "INFECTED" && j.SourceState != "FINAL_DEAD")
        {
            Print("[PNH_CORE][MULTI][ERROR] Unknown journal state; blocked NO_WRITE");
            return;
        }
        if (j.Phase != "COMMITTED")
        {
            Print("[PNH_CORE][MULTI] TRANSITION_NOT_COMMITTED steamSuffix=" + suffix + " phase=" + j.Phase + " NO_WRITE");
            return;
        }
        if (j.SourceLifeId >= 2147483646 || j.SourceRevision >= 2147483646)
        {
            Print("[PNH_CORE][MULTI][ERROR] Integer limit; blocked NO_WRITE");
            return;
        }
        if (p.LifeId != j.SourceLifeId + 1 || p.Revision != j.SourceRevision + 1 || p.State != "SURVIVOR")
        {
            Print("[PNH_CORE][MULTI][ERROR] Committed source does not match current player; blocked NO_WRITE");
            return;
        }
        Print("[PNH_CORE][MULTI] NEXT_LIFE_ELIGIBLE steamSuffix=" + suffix + " event=" + name + " currentLife=" + p.LifeId.ToString() + " previousLife=" + j.SourceLifeId.ToString() + " archiveRequired=true NO_WRITE");
    }
};
