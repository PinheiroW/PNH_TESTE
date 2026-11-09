// PNH_Core v0.10.3. Generic active journal rotation preflight, READ ONLY.
// Checks completed life sequence, archive semantic equality, and backup conflicts.
// NO CopyFile, NO DeleteFile, NO SaveFile for player data.
class PNH_LifeCycleRotationCoordinator
{
    protected static const string ROOT = "$profile:PNH";
    protected static const string CORE = "$profile:PNH/Core";
    protected static const string CONFIG = "$profile:PNH/Core/LifeCycleRotationCoordinator.json";
    protected static bool s_Enabled = false;
    protected static int s_MaxHistoryChecks = 64;

    static bool Initialize()
    {
        s_Enabled = false;
        s_MaxHistoryChecks = 64;
        if (!GetGame() || !GetGame().IsServer()) return false;
        if (!FileExist(ROOT)) MakeDirectory(ROOT);
        if (!FileExist(CORE)) MakeDirectory(CORE);
        ref PNH_LifeCycleRotationCoordinatorConfig cfg = new PNH_LifeCycleRotationCoordinatorConfig();
        string err;
        if (!FileExist(CONFIG))
        {
            if (JsonFileLoader<PNH_LifeCycleRotationCoordinatorConfig>.SaveFile(CONFIG, cfg, err))
                Print("[PNH_CORE][LRC] LifeCycleRotationCoordinator.json created (OFF)");
            else
                Print("[PNH_CORE][LRC][ERROR] Cannot create config: " + err);
            return false;
        }
        if (!JsonFileLoader<PNH_LifeCycleRotationCoordinatorConfig>.LoadFile(CONFIG, cfg, err) || !cfg || cfg.SchemaVersion != 1 || cfg.MaxHistoryChecks < 1 || cfg.MaxHistoryChecks > 256)
        {
            Print("[PNH_CORE][LRC][ERROR] Invalid config; OFF");
            return false;
        }
        s_Enabled = cfg.Enabled;
        s_MaxHistoryChecks = cfg.MaxHistoryChecks;
        Print("[PNH_CORE][LRC] Enabled=" + s_Enabled.ToString() + " mode=READ_ONLY maxHistoryChecks=" + s_MaxHistoryChecks.ToString());
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
            Print("[PNH_CORE][LRC][WARN] INVALID_ID NO_WRITE");
            return;
        }
        string suffix = id.Substring(13, 4);
        string playerPath = "$profile:PNH/Players/" + id + ".json";
        string journalPath = "$profile:PNH/LifeJournals/" + id + ".json";
        string archiveDir = "$profile:PNH/LifeHistory/" + id;
        string err;
        if (!FileExist(playerPath))
        {
            Print("[PNH_CORE][LRC][WARN] PLAYER_MISSING steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        ref PNH_PlayerBindingRecord p = new PNH_PlayerBindingRecord();
        if (!JsonFileLoader<PNH_PlayerBindingRecord>.LoadFile(playerPath, p, err) || !p || p.SchemaVersion != 1 || p.SteamID64 != id || p.LifeId < 1 || p.Revision < 1 || !ValidState(p.State))
        {
            Print("[PNH_CORE][LRC][ERROR] PLAYER_INVALID steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        if (!FileExist(journalPath))
        {
            Print("[PNH_CORE][LRC] NO_ACTIVE_JOURNAL steamSuffix=" + suffix + " currentLife=" + p.LifeId.ToString() + " NO_WRITE");
            return;
        }
        ref PNH_TransitionRecord j = new PNH_TransitionRecord();
        if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(journalPath, j, err) || !j || j.SchemaVersion != 1 || j.SteamID64 != id || j.SourceLifeId < 1 || j.SourceRevision < 1 || !ValidState(j.SourceState))
        {
            Print("[PNH_CORE][LRC][ERROR] JOURNAL_INVALID steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        if (j.Phase != "COMMITTED")
        {
            Print("[PNH_CORE][LRC] JOURNAL_NOT_COMMITTED steamSuffix=" + suffix + " phase=" + j.Phase + " NO_WRITE");
            return;
        }
        if (j.SourceLifeId >= 2147483646 || j.SourceRevision >= 2147483646 || p.LifeId != j.SourceLifeId + 1 || p.Revision != j.SourceRevision + 1 || p.State != "SURVIVOR")
        {
            Print("[PNH_CORE][LRC][ERROR] LIFE_MISMATCH steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        if (j.SourceLifeId > s_MaxHistoryChecks)
        {
            Print("[PNH_CORE][LRC][WARN] HISTORY_LIMIT steamSuffix=" + suffix + " required=" + j.SourceLifeId.ToString() + " NO_WRITE");
            return;
        }
        for (int life = 1; life <= j.SourceLifeId; life++)
        {
            string oldPath = archiveDir + "/Life_" + life.ToString() + ".json";
            if (!FileExist(oldPath))
            {
                Print("[PNH_CORE][LRC][WARN] ARCHIVE_MISSING steamSuffix=" + suffix + " life=" + life.ToString() + " NO_WRITE");
                return;
            }
            ref PNH_TransitionRecord old = new PNH_TransitionRecord();
            if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(oldPath, old, err) || !old || old.SchemaVersion != 1 || old.SteamID64 != id || old.SourceLifeId != life || old.SourceRevision < 1 || !ValidState(old.SourceState) || old.Phase != "COMMITTED")
            {
                Print("[PNH_CORE][LRC][ERROR] ARCHIVE_INVALID steamSuffix=" + suffix + " life=" + life.ToString() + " NO_WRITE");
                return;
            }
            if (life == j.SourceLifeId && !Equivalent(old, j))
            {
                Print("[PNH_CORE][LRC][ERROR] ACTIVE_ARCHIVE_MISMATCH steamSuffix=" + suffix + " NO_WRITE");
                return;
            }
        }
        // Important: v0.9.2 used a single legacy .rotation.bak for life 1.
        // A generic writable rotator MUST use a per-life backup path instead.
        string perLifeBackup = journalPath + ".Life_" + j.SourceLifeId.ToString() + ".rotation.bak";
        if (FileExist(perLifeBackup))
        {
            ref PNH_TransitionRecord backupRecord = new PNH_TransitionRecord();
            if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(perLifeBackup, backupRecord, err) || !Equivalent(backupRecord, j))
            {
                Print("[PNH_CORE][LRC][ERROR] PER_LIFE_BACKUP_CONFLICT steamSuffix=" + suffix + " NO_WRITE");
                return;
            }
            Print("[PNH_CORE][LRC] ROTATION_READY steamSuffix=" + suffix + " event=" + eventName + " currentLife=" + p.LifeId.ToString() + " sourceLife=" + j.SourceLifeId.ToString() + " archives=" + j.SourceLifeId.ToString() + " backup=EXISTS_VERIFIED NO_WRITE");
            return;
        }
        Print("[PNH_CORE][LRC] ROTATION_READY steamSuffix=" + suffix + " event=" + eventName + " currentLife=" + p.LifeId.ToString() + " sourceLife=" + j.SourceLifeId.ToString() + " archives=" + j.SourceLifeId.ToString() + " backup=ABSENT NO_WRITE");
    }
};
