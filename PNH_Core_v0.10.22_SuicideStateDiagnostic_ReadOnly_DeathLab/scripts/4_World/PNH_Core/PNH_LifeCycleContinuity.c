// PNH_Core v0.10.5 - life continuity after protected journal rotation.
// READ ONLY. Never creates, copies or deletes player/journal/history/backup files.
// The only possible write is creation of the disabled config during first bootstrap.
class PNH_LifeCycleContinuity
{
    protected static const string ROOT = "$profile:PNH";
    protected static const string CORE = "$profile:PNH/Core";
    protected static const string CONFIG = "$profile:PNH/Core/LifeCycleContinuity.json";
    protected static bool s_Enabled = false;
    protected static int s_MaxHistoryChecks = 64;

    static bool Initialize()
    {
        s_Enabled = false;
        s_MaxHistoryChecks = 64;
        if (!GetGame() || !GetGame().IsServer()) return false;
        if (!FileExist(ROOT)) MakeDirectory(ROOT);
        if (!FileExist(CORE)) MakeDirectory(CORE);
        ref PNH_LifeCycleContinuityConfig cfg = new PNH_LifeCycleContinuityConfig();
        string err;
        if (!FileExist(CONFIG))
        {
            if (JsonFileLoader<PNH_LifeCycleContinuityConfig>.SaveFile(CONFIG, cfg, err))
                Print("[PNH_CORE][LCC] LifeCycleContinuity.json created (OFF)");
            else
                Print("[PNH_CORE][LCC][ERROR] Cannot create config: " + err);
            return false;
        }
        if (!JsonFileLoader<PNH_LifeCycleContinuityConfig>.LoadFile(CONFIG, cfg, err) || !cfg || cfg.SchemaVersion != 1 || cfg.MaxHistoryChecks < 1 || cfg.MaxHistoryChecks > 256)
        {
            Print("[PNH_CORE][LCC][ERROR] Invalid config; OFF");
            return false;
        }
        s_Enabled = cfg.Enabled;
        s_MaxHistoryChecks = cfg.MaxHistoryChecks;
        Print("[PNH_CORE][LCC] Enabled=" + s_Enabled.ToString() + " mode=READ_ONLY maxHistoryChecks=" + s_MaxHistoryChecks.ToString());
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

    protected static bool Equivalent(PNH_TransitionRecord a, PNH_TransitionRecord b)
    {
        if (!a || !b) return false;
        return a.SchemaVersion == b.SchemaVersion && a.SteamID64 == b.SteamID64 && a.SourceLifeId == b.SourceLifeId && a.SourceRevision == b.SourceRevision && a.SourceState == b.SourceState && a.Phase == b.Phase;
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
            Print("[PNH_CORE][LCC][WARN] INVALID_ID NO_WRITE");
            return;
        }
        string suffix = id.Substring(13, 4);
        string playerPath = "$profile:PNH/Players/" + id + ".json";
        string journalPath = "$profile:PNH/LifeJournals/" + id + ".json";
        string historyDir = "$profile:PNH/LifeHistory/" + id;
        string err;
        if (!FileExist(playerPath))
        {
            Print("[PNH_CORE][LCC][WARN] PLAYER_MISSING steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        ref PNH_PlayerBindingRecord p = new PNH_PlayerBindingRecord();
        if (!JsonFileLoader<PNH_PlayerBindingRecord>.LoadFile(playerPath, p, err) || !p || p.SchemaVersion != 1 || p.SteamID64 != id || p.LifeId < 1 || p.Revision < 1 || !ValidState(p.State))
        {
            Print("[PNH_CORE][LCC][ERROR] PLAYER_INVALID steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        if (p.LifeId < 2 || p.State != "SURVIVOR")
        {
            Print("[PNH_CORE][LCC][WARN] POST_ROTATION_NOT_APPLICABLE steamSuffix=" + suffix + " currentLife=" + p.LifeId.ToString() + " NO_WRITE");
            return;
        }
        int lastLife = p.LifeId - 1;
        if (lastLife > s_MaxHistoryChecks)
        {
            Print("[PNH_CORE][LCC][WARN] HISTORY_SCAN_LIMIT steamSuffix=" + suffix + " required=" + lastLife.ToString() + " NO_WRITE");
            return;
        }
        // A fresh active journal is not expected yet. Do not classify it as safe or modify it.
        if (FileExist(journalPath))
        {
            Print("[PNH_CORE][LCC][WARN] ACTIVE_JOURNAL_PRESENT steamSuffix=" + suffix + " currentLife=" + p.LifeId.ToString() + " INSPECT_MANUALLY NO_WRITE");
            return;
        }
        ref PNH_TransitionRecord mostRecent = null;
        int priorRevision = 0;
        for (int life = 1; life <= lastLife; life++)
        {
            string historyPath = historyDir + "/Life_" + life.ToString() + ".json";
            if (!FileExist(historyPath))
            {
                Print("[PNH_CORE][LCC][WARN] HISTORY_MISSING steamSuffix=" + suffix + " life=" + life.ToString() + " NO_WRITE");
                return;
            }
            ref PNH_TransitionRecord h = new PNH_TransitionRecord();
            if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(historyPath, h, err) || !h || h.SchemaVersion != 1 || h.SteamID64 != id || h.SourceLifeId != life || h.SourceRevision < 1 || !ValidState(h.SourceState) || h.Phase != "COMMITTED")
            {
                Print("[PNH_CORE][LCC][ERROR] HISTORY_INVALID steamSuffix=" + suffix + " life=" + life.ToString() + " NO_WRITE");
                return;
            }
            if (life > 1 && h.SourceRevision != priorRevision + 1)
            {
                Print("[PNH_CORE][LCC][ERROR] REVISION_CHAIN_MISMATCH steamSuffix=" + suffix + " life=" + life.ToString() + " NO_WRITE");
                return;
            }
            priorRevision = h.SourceRevision;
            mostRecent = h;
        }
        if (!mostRecent || mostRecent.SourceRevision >= 2147483646 || p.Revision != mostRecent.SourceRevision + 1)
        {
            Print("[PNH_CORE][LCC][ERROR] CURRENT_REVISION_MISMATCH steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        // For life 1 the legacy .rotation.bak may exist; v0.10.4's per-life naming
        // applies to the last completed life and is mandatory for this validation.
        string backupPath = journalPath + ".Life_" + lastLife.ToString() + ".rotation.bak";
        if (!FileExist(backupPath))
        {
            Print("[PNH_CORE][LCC][WARN] LAST_ROTATION_BACKUP_MISSING steamSuffix=" + suffix + " sourceLife=" + lastLife.ToString() + " NO_WRITE");
            return;
        }
        ref PNH_TransitionRecord backup = new PNH_TransitionRecord();
        if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(backupPath, backup, err) || !Equivalent(backup, mostRecent))
        {
            Print("[PNH_CORE][LCC][ERROR] BACKUP_HISTORY_MISMATCH steamSuffix=" + suffix + " sourceLife=" + lastLife.ToString() + " NO_WRITE");
            return;
        }
        Print("[PNH_CORE][LCC] CONTINUITY_READY steamSuffix=" + suffix + " event=" + eventName + " currentLife=" + p.LifeId.ToString() + " revision=" + p.Revision.ToString() + " archives=" + lastLife.ToString() + " activeJournal=ABSENT lastBackup=VERIFIED nextSourceLife=" + p.LifeId.ToString() + " NO_WRITE");
    }
};
