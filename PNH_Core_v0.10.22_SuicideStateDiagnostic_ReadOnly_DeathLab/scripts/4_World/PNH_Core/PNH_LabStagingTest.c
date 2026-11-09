// v0.10.13 LAB ONLY: isolated JSON write/readback, NOT a trusted death.
// Runs only on SERVER BOOT if explicitly opted in. No login/death triggers.
// Destination: $profile:PNH/LabStaging/<SteamID64>.life3.labtest.tmp
// This writes NO active journal, no player, history, backup, or DayZ data.
class PNH_LabStagingTest
{
    protected static const string CONFIG = "$profile:PNH/Core/LabStagingTest.json";
    protected static const string PHRASE = "I_APPROVE_LAB_STAGING_LIFE3";
    protected static bool s_Busy = false;

    static void Initialize()
    {
        if (!GetGame() || !GetGame().IsServer()) return;
        if (!FileExist("$profile:PNH")) MakeDirectory("$profile:PNH");
        if (!FileExist("$profile:PNH/Core")) MakeDirectory("$profile:PNH/Core");
        ref PNH_LabStagingTestConfig cfg = new PNH_LabStagingTestConfig();
        string err;
        if (!FileExist(CONFIG))
        {
            if (JsonFileLoader<PNH_LabStagingTestConfig>.SaveFile(CONFIG, cfg, err))
                Print("[PNH_CORE][PLS] LabStagingTest.json created (OFF)");
            else
                Print("[PNH_CORE][PLS][ERROR] Config creation failed");
            return;
        }
        if (!JsonFileLoader<PNH_LabStagingTestConfig>.LoadFile(CONFIG, cfg, err) || !cfg || cfg.SchemaVersion != 1)
        {
            Print("[PNH_CORE][PLS][ERROR] Invalid config; OFF");
            return;
        }
        if (!cfg.Enabled || !cfg.LabWriteRequested)
        {
            Print("[PNH_CORE][PLS] LAB_TEST_OFF finalJournal=UNTOUCHED NO_WRITE");
            return;
        }
        if (cfg.ApprovalPhrase != PHRASE || !ValidId(cfg.ApprovedSteamID64))
        {
            Print("[PNH_CORE][PLS][WARN] INVALID_LAB_APPROVAL NO_WRITE");
            return;
        }
        if (s_Busy) return;
        s_Busy = true;
        bool ok = ExecuteLabWrite(cfg.ApprovedSteamID64);
        s_Busy = false;
        if (!ok) Print("[PNH_CORE][PLS][WARN] LAB_STAGE_NOT_COMPLETED; CHECK_DIAGNOSTICS NO_PUBLISH NO_COMMIT");
    }

    protected static bool ValidId(string id)
    {
        if (id.Length() != 17) return false;
        for (int i = 0; i < 17; i++)
            if ("0123456789".IndexOf(id.Substring(i, 1)) == -1) return false;
        return true;
    }

    protected static bool ExecuteLabWrite(string id)
    {
        string suffix = id.Substring(13, 4);
        string err;
        // Deny if any known journal/commit writer is active.
        ref PNH_TransitionConfig tx = new PNH_TransitionConfig();
        if (!JsonFileLoader<PNH_TransitionConfig>.LoadFile("$profile:PNH/Core/TransitionJournal.json", tx, err) || !tx || tx.WriteEnabled) return false;
        ref PNH_SafeLifeCommitConfig commit = new PNH_SafeLifeCommitConfig();
        if (!JsonFileLoader<PNH_SafeLifeCommitConfig>.LoadFile("$profile:PNH/Core/SafeLifeCommit.json", commit, err) || !commit || commit.ApplyEnabled || commit.RecoveryEnabled) return false;
        ref PNH_ProtectedJournalRotationConfig rotation = new PNH_ProtectedJournalRotationConfig();
        if (!JsonFileLoader<PNH_ProtectedJournalRotationConfig>.LoadFile("$profile:PNH/Core/ProtectedJournalRotation.json", rotation, err) || !rotation || rotation.RotationEnabled) return false;
        ref PNH_LifeCycleArchiveCoordinatorConfig archive = new PNH_LifeCycleArchiveCoordinatorConfig();
        if (!JsonFileLoader<PNH_LifeCycleArchiveCoordinatorConfig>.LoadFile("$profile:PNH/Core/LifeCycleArchiveCoordinator.json", archive, err) || !archive || archive.ArchiveEnabled) return false;
        ref PNH_JournalStagingConfig stage = new PNH_JournalStagingConfig();
        if (!JsonFileLoader<PNH_JournalStagingConfig>.LoadFile("$profile:PNH/Core/JournalStaging.json", stage, err) || !stage || stage.Enabled || stage.AllowStaging) return false;

        string playerPath = "$profile:PNH/Players/" + id + ".json";
        string activeJournal = "$profile:PNH/LifeJournals/" + id + ".json";
        if (FileExist(activeJournal)) return false;
        if (FileExist(activeJournal + ".create.tmp")) return false;
        if (FileExist(activeJournal + ".tmp")) return false;
        if (FileExist(activeJournal + ".pending")) return false;
        if (FileExist(activeJournal + ".staging")) return false;
        if (FileExist(activeJournal + ".new")) return false;
        if (FileExist("$profile:PNH/LifeHistory/" + id + "/Life_3.json")) return false;
        if (FileExist(activeJournal + ".Life_3.rotation.bak")) return false;
        if (!FileExist(playerPath)) return false;
        ref PNH_PlayerBindingRecord p = new PNH_PlayerBindingRecord();
        if (!JsonFileLoader<PNH_PlayerBindingRecord>.LoadFile(playerPath, p, err) || !p) return false;
        if (p.SchemaVersion != 1 || p.SteamID64 != id) return false;
        if (p.LifeId != 3 || p.Revision != 3 || p.State != "SURVIVOR") return false;
        ref PNH_TransitionRecord h1 = new PNH_TransitionRecord();
        ref PNH_TransitionRecord h2 = new PNH_TransitionRecord();
        ref PNH_TransitionRecord back = new PNH_TransitionRecord();
        if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile("$profile:PNH/LifeHistory/" + id + "/Life_1.json", h1, err) || !h1) return false;
        if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile("$profile:PNH/LifeHistory/" + id + "/Life_2.json", h2, err) || !h2) return false;
        if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(activeJournal + ".Life_2.rotation.bak", back, err) || !back) return false;
        if (h1.SteamID64 != id || h1.SourceLifeId != 1 || h1.Phase != "COMMITTED") return false;
        if (h2.SteamID64 != id || h2.SourceLifeId != 2 || h2.SourceRevision != 2 || h2.Phase != "COMMITTED") return false;
        if (back.SteamID64 != id || back.SourceLifeId != 2 || back.SourceRevision != 2 || back.Phase != "COMMITTED") return false;

        // Distinct directory and phase: NEVER a death record and NEVER an active journal.
        string dir = "$profile:PNH/LabStaging";
        string dest = dir + "/" + id + ".life3.labtest.tmp";
        if (FileExist(dest))
        {
            Print("[PNH_CORE][PLS] LAB_STAGE_ALREADY_PRESENT steamSuffix=" + suffix + " NO_WRITE MANUAL_REVIEW");
            return false;
        }
        if (!FileExist(dir)) MakeDirectory(dir);
        if (FileExist(dest)) return false;
        ref PNH_TransitionRecord test = new PNH_TransitionRecord();
        test.SchemaVersion = 1;
        test.SteamID64 = id;
        test.SourceLifeId = 3;
        test.SourceRevision = 3;
        test.SourceState = "SURVIVOR";
        test.Phase = "LAB_TEST_ONLY";
        if (!JsonFileLoader<PNH_TransitionRecord>.SaveFile(dest, test, err))
        {
            Print("[PNH_CORE][PLS][ERROR] LAB_STAGE_SAVE_FAILED steamSuffix=" + suffix);
            return false;
        }
        ref PNH_TransitionRecord verify = new PNH_TransitionRecord();
        if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(dest, verify, err) || !verify)
        {
            Print("[PNH_CORE][PLS][ERROR] LAB_STAGE_READBACK_FAILED steamSuffix=" + suffix);
            return false;
        }
        if (verify.SchemaVersion != 1 || verify.SteamID64 != id) return false;
        if (verify.SourceLifeId != 3 || verify.SourceRevision != 3) return false;
        if (verify.SourceState != "SURVIVOR" || verify.Phase != "LAB_TEST_ONLY") return false;
        Print("[PNH_CORE][PLS] LAB_STAGE_VERIFIED steamSuffix=" + suffix + " phase=LAB_TEST_ONLY temp=CREATED finalJournal=ABSENT PLAYER_UNTOUCHED NO_COMMIT");
        return true;
    }
};
