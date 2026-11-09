// v0.8.3 - Explicit recovery of an existing NEW_OBSERVED journal (single-transition LAB).
// Writes ONLY PNH/Players and PNH/LifeJournals; vanilla persistence is untouched.
// IMPORTANT: not crash-atomic; JSON backup + readback + fail-closed validation.
// A COMMITTED journal intentionally blocks NEXT deaths pending history rotation in a later version.
class PNH_SafeLifeCommit
{
    protected static const string ROOT = "$profile:PNH";
    protected static const string CORE = "$profile:PNH/Core";
    protected static const string CONFIG = "$profile:PNH/Core/SafeLifeCommit.json";
    protected static const string PLAYERS = "$profile:PNH/Players";
    protected static const string JOURNALS = "$profile:PNH/LifeJournals";
    protected static bool s_Enabled = false;
    protected static bool s_Apply = false;
    protected static bool s_Recovery = false;
    protected static string s_RecoverySteamId = "";
    protected static int s_ExpectedLife = 0;
    protected static int s_ExpectedRevision = 0;

    static bool Initialize()
    {
        s_Enabled = false;
        s_Apply = false;
        s_Recovery = false;
        s_RecoverySteamId = "";
        s_ExpectedLife = 0;
        s_ExpectedRevision = 0;
        if (!GetGame() || !GetGame().IsServer()) return false;
        if (!FileExist(ROOT)) MakeDirectory(ROOT);
        if (!FileExist(CORE)) MakeDirectory(CORE);
        ref PNH_SafeLifeCommitConfig cfg = new PNH_SafeLifeCommitConfig();
        string err;
        if (!FileExist(CONFIG))
        {
            if (JsonFileLoader<PNH_SafeLifeCommitConfig>.SaveFile(CONFIG, cfg, err))
                Print("[PNH_CORE][COMMIT] SafeLifeCommit.json created (OFF)");
            else
                Print("[PNH_CORE][COMMIT][ERROR] Cannot create config: " + err);
            return false;
        }
        if (!JsonFileLoader<PNH_SafeLifeCommitConfig>.LoadFile(CONFIG, cfg, err) || !cfg || cfg.SchemaVersion != 1)
        {
            Print("[PNH_CORE][COMMIT][ERROR] Invalid config; OFF");
            return false;
        }
        s_Enabled = cfg.Enabled;
        s_Apply = s_Enabled && cfg.ApplyEnabled;
        s_Recovery = s_Enabled && cfg.RecoveryEnabled;
        s_RecoverySteamId = cfg.RecoverySteamID64;
        s_ExpectedLife = cfg.ExpectedSourceLifeId;
        s_ExpectedRevision = cfg.ExpectedSourceRevision;
        Print("[PNH_CORE][COMMIT] Enabled=" + s_Enabled.ToString() + " ApplyEnabled=" + s_Apply.ToString() + " mode=EXPLICIT_RECOVERY_LAB");
        Print("[PNH_CORE][COMMIT] RecoveryEnabled=" + s_Recovery.ToString() + " recoveryRequiresApproval=true");
        if (s_Apply && !PNH_TransitionJournal.CanWrite())
        {
            Print("[PNH_CORE][COMMIT][ERROR] TransitionJournal WriteEnabled is OFF; commit blocked");
            s_Apply = false;
        }
        return s_Enabled;
    }

    protected static bool SteamId(CF_EventArgs args, out string id)
    {
        id = "";
        CF_EventPlayerArgs e = CF_EventPlayerArgs.Cast(args);
        if (!e) return false;
        PlayerIdentity identity = e.Identity;
        if (!identity && e.Player) identity = e.Player.GetIdentity();
        if (!identity) return false;
        id = identity.GetPlainId();
        if (id.Length() != 17) return false;
        string digits = "0123456789";
        for (int i = 0; i < 17; i++)
            if (digits.IndexOf(id.Substring(i, 1)) == -1) return false;
        return true;
    }

    protected static bool ReadJournal(string id, out PNH_TransitionRecord j)
    {
        j = new PNH_TransitionRecord();
        string err;
        string path = JOURNALS + "/" + id + ".json";
        if (!FileExist(path)) return false;
        if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(path, j, err) || !j) return false;
        if (j.SchemaVersion != 1 || j.SteamID64 != id || j.SourceLifeId < 1 || j.SourceRevision < 1) return false;
        if (j.SourceState != "SURVIVOR" && j.SourceState != "PENDING" && j.SourceState != "INFECTED" && j.SourceState != "FINAL_DEAD") return false;
        return j.Phase == "DEATH_RECORDED" || j.Phase == "NEW_OBSERVED" || j.Phase == "COMMITTED";
    }

    protected static bool ReadPlayer(string id, out PNH_PlayerBindingRecord r)
    {
        r = new PNH_PlayerBindingRecord();
        string path = PLAYERS + "/" + id + ".json";
        if (!FileExist(path)) return false;
        string err;
        if (!JsonFileLoader<PNH_PlayerBindingRecord>.LoadFile(path, r, err) || !r) return false;
        if (r.SchemaVersion != 1 || r.SteamID64 != id || r.LifeId < 1 || r.Revision < 1) return false;
        return r.State == "SURVIVOR" || r.State == "PENDING" || r.State == "INFECTED" || r.State == "FINAL_DEAD";
    }

    protected static bool IsSource(PNH_PlayerBindingRecord r, PNH_TransitionRecord j)
    {
        return r.LifeId == j.SourceLifeId && r.Revision == j.SourceRevision && r.State == j.SourceState;
    }

    protected static bool IsTarget(PNH_PlayerBindingRecord r, PNH_TransitionRecord j)
    {
        return r.LifeId == j.SourceLifeId + 1 && r.Revision == j.SourceRevision + 1 && r.State == "SURVIVOR";
    }

    // Only recognize recovery for an exact, explicitly administrator-approved account
    // AND expected source life/revision. Old journal files never authorize writes by themselves.
    protected static bool RecoveryApproved(string id, PNH_TransitionRecord j)
    {
        if (!s_Recovery || !s_Apply) return false;
        if (s_RecoverySteamId != id || !ValidSteamIdString(s_RecoverySteamId)) return false;
        if (s_ExpectedLife < 1 || s_ExpectedRevision < 1) return false;
        return j.SourceLifeId == s_ExpectedLife && j.SourceRevision == s_ExpectedRevision;
    }

    protected static bool ValidSteamIdString(string id)
    {
        if (id.Length() != 17) return false;
        string digits = "0123456789";
        for (int i = 0; i < 17; i++)
            if (digits.IndexOf(id.Substring(i, 1)) == -1) return false;
        return true;
    }

    // Read-only inspection on CLIENT_READY by default.
    // Apply a PREVIOUSLY observed transition only with explicit account+life+revision approval.
    static void OnReady(CF_EventArgs args)
    {
        if (!s_Enabled || !GetGame() || !GetGame().IsServer()) return;
        string id;
        if (!SteamId(args, id)) return;
        string path = JOURNALS + "/" + id + ".json";
        if (!FileExist(path)) return;
        ref PNH_TransitionRecord j;
        if (!ReadJournal(id, j))
        {
            Print("[PNH_CORE][COMMIT][ERROR] READY_INVALID_JOURNAL; blocked");
            return;
        }
        ref PNH_PlayerBindingRecord r;
        if (!ReadPlayer(id, r))
        {
            Print("[PNH_CORE][COMMIT][ERROR] Invalid player record; blocked");
            return;
        }
        string suffix = id.Substring(13, 4);
        if (j.Phase == "COMMITTED" && IsTarget(r, j))
        {
            Print("[PNH_CORE][COMMIT] READY_ALREADY_COMMITTED steamSuffix=" + suffix + " life=" + r.LifeId.ToString() + " NO_WRITE");
            return;
        }
        if (j.Phase == "NEW_OBSERVED" && IsTarget(r, j))
        {
            if (!s_Apply)
            {
                Print("[PNH_CORE][COMMIT] READY_RECOVERY_NEEDED steamSuffix=" + suffix + " targetAlreadyApplied=true NO_WRITE");
                return;
            }
            // LAB safeguard: marker repair also requires exact administrator approval.
            if (!RecoveryApproved(id, j))
            {
                Print("[PNH_CORE][COMMIT] READY_MARKER_REPAIR_REQUIRES_APPROVAL steamSuffix=" + suffix + " NO_WRITE");
                return;
            }
            // Repair journal marker only: never increment player twice.
            if (PNH_TransitionJournal.MarkCommitted(id, j))
                Print("[PNH_CORE][COMMIT] RECOVERED_MARKER steamSuffix=" + suffix + " life=" + r.LifeId.ToString());
            else
                Print("[PNH_CORE][COMMIT][ERROR] MARKER_REPAIR_FAILED; player already at target");
            return;
        }
        if (j.Phase == "NEW_OBSERVED" && IsSource(r, j))
        {
            if (!RecoveryApproved(id, j))
            {
                Print("[PNH_CORE][COMMIT] RECOVERY_CANDIDATE steamSuffix=" + suffix + " sourceLife=" + r.LifeId.ToString() + " sourceRevision=" + r.Revision.ToString() + " NO_WRITE approvalRequired=true");
                return;
            }
            Print("[PNH_CORE][COMMIT] RECOVERY_APPROVED steamSuffix=" + suffix + " sourceLife=" + r.LifeId.ToString());
            CommitFromSource(id, j, r);
            return;
        }
        if (j.Phase == "DEATH_RECORDED" && IsSource(r, j))
        {
            Print("[PNH_CORE][COMMIT] READY_DEATH_PENDING steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        Print("[PNH_CORE][COMMIT][WARN] READY_STATE_MISMATCH steamSuffix=" + suffix + " NO_WRITE");
    }

    // Fresh CLIENT_NEW path, preserved from v0.8.2. The existing journal must be NEW_OBSERVED.
    static void OnNewlyObserved(CF_EventArgs args)
    {
        if (!s_Enabled || !GetGame() || !GetGame().IsServer()) return;
        string id;
        if (!SteamId(args, id)) return;
        string suffix = id.Substring(13, 4);
        ref PNH_TransitionRecord j;
        ref PNH_PlayerBindingRecord r;
        if (!ReadJournal(id, j) || !ReadPlayer(id, r))
        {
            Print("[PNH_CORE][COMMIT][ERROR] Invalid/missing journal or player; blocked");
            return;
        }
        if (j.Phase != "NEW_OBSERVED" || !IsSource(r, j))
        {
            Print("[PNH_CORE][COMMIT][ERROR] Not an eligible new transition; blocked");
            return;
        }
        if (!s_Apply)
        {
            Print("[PNH_CORE][COMMIT] COMMIT_DRY_RUN steamSuffix=" + suffix + " sourceLife=" + r.LifeId.ToString() + " targetLife=" + (r.LifeId + 1).ToString() + " NO_WRITE");
            return;
        }
        // LAB safeguard: a fresh CLIENT_NEW cannot bypass the account-specific approval.
        if (!RecoveryApproved(id, j))
        {
            Print("[PNH_CORE][COMMIT] NEW_COMMIT_REQUIRES_APPROVAL steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        CommitFromSource(id, j, r);
    }

    // Single shared commit implementation for fresh and approved recovery events.
    // Re-read both records immediately before applying. All ambiguous states fail closed.
    protected static void CommitFromSource(string id, PNH_TransitionRecord j, PNH_PlayerBindingRecord r)
    {
        string suffix = id.Substring(13, 4);
        if (!s_Apply || !PNH_TransitionJournal.CanWrite()) return;
        ref PNH_TransitionRecord latestJ;
        ref PNH_PlayerBindingRecord latestR;
        if (!ReadJournal(id, latestJ) || !ReadPlayer(id, latestR) || latestJ.Phase != "NEW_OBSERVED")
        {
            Print("[PNH_CORE][COMMIT][ERROR] COMMIT_RECHECK_FAILED NO_WRITE");
            return;
        }
        if (latestJ.SourceLifeId != j.SourceLifeId || latestJ.SourceRevision != j.SourceRevision || latestJ.SourceState != j.SourceState || !IsSource(latestR, latestJ))
        {
            Print("[PNH_CORE][COMMIT][ERROR] COMMIT_SOURCE_CHANGED NO_WRITE");
            return;
        }
        if (latestR.LifeId >= 2147483646 || latestR.Revision >= 2147483646)
        {
            Print("[PNH_CORE][COMMIT][ERROR] Life/revision integer limit; blocked");
            return;
        }
        string path = PLAYERS + "/" + id + ".json";
        string backup = path + ".precommit.bak";
        if (!CopyFile(path, backup))
        {
            Print("[PNH_CORE][COMMIT][ERROR] Player backup failed; blocked");
            return;
        }
        // Verify backup contents before touching the primary record.
        ref PNH_PlayerBindingRecord checkedBackup = new PNH_PlayerBindingRecord();
        string err;
        if (!JsonFileLoader<PNH_PlayerBindingRecord>.LoadFile(backup, checkedBackup, err) || !checkedBackup || checkedBackup.SteamID64 != id || checkedBackup.LifeId != latestR.LifeId || checkedBackup.Revision != latestR.Revision || checkedBackup.State != latestR.State)
        {
            Print("[PNH_CORE][COMMIT][ERROR] Backup verification failed; blocked");
            return;
        }
        latestR.LifeId = latestR.LifeId + 1;
        latestR.Revision = latestR.Revision + 1;
        latestR.State = "SURVIVOR";
        if (!JsonFileLoader<PNH_PlayerBindingRecord>.SaveFile(path, latestR, err))
        {
            Print("[PNH_CORE][COMMIT][ERROR] Player write failed; inspect backup");
            return;
        }
        ref PNH_PlayerBindingRecord verified;
        if (!ReadPlayer(id, verified) || !IsTarget(verified, latestJ))
        {
            Print("[PNH_CORE][COMMIT][ERROR] Player postwrite invalid; inspect backup; do not retry blindly");
            return;
        }
        if (!PNH_TransitionJournal.MarkCommitted(id, latestJ))
        {
            Print("[PNH_CORE][COMMIT][ERROR] Player advanced but journal not committed; reconnect may repair marker");
            return;
        }
        Print("[PNH_CORE][COMMIT] COMMIT_OK steamSuffix=" + suffix + " life=" + verified.LifeId.ToString() + " revision=" + verified.Revision.ToString() + " state=SURVIVOR");
    }
};
