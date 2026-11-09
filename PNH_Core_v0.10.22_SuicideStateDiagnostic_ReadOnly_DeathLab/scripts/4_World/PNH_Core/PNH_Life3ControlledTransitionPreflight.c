// PNH_Core v0.10.7 - life 3 to 4 controlled-transition preflight (READ ONLY).
// READ ONLY for player data; only the disabled configuration may be created at bootstrap.
// This is a preflight signal, not authorization for a death, journal write or life commit.
class PNH_Life3ControlledTransitionPreflight
{
    protected static const string ROOT = "$profile:PNH";
    protected static const string CORE = "$profile:PNH/Core";
    protected static const string CONFIG = "$profile:PNH/Core/Life3ControlledTransitionPreflight.json";
    protected static bool s_Enabled = false;
    protected static int s_MaxHistoryChecks = 64;

    static bool Initialize()
    {
        s_Enabled = false;
        s_MaxHistoryChecks = 64;
        if (!GetGame() || !GetGame().IsServer()) return false;
        if (!FileExist(ROOT)) MakeDirectory(ROOT);
        if (!FileExist(CORE)) MakeDirectory(CORE);
        ref PNH_Life3ControlledTransitionPreflightConfig cfg = new PNH_Life3ControlledTransitionPreflightConfig();
        string err;
        if (!FileExist(CONFIG))
        {
            if (JsonFileLoader<PNH_Life3ControlledTransitionPreflightConfig>.SaveFile(CONFIG, cfg, err))
                Print("[PNH_CORE][L34] Life3ControlledTransitionPreflight.json created (OFF)");
            else
                Print("[PNH_CORE][L34][ERROR] Cannot create config: " + err);
            return false;
        }
        if (!JsonFileLoader<PNH_Life3ControlledTransitionPreflightConfig>.LoadFile(CONFIG, cfg, err) || !cfg || cfg.SchemaVersion != 1 || cfg.MaxHistoryChecks < 1 || cfg.MaxHistoryChecks > 256)
        {
            Print("[PNH_CORE][L34][ERROR] Invalid config; OFF");
            return false;
        }
        s_Enabled = cfg.Enabled;
        s_MaxHistoryChecks = cfg.MaxHistoryChecks;
        Print("[PNH_CORE][L34] Enabled=" + s_Enabled.ToString() + " mode=READ_ONLY maxHistoryChecks=" + s_MaxHistoryChecks.ToString());
        return s_Enabled;
    }

    // Read only: verify writer toggles are currently OFF. Missing/invalid config fails closed.
    protected static bool WritersDisabled(string suffix)
    {
        string err;
        ref PNH_TransitionConfig tx = new PNH_TransitionConfig();
        if (!JsonFileLoader<PNH_TransitionConfig>.LoadFile("$profile:PNH/Core/TransitionJournal.json", tx, err) || !tx || tx.SchemaVersion != 1 || tx.WriteEnabled)
        {
            Print("[PNH_CORE][L34][WARN] TRANSITION_WRITER_UNSAFE steamSuffix=" + suffix + " NO_WRITE");
            return false;
        }
        ref PNH_SafeLifeCommitConfig commit = new PNH_SafeLifeCommitConfig();
        if (!JsonFileLoader<PNH_SafeLifeCommitConfig>.LoadFile("$profile:PNH/Core/SafeLifeCommit.json", commit, err) || !commit || commit.SchemaVersion != 1 || commit.ApplyEnabled || commit.RecoveryEnabled)
        {
            Print("[PNH_CORE][L34][WARN] COMMIT_WRITER_UNSAFE steamSuffix=" + suffix + " NO_WRITE");
            return false;
        }
        ref PNH_LifeCycleArchiveCoordinatorConfig archive = new PNH_LifeCycleArchiveCoordinatorConfig();
        if (!JsonFileLoader<PNH_LifeCycleArchiveCoordinatorConfig>.LoadFile("$profile:PNH/Core/LifeCycleArchiveCoordinator.json", archive, err) || !archive || archive.SchemaVersion != 1 || archive.ArchiveEnabled)
        {
            Print("[PNH_CORE][L34][WARN] ARCHIVE_WRITER_UNSAFE steamSuffix=" + suffix + " NO_WRITE");
            return false;
        }
        ref PNH_ProtectedJournalRotationConfig rotation = new PNH_ProtectedJournalRotationConfig();
        if (!JsonFileLoader<PNH_ProtectedJournalRotationConfig>.LoadFile("$profile:PNH/Core/ProtectedJournalRotation.json", rotation, err) || !rotation || rotation.SchemaVersion != 1 || rotation.RotationEnabled)
        {
            Print("[PNH_CORE][L34][WARN] ROTATION_WRITER_UNSAFE steamSuffix=" + suffix + " NO_WRITE");
            return false;
        }
        ref PNH_SecondLifeTransitionGuardConfig guard = new PNH_SecondLifeTransitionGuardConfig();
        if (!JsonFileLoader<PNH_SecondLifeTransitionGuardConfig>.LoadFile("$profile:PNH/Core/SecondLifeTransitionGuard.json", guard, err) || !guard || guard.SchemaVersion != 1 || guard.JournalWriteApproved)
        {
            Print("[PNH_CORE][L34][WARN] OLD_GUARD_UNSAFE steamSuffix=" + suffix + " NO_WRITE");
            return false;
        }
        ref PNH_ActiveJournalRotationConfig oldRotation = new PNH_ActiveJournalRotationConfig();
        if (!JsonFileLoader<PNH_ActiveJournalRotationConfig>.LoadFile("$profile:PNH/Core/ActiveJournalRotation.json", oldRotation, err) || !oldRotation || oldRotation.SchemaVersion != 1 || oldRotation.RotationEnabled)
        {
            Print("[PNH_CORE][L34][WARN] OLD_ROTATION_UNSAFE steamSuffix=" + suffix + " NO_WRITE");
            return false;
        }
        ref PNH_LifeHistoryArchiveConfig oldArchive = new PNH_LifeHistoryArchiveConfig();
        if (!JsonFileLoader<PNH_LifeHistoryArchiveConfig>.LoadFile("$profile:PNH/Core/LifeHistoryArchive.json", oldArchive, err) || !oldArchive || oldArchive.SchemaVersion != 1 || oldArchive.ArchiveEnabled)
        {
            Print("[PNH_CORE][L34][WARN] OLD_ARCHIVE_UNSAFE steamSuffix=" + suffix + " NO_WRITE");
            return false;
        }
        return true;
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
            Print("[PNH_CORE][L34][WARN] INVALID_ID NO_WRITE");
            return;
        }
        string suffix = id.Substring(13, 4);
        if (!WritersDisabled(suffix)) return;
        string playerPath = "$profile:PNH/Players/" + id + ".json";
        string journalPath = "$profile:PNH/LifeJournals/" + id + ".json";
        string historyDir = "$profile:PNH/LifeHistory/" + id;
        string err;
        if (!FileExist(playerPath))
        {
            Print("[PNH_CORE][L34][WARN] PLAYER_MISSING steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        ref PNH_PlayerBindingRecord p = new PNH_PlayerBindingRecord();
        if (!JsonFileLoader<PNH_PlayerBindingRecord>.LoadFile(playerPath, p, err) || !p || p.SchemaVersion != 1 || p.SteamID64 != id || p.LifeId < 3 || p.LifeId >= 2147483646 || p.Revision < 3 || p.State != "SURVIVOR")
        {
            Print("[PNH_CORE][L34][ERROR] PLAYER_NOT_ELIGIBLE steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        int lastLife = p.LifeId - 1;
        if (lastLife > s_MaxHistoryChecks)
        {
            Print("[PNH_CORE][L34][WARN] HISTORY_SCAN_LIMIT steamSuffix=" + suffix + " required=" + lastLife.ToString() + " NO_WRITE");
            return;
        }
        if (FileExist(journalPath))
        {
            Print("[PNH_CORE][L34][WARN] ACTIVE_JOURNAL_PRESENT steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        // Do not mark the next life ready if a future archive/rotation backup already exists.
        string unexpectedHistory = historyDir + "/Life_" + p.LifeId.ToString() + ".json";
        string unexpectedBackup = journalPath + ".Life_" + p.LifeId.ToString() + ".rotation.bak";
        if (FileExist(unexpectedHistory) || FileExist(unexpectedBackup))
        {
            Print("[PNH_CORE][L34][ERROR] NEXT_LIFE_ARTIFACT_EXISTS steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        ref PNH_TransitionRecord mostRecent = null;
        int priorRevision = 0;
        for (int life = 1; life <= lastLife; life++)
        {
            string path = historyDir + "/Life_" + life.ToString() + ".json";
            if (!FileExist(path))
            {
                Print("[PNH_CORE][L34][WARN] HISTORY_MISSING steamSuffix=" + suffix + " life=" + life.ToString() + " NO_WRITE");
                return;
            }
            ref PNH_TransitionRecord h = new PNH_TransitionRecord();
            if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(path, h, err) || !h || h.SchemaVersion != 1 || h.SteamID64 != id || h.SourceLifeId != life || h.SourceRevision < 1 || !ValidState(h.SourceState) || h.Phase != "COMMITTED")
            {
                Print("[PNH_CORE][L34][ERROR] HISTORY_INVALID steamSuffix=" + suffix + " life=" + life.ToString() + " NO_WRITE");
                return;
            }
            if ((life == 1 && h.SourceRevision != 1) || (life > 1 && h.SourceRevision != priorRevision + 1))
            {
                Print("[PNH_CORE][L34][ERROR] REVISION_CHAIN_MISMATCH steamSuffix=" + suffix + " life=" + life.ToString() + " NO_WRITE");
                return;
            }
            priorRevision = h.SourceRevision;
            mostRecent = h;
        }
        if (!mostRecent || p.Revision != priorRevision + 1)
        {
            Print("[PNH_CORE][L34][ERROR] CURRENT_REVISION_MISMATCH steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        string lastBackupPath = journalPath + ".Life_" + lastLife.ToString() + ".rotation.bak";
        if (!FileExist(lastBackupPath))
        {
            Print("[PNH_CORE][L34][WARN] LAST_BACKUP_MISSING steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        ref PNH_TransitionRecord lastBackup = new PNH_TransitionRecord();
        if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(lastBackupPath, lastBackup, err) || !Equivalent(lastBackup, mostRecent))
        {
            Print("[PNH_CORE][L34][ERROR] LAST_BACKUP_MISMATCH steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        Print("[PNH_CORE][L34] PREFLIGHT_READY steamSuffix=" + suffix + " event=" + eventName + " sourceLife=" + p.LifeId.ToString() + " sourceRevision=" + p.Revision.ToString() + " candidateNextLife=" + (p.LifeId + 1).ToString() + " archives=" + lastLife.ToString() + " journal=ABSENT lastBackup=VERIFIED futureArtifacts=ABSENT writerGates=OFF NO_WRITE");
    }
};
