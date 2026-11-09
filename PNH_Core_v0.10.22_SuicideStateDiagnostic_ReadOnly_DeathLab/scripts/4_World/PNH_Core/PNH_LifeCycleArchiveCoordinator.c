// v0.10.2 COPY_ONLY archive of the committed active journal.
// Explicit LAB identity whitelist; sourceLife determined dynamically. No timers or deletions.
// Archive completion uses semantic record comparison; SHA-256 verification remains an external test.
class PNH_LifeCycleArchiveCoordinator
{
    protected static const string ROOT = "$profile:PNH";
    protected static const string CORE = "$profile:PNH/Core";
    protected static const string CONFIG = "$profile:PNH/Core/LifeCycleArchiveCoordinator.json";
    protected static bool s_Enabled = false;
    protected static bool s_ArchiveEnabled = false;
    protected static string s_ApprovedId = "";
    protected static int s_MaxHistoryChecks = 64;

    static bool Initialize()
    {
        s_Enabled = false;
        s_ArchiveEnabled = false;
        s_ApprovedId = "";
        s_MaxHistoryChecks = 64;
        if (!GetGame() || !GetGame().IsServer()) return false;
        if (!FileExist(ROOT)) MakeDirectory(ROOT);
        if (!FileExist(CORE)) MakeDirectory(CORE);
        ref PNH_LifeCycleArchiveCoordinatorConfig cfg = new PNH_LifeCycleArchiveCoordinatorConfig();
        string err;
        if (!FileExist(CONFIG))
        {
            if (JsonFileLoader<PNH_LifeCycleArchiveCoordinatorConfig>.SaveFile(CONFIG, cfg, err))
                Print("[PNH_CORE][LAC] LifeCycleArchiveCoordinator.json created (OFF)");
            else
                Print("[PNH_CORE][LAC][ERROR] Cannot create config: " + err);
            return false;
        }
        if (!JsonFileLoader<PNH_LifeCycleArchiveCoordinatorConfig>.LoadFile(CONFIG, cfg, err) || !cfg || cfg.SchemaVersion != 1 || cfg.MaxHistoryChecks < 1 || cfg.MaxHistoryChecks > 256)
        {
            Print("[PNH_CORE][LAC][ERROR] Invalid config; OFF");
            return false;
        }
        s_Enabled = cfg.Enabled;
        s_ArchiveEnabled = cfg.Enabled && cfg.ArchiveEnabled;
        s_ApprovedId = cfg.ApprovedSteamID64;
        s_MaxHistoryChecks = cfg.MaxHistoryChecks;
        // When ArchiveEnabled is enabled, require the approved 17-digit ID before allowing events.
        if (s_ArchiveEnabled && !ValidId(s_ApprovedId))
        {
            s_ArchiveEnabled = false;
            Print("[PNH_CORE][LAC][ERROR] ArchiveEnabled requires ApprovedSteamID64; COPY DISABLED");
        }
        Print("[PNH_CORE][LAC] Enabled=" + s_Enabled.ToString() + " ArchiveEnabled=" + s_ArchiveEnabled.ToString() + " mode=COPY_ONLY maxHistoryChecks=" + s_MaxHistoryChecks.ToString());
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

    protected static bool ValidCommitted(PNH_TransitionRecord j, string id, int life)
    {
        if (!j || j.SchemaVersion != 1 || j.SteamID64 != id || j.Phase != "COMMITTED" || j.SourceLifeId != life || j.SourceRevision < 1 || !ValidState(j.SourceState)) return false;
        return true;
    }

    protected static bool SameRecord(PNH_TransitionRecord a, PNH_TransitionRecord b)
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
            Print("[PNH_CORE][LAC][WARN] INVALID_ID NO_WRITE");
            return;
        }
        string suffix = id.Substring(13, 4);
        string playerPath = "$profile:PNH/Players/" + id + ".json";
        string journalPath = "$profile:PNH/LifeJournals/" + id + ".json";
        string archiveDir = "$profile:PNH/LifeHistory/" + id;
        if (!FileExist(playerPath))
        {
            Print("[PNH_CORE][LAC][WARN] PLAYER_MISSING steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        ref PNH_PlayerBindingRecord p = new PNH_PlayerBindingRecord();
        string err;
        if (!JsonFileLoader<PNH_PlayerBindingRecord>.LoadFile(playerPath, p, err) || !p || p.SchemaVersion != 1 || p.SteamID64 != id || p.LifeId < 1 || p.Revision < 1 || !ValidState(p.State))
        {
            Print("[PNH_CORE][LAC][ERROR] INVALID_PLAYER steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        if (!FileExist(journalPath))
        {
            Print("[PNH_CORE][LAC] NO_ACTIVE_JOURNAL steamSuffix=" + suffix + " currentLife=" + p.LifeId.ToString() + " NO_WRITE");
            return;
        }
        ref PNH_TransitionRecord journal = new PNH_TransitionRecord();
        if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(journalPath, journal, err) || !journal || journal.SchemaVersion != 1 || journal.SteamID64 != id || journal.SourceLifeId < 1 || journal.SourceRevision < 1 || !ValidState(journal.SourceState))
        {
            Print("[PNH_CORE][LAC][ERROR] INVALID_JOURNAL steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        if (journal.Phase != "COMMITTED")
        {
            Print("[PNH_CORE][LAC] NOT_COMMITTED steamSuffix=" + suffix + " phase=" + journal.Phase + " NO_WRITE");
            return;
        }
        if (journal.SourceLifeId >= 2147483646 || journal.SourceRevision >= 2147483646 || p.LifeId != journal.SourceLifeId + 1 || p.Revision != journal.SourceRevision + 1 || p.State != "SURVIVOR")
        {
            Print("[PNH_CORE][LAC][ERROR] PLAYER_JOURNAL_MISMATCH steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        int expectedArchives = journal.SourceLifeId - 1;
        if (expectedArchives > s_MaxHistoryChecks)
        {
            Print("[PNH_CORE][LAC][WARN] HISTORY_LIMIT steamSuffix=" + suffix + " required=" + expectedArchives.ToString() + " NO_WRITE");
            return;
        }
        for (int life = 1; life <= expectedArchives; life++)
        {
            string oldPath = archiveDir + "/Life_" + life.ToString() + ".json";
            if (!FileExist(oldPath))
            {
                Print("[PNH_CORE][LAC][WARN] PRIOR_ARCHIVE_MISSING steamSuffix=" + suffix + " life=" + life.ToString() + " NO_WRITE");
                return;
            }
            ref PNH_TransitionRecord old = new PNH_TransitionRecord();
            if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(oldPath, old, err) || !ValidCommitted(old, id, life))
            {
                Print("[PNH_CORE][LAC][ERROR] PRIOR_ARCHIVE_INVALID steamSuffix=" + suffix + " life=" + life.ToString() + " NO_WRITE");
                return;
            }
        }
        string destination = archiveDir + "/Life_" + journal.SourceLifeId.ToString() + ".json";
        if (FileExist(destination))
        {
            ref PNH_TransitionRecord archived = new PNH_TransitionRecord();
            if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(destination, archived, err) || !SameRecord(archived, journal))
            {
                Print("[PNH_CORE][LAC][ERROR] ARCHIVE_CONFLICT steamSuffix=" + suffix + " sourceLife=" + journal.SourceLifeId.ToString() + " NO_WRITE");
                return;
            }
            Print("[PNH_CORE][LAC] ALREADY_ARCHIVED steamSuffix=" + suffix + " sourceLife=" + journal.SourceLifeId.ToString() + " NO_WRITE");
            return;
        }
        if (!s_ArchiveEnabled)
        {
            Print("[PNH_CORE][LAC] ARCHIVE_READY steamSuffix=" + suffix + " event=" + eventName + " currentLife=" + p.LifeId.ToString() + " sourceLife=" + journal.SourceLifeId.ToString() + " revision=" + p.Revision.ToString() + " priorArchives=" + expectedArchives.ToString() + " destination=ABSENT NO_WRITE");
            return;
        }
        if (s_ApprovedId != id)
        {
            Print("[PNH_CORE][LAC] APPROVAL_REQUIRED steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        // The archive folder may already exist for prior lives. Do not alter them.
        if (!FileExist("$profile:PNH/LifeHistory")) MakeDirectory("$profile:PNH/LifeHistory");
        if (!FileExist(archiveDir)) MakeDirectory(archiveDir);
        if (!FileExist(archiveDir))
        {
            Print("[PNH_CORE][LAC][ERROR] HISTORY_DIRECTORY_FAILED steamSuffix=" + suffix + " SOURCE_UNTOUCHED");
            return;
        }
        // Do not overwrite if another callback or prior partial attempt created it.
        if (FileExist(destination))
        {
            Print("[PNH_CORE][LAC][WARN] DESTINATION_APPEARED steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        if (!CopyFile(journalPath, destination))
        {
            Print("[PNH_CORE][LAC][ERROR] COPY_FAILED steamSuffix=" + suffix + " SOURCE_UNTOUCHED");
            return;
        }
        ref PNH_TransitionRecord verified = new PNH_TransitionRecord();
        if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(destination, verified, err) || !SameRecord(verified, journal))
        {
            // Never delete or overwrite a questionable destination automatically.
            Print("[PNH_CORE][LAC][ERROR] COPY_VERIFY_FAILED steamSuffix=" + suffix + " SOURCE_UNTOUCHED INSPECT_DESTINATION");
            return;
        }
        Print("[PNH_CORE][LAC] ARCHIVE_OK steamSuffix=" + suffix + " sourceLife=" + journal.SourceLifeId.ToString() + " currentLife=" + p.LifeId.ToString() + " ACTIVE_JOURNAL_UNTOUCHED");
    }
};
