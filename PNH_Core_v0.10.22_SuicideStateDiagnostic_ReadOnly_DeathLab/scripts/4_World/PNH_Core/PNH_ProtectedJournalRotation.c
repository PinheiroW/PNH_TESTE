// PNH_Core v0.10.4. Protected journal rotation, explicit LAB authorization.
// Checks completed life sequence, archive semantic equality, and backup conflicts.
// Copy/Delete only after validation and explicit approval; no player record writes.
class PNH_ProtectedJournalRotation
{
    protected static const string ROOT = "$profile:PNH";
    protected static const string CORE = "$profile:PNH/Core";
    protected static const string CONFIG = "$profile:PNH/Core/ProtectedJournalRotation.json";
    protected static bool s_Enabled = false;
    protected static int s_MaxHistoryChecks = 64;
    protected static bool s_Rotate = false;
    protected static string s_ApprovedId = "";

    static bool Initialize()
    {
        s_Enabled = false;
        s_MaxHistoryChecks = 64;
        s_Rotate = false;
        s_ApprovedId = "";
        if (!GetGame() || !GetGame().IsServer()) return false;
        if (!FileExist(ROOT)) MakeDirectory(ROOT);
        if (!FileExist(CORE)) MakeDirectory(CORE);
        ref PNH_ProtectedJournalRotationConfig cfg = new PNH_ProtectedJournalRotationConfig();
        string err;
        if (!FileExist(CONFIG))
        {
            if (JsonFileLoader<PNH_ProtectedJournalRotationConfig>.SaveFile(CONFIG, cfg, err))
                Print("[PNH_CORE][PJR] ProtectedJournalRotation.json created (OFF)");
            else
                Print("[PNH_CORE][PJR][ERROR] Cannot create config: " + err);
            return false;
        }
        if (!JsonFileLoader<PNH_ProtectedJournalRotationConfig>.LoadFile(CONFIG, cfg, err) || !cfg || cfg.SchemaVersion != 1 || cfg.MaxHistoryChecks < 1 || cfg.MaxHistoryChecks > 256)
        {
            Print("[PNH_CORE][PJR][ERROR] Invalid config; OFF");
            return false;
        }
        s_Enabled = cfg.Enabled;
        s_MaxHistoryChecks = cfg.MaxHistoryChecks;
        s_ApprovedId = cfg.ApprovedSteamID64;
        s_Rotate = cfg.Enabled && cfg.RotationEnabled;
        if (s_Rotate && !ValidId(s_ApprovedId))
        {
            s_Rotate = false;
            Print("[PNH_CORE][PJR][ERROR] ApprovedSteamID64 missing; ROTATION DISABLED");
        }
        Print("[PNH_CORE][PJR] Enabled=" + s_Enabled.ToString() + " RotationEnabled=" + s_Rotate.ToString() + " mode=PROTECTED_COPY_DELETE_LAB maxHistoryChecks=" + s_MaxHistoryChecks.ToString());
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
            Print("[PNH_CORE][PJR][WARN] INVALID_ID NO_WRITE");
            return;
        }
        string suffix = id.Substring(13, 4);
        string playerPath = "$profile:PNH/Players/" + id + ".json";
        string journalPath = "$profile:PNH/LifeJournals/" + id + ".json";
        string archiveDir = "$profile:PNH/LifeHistory/" + id;
        string err;
        if (!FileExist(playerPath))
        {
            Print("[PNH_CORE][PJR][WARN] PLAYER_MISSING steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        ref PNH_PlayerBindingRecord p = new PNH_PlayerBindingRecord();
        if (!JsonFileLoader<PNH_PlayerBindingRecord>.LoadFile(playerPath, p, err) || !p || p.SchemaVersion != 1 || p.SteamID64 != id || p.LifeId < 1 || p.Revision < 1 || !ValidState(p.State))
        {
            Print("[PNH_CORE][PJR][ERROR] PLAYER_INVALID steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        if (!FileExist(journalPath))
        {
            Print("[PNH_CORE][PJR] NO_ACTIVE_JOURNAL steamSuffix=" + suffix + " currentLife=" + p.LifeId.ToString() + " NO_WRITE");
            return;
        }
        ref PNH_TransitionRecord j = new PNH_TransitionRecord();
        if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(journalPath, j, err) || !j || j.SchemaVersion != 1 || j.SteamID64 != id || j.SourceLifeId < 1 || j.SourceRevision < 1 || !ValidState(j.SourceState))
        {
            Print("[PNH_CORE][PJR][ERROR] JOURNAL_INVALID steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        if (j.Phase != "COMMITTED")
        {
            Print("[PNH_CORE][PJR] JOURNAL_NOT_COMMITTED steamSuffix=" + suffix + " phase=" + j.Phase + " NO_WRITE");
            return;
        }
        if (j.SourceLifeId >= 2147483646 || j.SourceRevision >= 2147483646 || p.LifeId != j.SourceLifeId + 1 || p.Revision != j.SourceRevision + 1 || p.State != "SURVIVOR")
        {
            Print("[PNH_CORE][PJR][ERROR] LIFE_MISMATCH steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        if (j.SourceLifeId > s_MaxHistoryChecks)
        {
            Print("[PNH_CORE][PJR][WARN] HISTORY_LIMIT steamSuffix=" + suffix + " required=" + j.SourceLifeId.ToString() + " NO_WRITE");
            return;
        }
        for (int life = 1; life <= j.SourceLifeId; life++)
        {
            string oldPath = archiveDir + "/Life_" + life.ToString() + ".json";
            if (!FileExist(oldPath))
            {
                Print("[PNH_CORE][PJR][WARN] ARCHIVE_MISSING steamSuffix=" + suffix + " life=" + life.ToString() + " NO_WRITE");
                return;
            }
            ref PNH_TransitionRecord old = new PNH_TransitionRecord();
            if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(oldPath, old, err) || !old || old.SchemaVersion != 1 || old.SteamID64 != id || old.SourceLifeId != life || old.SourceRevision < 1 || !ValidState(old.SourceState) || old.Phase != "COMMITTED")
            {
                Print("[PNH_CORE][PJR][ERROR] ARCHIVE_INVALID steamSuffix=" + suffix + " life=" + life.ToString() + " NO_WRITE");
                return;
            }
            if (life == j.SourceLifeId && !Equivalent(old, j))
            {
                Print("[PNH_CORE][PJR][ERROR] ACTIVE_ARCHIVE_MISMATCH steamSuffix=" + suffix + " NO_WRITE");
                return;
            }
        }
        // v0.9.2 used .rotation.bak for life 1. Use a distinct file for each life.
        string perLifeBackup = journalPath + ".Life_" + j.SourceLifeId.ToString() + ".rotation.bak";
        if (FileExist(perLifeBackup))
        {
            ref PNH_TransitionRecord prior = new PNH_TransitionRecord();
            if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(perLifeBackup, prior, err) || !Equivalent(prior, j))
            {
                Print("[PNH_CORE][PJR][ERROR] PER_LIFE_BACKUP_CONFLICT steamSuffix=" + suffix + " NO_WRITE");
                return;
            }
        }
        if (!s_Rotate)
        {
            string backupStatus = "ABSENT";
            if (FileExist(perLifeBackup)) backupStatus = "EXISTS_VERIFIED";
            Print("[PNH_CORE][PJR] ROTATION_READY steamSuffix=" + suffix + " event=" + eventName + " currentLife=" + p.LifeId.ToString() + " sourceLife=" + j.SourceLifeId.ToString() + " archives=" + j.SourceLifeId.ToString() + " backup=" + backupStatus + " NO_WRITE");
            return;
        }
        if (s_ApprovedId != id)
        {
            Print("[PNH_CORE][PJR] APPROVAL_REQUIRED steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        // Backup first: never overwrite a backup from a previous attempt.
        if (!FileExist(perLifeBackup))
        {
            if (!CopyFile(journalPath, perLifeBackup))
            {
                Print("[PNH_CORE][PJR][ERROR] BACKUP_COPY_FAILED steamSuffix=" + suffix + " SOURCE_UNTOUCHED");
                return;
            }
        }
        ref PNH_TransitionRecord verifiedBackup = new PNH_TransitionRecord();
        if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(perLifeBackup, verifiedBackup, err) || !Equivalent(verifiedBackup, j))
        {
            Print("[PNH_CORE][PJR][ERROR] BACKUP_VERIFY_FAILED steamSuffix=" + suffix + " ACTIVE_UNTOUCHED");
            return;
        }
        // Defense against changed source between backup creation and deletion.
        ref PNH_TransitionRecord latest = new PNH_TransitionRecord();
        if (!FileExist(journalPath) || !JsonFileLoader<PNH_TransitionRecord>.LoadFile(journalPath, latest, err) || !Equivalent(latest, j))
        {
            Print("[PNH_CORE][PJR][ERROR] SOURCE_CHANGED steamSuffix=" + suffix + " ACTIVE_UNTOUCHED");
            return;
        }
        if (!DeleteFile(journalPath))
        {
            Print("[PNH_CORE][PJR][ERROR] DELETE_FAILED steamSuffix=" + suffix + " INSPECT_ACTIVE_JOURNAL");
            return;
        }
        if (FileExist(journalPath))
        {
            Print("[PNH_CORE][PJR][ERROR] ACTIVE_STILL_PRESENT steamSuffix=" + suffix + " INSPECT_ACTIVE_JOURNAL");
            return;
        }
        Print("[PNH_CORE][PJR] ROTATION_OK steamSuffix=" + suffix + " sourceLife=" + j.SourceLifeId.ToString() + " currentLife=" + p.LifeId.ToString() + " BACKUP_VERIFIED PLAYER_UNTOUCHED");
    }
};
