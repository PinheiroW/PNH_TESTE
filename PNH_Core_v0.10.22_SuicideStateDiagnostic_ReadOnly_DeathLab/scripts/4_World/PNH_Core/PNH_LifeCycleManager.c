// PNH_Core v0.10.0: READ-ONLY inspection of the persisted life chain.
// Does not write player records, journals, history or vanilla storage.
// The only file it can create is its disabled configuration on first startup.
class PNH_LifeCycleManager
{
    protected static const string ROOT = "$profile:PNH";
    protected static const string CORE = "$profile:PNH/Core";
    protected static const string CONFIG = "$profile:PNH/Core/LifeCycleManager.json";
    protected static bool s_Enabled = false;
    protected static int s_MaxHistoryChecks = 64;

    static bool Initialize()
    {
        s_Enabled = false;
        s_MaxHistoryChecks = 64;
        if (!GetGame() || !GetGame().IsServer()) return false;
        if (!FileExist(ROOT)) MakeDirectory(ROOT);
        if (!FileExist(CORE)) MakeDirectory(CORE);
        ref PNH_LifeCycleManagerConfig cfg = new PNH_LifeCycleManagerConfig();
        string err;
        if (!FileExist(CONFIG))
        {
            if (JsonFileLoader<PNH_LifeCycleManagerConfig>.SaveFile(CONFIG, cfg, err))
                Print("[PNH_CORE][LCM] LifeCycleManager.json created (OFF)");
            else
                Print("[PNH_CORE][LCM][ERROR] Cannot create config: " + err);
            return false;
        }
        if (!JsonFileLoader<PNH_LifeCycleManagerConfig>.LoadFile(CONFIG, cfg, err) || !cfg || cfg.SchemaVersion != 1 || cfg.MaxHistoryChecks < 1 || cfg.MaxHistoryChecks > 256)
        {
            Print("[PNH_CORE][LCM][ERROR] Invalid config; OFF");
            return false;
        }
        s_Enabled = cfg.Enabled;
        s_MaxHistoryChecks = cfg.MaxHistoryChecks;
        Print("[PNH_CORE][LCM] Enabled=" + s_Enabled.ToString() + " mode=READ_ONLY maxHistoryChecks=" + s_MaxHistoryChecks.ToString());
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

    protected static bool ValidState(string state)
    {
        return state == "SURVIVOR" || state == "PENDING" || state == "INFECTED" || state == "FINAL_DEAD";
    }

    protected static bool ValidJournal(PNH_TransitionRecord j, string id)
    {
        if (!j || j.SchemaVersion != 1 || j.SteamID64 != id || j.SourceLifeId < 1 || j.SourceRevision < 1 || !ValidState(j.SourceState)) return false;
        return j.Phase == "DEATH_RECORDED" || j.Phase == "NEW_OBSERVED" || j.Phase == "COMMITTED";
    }

    static void OnCFEvent(string eventName, CF_EventArgs args)
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
            Print("[PNH_CORE][LCM][WARN] INVALID_ID NO_WRITE");
            return;
        }
        string suffix = id.Substring(13, 4);
        string playerPath = "$profile:PNH/Players/" + id + ".json";
        string journalPath = "$profile:PNH/LifeJournals/" + id + ".json";
        if (!FileExist(playerPath))
        {
            Print("[PNH_CORE][LCM][WARN] PLAYER_RECORD_MISSING steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        ref PNH_PlayerBindingRecord p = new PNH_PlayerBindingRecord();
        string err;
        if (!JsonFileLoader<PNH_PlayerBindingRecord>.LoadFile(playerPath, p, err) || !p || p.SchemaVersion != 1 || p.SteamID64 != id || p.LifeId < 1 || p.Revision < 1 || !ValidState(p.State))
        {
            Print("[PNH_CORE][LCM][ERROR] INVALID_PLAYER_RECORD steamSuffix=" + suffix + " NO_WRITE");
            return;
        }

        int requiredArchives = p.LifeId - 1;
        string stage = "ARCHIVED_READY";
        if (FileExist(journalPath))
        {
            ref PNH_TransitionRecord j = new PNH_TransitionRecord();
            if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(journalPath, j, err) || !ValidJournal(j, id))
            {
                Print("[PNH_CORE][LCM][ERROR] INVALID_ACTIVE_JOURNAL steamSuffix=" + suffix + " NO_WRITE");
                return;
            }
            if (j.Phase == "COMMITTED")
            {
                if (p.LifeId != j.SourceLifeId + 1 || p.Revision != j.SourceRevision + 1 || p.State != "SURVIVOR")
                {
                    Print("[PNH_CORE][LCM][ERROR] COMMITTED_PLAYER_MISMATCH steamSuffix=" + suffix + " NO_WRITE");
                    return;
                }
                requiredArchives = p.LifeId - 2;
                stage = "COMMITTED_NEEDS_ARCHIVE";
            }
            else
            {
                if (p.LifeId != j.SourceLifeId || p.Revision != j.SourceRevision)
                {
                    Print("[PNH_CORE][LCM][ERROR] PENDING_PLAYER_MISMATCH steamSuffix=" + suffix + " NO_WRITE");
                    return;
                }
                stage = "TRANSITION_" + j.Phase;
            }
        }
        else if (p.LifeId == 1)
        {
            stage = "BASELINE_READY";
        }

        if (requiredArchives > s_MaxHistoryChecks)
        {
            Print("[PNH_CORE][LCM][WARN] HISTORY_SCAN_LIMIT steamSuffix=" + suffix + " required=" + requiredArchives.ToString() + " NO_WRITE");
            return;
        }
        for (int life = 1; life <= requiredArchives; life++)
        {
            string archivePath = "$profile:PNH/LifeHistory/" + id + "/Life_" + life.ToString() + ".json";
            if (!FileExist(archivePath))
            {
                Print("[PNH_CORE][LCM][WARN] ARCHIVE_MISSING steamSuffix=" + suffix + " life=" + life.ToString() + " NO_WRITE");
                return;
            }
            ref PNH_TransitionRecord a = new PNH_TransitionRecord();
            if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(archivePath, a, err) || !ValidJournal(a, id) || a.Phase != "COMMITTED" || a.SourceLifeId != life)
            {
                Print("[PNH_CORE][LCM][ERROR] ARCHIVE_INVALID steamSuffix=" + suffix + " life=" + life.ToString() + " NO_WRITE");
                return;
            }
        }
        Print("[PNH_CORE][LCM] LIFE_CHAIN_OK steamSuffix=" + suffix + " event=" + eventName + " currentLife=" + p.LifeId.ToString() + " revision=" + p.Revision.ToString() + " archivedCount=" + requiredArchives.ToString() + " stage=" + stage + " NO_WRITE");
    }
};
