// PNH_Core v0.10.12 - isolated staging + read-only artifact inspection (NOT wired to connect/death).
// No final journal publish, player update, archive, rotation or commit.
// Never automatically cleans an existing partial artifact; fail closed.
class PNH_JournalStaging
{
    protected static const string ROOT = "$profile:PNH";
    protected static const string CORE = "$profile:PNH/Core";
    protected static const string CONFIG = "$profile:PNH/Core/JournalStaging.json";

    protected static bool s_InspectionEnabled = false;
    protected static bool s_StageBusy = false;

    static bool Initialize()
    {
        s_InspectionEnabled = false;
        s_StageBusy = false;
        if (!GetGame() || !GetGame().IsServer()) return false;
        if (!FileExist(ROOT)) MakeDirectory(ROOT);
        if (!FileExist(CORE)) MakeDirectory(CORE);
        ref PNH_JournalStagingConfig cfg = new PNH_JournalStagingConfig();
        string err;
        if (!FileExist(CONFIG))
        {
            if (JsonFileLoader<PNH_JournalStagingConfig>.SaveFile(CONFIG, cfg, err))
                Print("[PNH_CORE][PJS] JournalStaging.json created (OFF)");
            else
                Print("[PNH_CORE][PJS][ERROR] Config creation failed: " + err);
            return false;
        }
        if (!JsonFileLoader<PNH_JournalStagingConfig>.LoadFile(CONFIG, cfg, err) || !cfg || cfg.SchemaVersion != 1)
        {
            Print("[PNH_CORE][PJS][ERROR] Invalid staging config; OFF");
            return false;
        }
        if (cfg.AllowStaging || cfg.ApprovedSteamID64 != "")
        {
            Print("[PNH_CORE][PJS][WARN] Unsafe staging flags; inspection OFF");
            return false;
        }
        s_InspectionEnabled = cfg.Enabled;
        // Staging is deliberately NOT connected to a trusted death/transition signal.
        // Do not use configuration to attempt execution on player connection.
        Print("[PNH_CORE][PJS] stageMethod=AVAILABLE wiring=NONE publish=NOT_IMPLEMENTED commit=NOT_IMPLEMENTED NO_WRITE");
        Print("[PNH_CORE][PJS] Enabled=" + s_InspectionEnabled.ToString() + " mode=READ_ONLY stageWiring=NONE publish=NONE NO_WRITE");
        return s_InspectionEnabled;
    }

    protected static bool ValidId(string id)
    {
        if (id.Length() != 17) return false;
        string digits = "0123456789";
        for (int i = 0; i < 17; i++)
            if (digits.IndexOf(id.Substring(i, 1)) == -1) return false;
        return true;
    }

    // Login hook is diagnostic ONLY; it cannot stage or change any file.
    static void OnCFEvent(string eventName, CF_EventArgs args)
    {
        if (!s_InspectionEnabled || !GetGame() || !GetGame().IsServer()) return;
        CF_EventPlayerArgs e = CF_EventPlayerArgs.Cast(args);
        if (!e) return;
        PlayerIdentity identity = e.Identity;
        if (!identity && e.Player) identity = e.Player.GetIdentity();
        if (!identity) return;
        string id = identity.GetPlainId();
        if (!ValidId(id)) return;
        string suffix = id.Substring(13, 4);
        string playerPath = "$profile:PNH/Players/" + id + ".json";
        string journalPath = "$profile:PNH/LifeJournals/" + id + ".json";
        string stagingPath = journalPath + ".create.tmp";
        if (FileExist(journalPath))
        {
            Print("[PNH_CORE][PJS][WARN] FINAL_JOURNAL_PRESENT steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        if (FileExist(journalPath + ".tmp") || FileExist(journalPath + ".pending") || FileExist(journalPath + ".staging") || FileExist(journalPath + ".new"))
        {
            Print("[PNH_CORE][PJS][WARN] UNKNOWN_PARTIAL_ARTIFACT steamSuffix=" + suffix + " MANUAL_REVIEW NO_WRITE");
            return;
        }
        if (FileExist(stagingPath))
        {
            string err;
            ref PNH_TransitionRecord partial = new PNH_TransitionRecord();
            if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(stagingPath, partial, err) || !partial)
            {
                Print("[PNH_CORE][PJS][WARN] STAGING_INVALID steamSuffix=" + suffix + " MANUAL_REVIEW NO_WRITE");
                return;
            }
            if (partial.SchemaVersion != 1 || partial.SteamID64 != id || partial.SourceLifeId != 3 || partial.SourceRevision != 3 || partial.SourceState != "SURVIVOR" || partial.Phase != "DEATH_RECORDED")
            {
                Print("[PNH_CORE][PJS][WARN] STAGING_CONFLICT steamSuffix=" + suffix + " MANUAL_REVIEW NO_WRITE");
                return;
            }
            Print("[PNH_CORE][PJS] STAGING_VALID_UNPUBLISHED steamSuffix=" + suffix + " MANUAL_REVIEW NO_WRITE");
            return;
        }
        if (!FileExist(playerPath))
        {
            Print("[PNH_CORE][PJS][WARN] PLAYER_MISSING steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        string error;
        ref PNH_PlayerBindingRecord p = new PNH_PlayerBindingRecord();
        if (!JsonFileLoader<PNH_PlayerBindingRecord>.LoadFile(playerPath, p, error) || !p || p.SchemaVersion != 1 || p.SteamID64 != id || p.LifeId != 3 || p.Revision != 3 || p.State != "SURVIVOR")
        {
            Print("[PNH_CORE][PJS][WARN] PLAYER_UNEXPECTED steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        if (FileExist("$profile:PNH/LifeHistory/" + id + "/Life_3.json") || FileExist(journalPath + ".Life_3.rotation.bak"))
        {
            Print("[PNH_CORE][PJS][WARN] FUTURE_ARTIFACT_PRESENT steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        Print("[PNH_CORE][PJS] STAGING_INSPECTION_READY steamSuffix=" + suffix + " event=" + eventName + " sourceLife=3 revision=3 staging=ABSENT finalJournal=ABSENT NO_WRITE");
    }

    // This method is NOT called by this release. The future trusted server-side
    // death handler must prove a real death and call only once per transition.
    // It creates ONLY an isolated temp file, never publishes the active journal.
    static bool StageAfterTrustedDeath(string id, int expectedLife, int expectedRevision)
    {
        // Reentrancy protection inside this DayZ server process only.
        // NOT cross-process atomicity or a durable transaction lock.
        if (s_StageBusy) return false;
        s_StageBusy = true;
        bool result = StageInternal(id, expectedLife, expectedRevision);
        s_StageBusy = false;
        return result;
    }

    protected static bool StageInternal(string id, int expectedLife, int expectedRevision)
    {
        if (!GetGame() || !GetGame().IsServer()) return false;
        if (!ValidId(id)) return false;
        if (expectedLife != 3 || expectedRevision != 3) return false;
        string suffix = id.Substring(13, 4);
        string err;
        ref PNH_JournalStagingConfig cfg = new PNH_JournalStagingConfig();
        if (!JsonFileLoader<PNH_JournalStagingConfig>.LoadFile(CONFIG, cfg, err) || !cfg) return false;
        if (cfg.SchemaVersion != 1) return false;
        if (!cfg.Enabled || !cfg.AllowStaging) return false;
        if (cfg.ApprovedSteamID64 != id) return false;

        // Other known journal/commit writers must stay disabled.
        ref PNH_TransitionConfig tx = new PNH_TransitionConfig();
        if (!JsonFileLoader<PNH_TransitionConfig>.LoadFile("$profile:PNH/Core/TransitionJournal.json", tx, err) || !tx || tx.WriteEnabled) return false;
        ref PNH_SafeLifeCommitConfig commit = new PNH_SafeLifeCommitConfig();
        if (!JsonFileLoader<PNH_SafeLifeCommitConfig>.LoadFile("$profile:PNH/Core/SafeLifeCommit.json", commit, err) || !commit || commit.ApplyEnabled || commit.RecoveryEnabled) return false;
        ref PNH_ProtectedJournalRotationConfig rotation = new PNH_ProtectedJournalRotationConfig();
        if (!JsonFileLoader<PNH_ProtectedJournalRotationConfig>.LoadFile("$profile:PNH/Core/ProtectedJournalRotation.json", rotation, err) || !rotation || rotation.RotationEnabled) return false;
        ref PNH_LifeCycleArchiveCoordinatorConfig archive = new PNH_LifeCycleArchiveCoordinatorConfig();
        if (!JsonFileLoader<PNH_LifeCycleArchiveCoordinatorConfig>.LoadFile("$profile:PNH/Core/LifeCycleArchiveCoordinator.json", archive, err) || !archive || archive.ArchiveEnabled) return false;

        string playerPath = "$profile:PNH/Players/" + id + ".json";
        string journalPath = "$profile:PNH/LifeJournals/" + id + ".json";
        string stagingPath = journalPath + ".create.tmp";
        if (!FileExist(playerPath)) return false;
        if (FileExist(journalPath)) return false;
        if (FileExist(stagingPath)) return false;
        if (FileExist(journalPath + ".tmp")) return false;
        if (FileExist(journalPath + ".pending")) return false;
        if (FileExist(journalPath + ".staging")) return false;
        if (FileExist(journalPath + ".new")) return false;
        if (FileExist("$profile:PNH/LifeHistory/" + id + "/Life_3.json")) return false;
        if (FileExist(journalPath + ".Life_3.rotation.bak")) return false;

        ref PNH_PlayerBindingRecord p = new PNH_PlayerBindingRecord();
        if (!JsonFileLoader<PNH_PlayerBindingRecord>.LoadFile(playerPath, p, err) || !p) return false;
        if (p.SchemaVersion != 1 || p.SteamID64 != id) return false;
        if (p.LifeId != expectedLife || p.Revision != expectedRevision || p.State != "SURVIVOR") return false;

        string historyDir = "$profile:PNH/LifeHistory/" + id;
        ref PNH_TransitionRecord h1 = new PNH_TransitionRecord();
        ref PNH_TransitionRecord h2 = new PNH_TransitionRecord();
        if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(historyDir + "/Life_1.json", h1, err) || !h1) return false;
        if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(historyDir + "/Life_2.json", h2, err) || !h2) return false;
        if (h1.SteamID64 != id || h1.SourceLifeId != 1 || h1.Phase != "COMMITTED") return false;
        if (h2.SteamID64 != id || h2.SourceLifeId != 2 || h2.SourceRevision != 2 || h2.Phase != "COMMITTED") return false;
        ref PNH_TransitionRecord backup = new PNH_TransitionRecord();
        if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(journalPath + ".Life_2.rotation.bak", backup, err) || !backup) return false;
        if (backup.SteamID64 != h2.SteamID64 || backup.SourceLifeId != h2.SourceLifeId) return false;
        if (backup.SourceRevision != h2.SourceRevision || backup.SourceState != h2.SourceState || backup.Phase != h2.Phase) return false;

        ref PNH_TransitionRecord nextJournal = new PNH_TransitionRecord();
        nextJournal.SchemaVersion = 1;
        nextJournal.SteamID64 = id;
        nextJournal.SourceLifeId = expectedLife;
        nextJournal.SourceRevision = expectedRevision;
        nextJournal.SourceState = "SURVIVOR";
        nextJournal.Phase = "DEATH_RECORDED";

        // JsonFileLoader may leave a partial file if the process crashes.
        // Such a file is NEVER silently overwritten or treated as published.
        if (!JsonFileLoader<PNH_TransitionRecord>.SaveFile(stagingPath, nextJournal, err))
        {
            Print("[PNH_CORE][PJS][ERROR] STAGE_SAVE_FAILED steamSuffix=" + suffix);
            return false;
        }
        ref PNH_TransitionRecord verified = new PNH_TransitionRecord();
        if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(stagingPath, verified, err) || !verified)
        {
            Print("[PNH_CORE][PJS][ERROR] STAGE_READBACK_FAILED steamSuffix=" + suffix);
            return false;
        }
        if (verified.SchemaVersion != 1 || verified.SteamID64 != id) return false;
        if (verified.SourceLifeId != expectedLife || verified.SourceRevision != expectedRevision) return false;
        if (verified.SourceState != "SURVIVOR" || verified.Phase != "DEATH_RECORDED") return false;
        Print("[PNH_CORE][PJS] STAGE_VERIFIED steamSuffix=" + suffix + " sourceLife=3 finalJournal=ABSENT PLAYER_UNTOUCHED STAGING_ONLY");
        return true;
    }
};
