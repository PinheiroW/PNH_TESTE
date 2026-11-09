// PNH_Core v0.9.4 - secondary transition protection (life 2 -> life 3).
// No writes in this class. Guard must approve journal creation/phase advance
// for EVERY source LifeId >= 2. Never authorizes the commit itself.
class PNH_SecondLifeTransitionGuard
{
    protected static const string ROOT = "$profile:PNH";
    protected static const string CORE = "$profile:PNH/Core";
    protected static const string CONFIG = "$profile:PNH/Core/SecondLifeTransitionGuard.json";
    protected static bool s_Enabled = false;
    protected static bool s_WriteApproved = false;
    protected static string s_ApprovedId = "";
    protected static int s_Life = 2;
    protected static int s_Revision = 2;

    static bool Initialize()
    {
        s_Enabled = false;
        s_WriteApproved = false;
        s_ApprovedId = "";
        s_Life = 2;
        s_Revision = 2;
        if (!GetGame() || !GetGame().IsServer()) return false;
        if (!FileExist(ROOT)) MakeDirectory(ROOT);
        if (!FileExist(CORE)) MakeDirectory(CORE);
        ref PNH_SecondLifeTransitionGuardConfig cfg = new PNH_SecondLifeTransitionGuardConfig();
        string err;
        if (!FileExist(CONFIG))
        {
            if (JsonFileLoader<PNH_SecondLifeTransitionGuardConfig>.SaveFile(CONFIG, cfg, err))
                Print("[PNH_CORE][LIFE2TX] SecondLifeTransitionGuard.json created (OFF)");
            else
                Print("[PNH_CORE][LIFE2TX][ERROR] Cannot create config: " + err);
            return false;
        }
        if (!JsonFileLoader<PNH_SecondLifeTransitionGuardConfig>.LoadFile(CONFIG, cfg, err) || !cfg || cfg.SchemaVersion != 1 || cfg.ExpectedSourceLifeId != 2 || cfg.ExpectedSourceRevision != 2)
        {
            Print("[PNH_CORE][LIFE2TX][ERROR] Invalid config; OFF");
            return false;
        }
        s_Enabled = cfg.Enabled;
        s_WriteApproved = cfg.Enabled && cfg.JournalWriteApproved;
        s_ApprovedId = cfg.ApprovedSteamID64;
        s_Life = cfg.ExpectedSourceLifeId;
        s_Revision = cfg.ExpectedSourceRevision;
        Print("[PNH_CORE][LIFE2TX] Enabled=" + s_Enabled.ToString() + " JournalWriteApproved=" + s_WriteApproved.ToString() + " mode=EXPLICIT_SECOND_LIFE_LAB");
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

    protected static bool Same(PNH_TransitionRecord a, PNH_TransitionRecord b)
    {
        if (!a || !b) return false;
        return a.SchemaVersion == b.SchemaVersion && a.SteamID64 == b.SteamID64 && a.SourceLifeId == b.SourceLifeId && a.SourceRevision == b.SourceRevision && a.SourceState == b.SourceState && a.Phase == b.Phase;
    }

    // Check disk baseline and the already completed life-1 archive + rotation backup.
    // The active journal may be present on CLIENT_NEW, but never on readiness preflight.
    protected static bool VerifiedContext(string id, PNH_PlayerBindingRecord baseline)
    {
        if (!ValidId(id) || !baseline || baseline.SchemaVersion != 1 || baseline.SteamID64 != id) return false;
        if (baseline.LifeId != 2 || baseline.Revision != 2 || baseline.State != "SURVIVOR") return false;
        string archivePath = "$profile:PNH/LifeHistory/" + id + "/Life_1.json";
        string backupPath = "$profile:PNH/LifeJournals/" + id + ".json.rotation.bak";
        if (!FileExist(archivePath) || !FileExist(backupPath)) return false;
        ref PNH_TransitionRecord a = new PNH_TransitionRecord();
        ref PNH_TransitionRecord b = new PNH_TransitionRecord();
        string err;
        if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(archivePath, a, err) || !JsonFileLoader<PNH_TransitionRecord>.LoadFile(backupPath, b, err)) return false;
        if (!Same(a, b)) return false;
        return a.SchemaVersion == 1 && a.SteamID64 == id && a.SourceLifeId == 1 && a.SourceRevision == 1 && a.SourceState == "SURVIVOR" && a.Phase == "COMMITTED";
    }

    // Called ONLY before journal writes. For life 1, preserve validated legacy behavior.
    // For life >= 2, reject unless all conditions are approved, including actor and history.
    static bool AllowJournalWrite(string id, PNH_PlayerBindingRecord baseline)
    {
        if (!baseline || baseline.LifeId < 1) return false;
        if (baseline.LifeId == 1) return true;
        if (!GetGame() || !GetGame().IsServer()) return false;
        if (!s_WriteApproved || !ValidId(s_ApprovedId) || s_ApprovedId != id || baseline.LifeId != s_Life || baseline.Revision != s_Revision)
        {
            Print("[PNH_CORE][LIFE2TX] JOURNAL_WRITE_BLOCKED steamSuffix=" + id.Substring(13, 4) + " approvalRequired=true NO_WRITE");
            return false;
        }
        if (!VerifiedContext(id, baseline))
        {
            Print("[PNH_CORE][LIFE2TX][ERROR] CONTEXT_INVALID steamSuffix=" + id.Substring(13, 4) + " NO_WRITE");
            return false;
        }
        return true;
    }

    static bool AllowCommitMarker(string id, PNH_TransitionRecord journal)
    {
        if (!journal || journal.SourceLifeId != 2 || journal.SourceRevision != 2 || journal.SteamID64 != id || journal.SourceState != "SURVIVOR") return false;
        if (!s_WriteApproved || !ValidId(s_ApprovedId) || s_ApprovedId != id || !GetGame() || !GetGame().IsServer()) return false;
        string archivePath = "$profile:PNH/LifeHistory/" + id + "/Life_1.json";
        string backupPath = "$profile:PNH/LifeJournals/" + id + ".json.rotation.bak";
        if (!FileExist(archivePath) || !FileExist(backupPath)) return false;
        ref PNH_TransitionRecord a = new PNH_TransitionRecord();
        ref PNH_TransitionRecord b = new PNH_TransitionRecord();
        string err;
        if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(archivePath, a, err) || !JsonFileLoader<PNH_TransitionRecord>.LoadFile(backupPath, b, err)) return false;
        return Same(a, b) && a.SteamID64 == id && a.SourceLifeId == 1 && a.SourceRevision == 1 && a.SourceState == "SURVIVOR" && a.Phase == "COMMITTED";
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
        if (!ValidId(id)) return;
        string suffix = id.Substring(13, 4);
        string playerPath = "$profile:PNH/Players/" + id + ".json";
        string journalPath = "$profile:PNH/LifeJournals/" + id + ".json";
        if (!FileExist(playerPath))
        {
            Print("[PNH_CORE][LIFE2TX][WARN] PLAYER_MISSING steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        ref PNH_PlayerBindingRecord p = new PNH_PlayerBindingRecord();
        string err;
        if (!JsonFileLoader<PNH_PlayerBindingRecord>.LoadFile(playerPath, p, err) || !VerifiedContext(id, p))
        {
            Print("[PNH_CORE][LIFE2TX][WARN] CONTEXT_NOT_READY steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        if (FileExist(journalPath))
        {
            Print("[PNH_CORE][LIFE2TX] JOURNAL_PRESENT steamSuffix=" + suffix + " event=" + eventName + " NO_WRITE");
            return;
        }
        Print("[PNH_CORE][LIFE2TX] SECOND_LIFE_CANDIDATE steamSuffix=" + suffix + " event=" + eventName + " currentLife=2 revision=2 archivedLife=1 journal=ABSENT NO_WRITE");
    }
};
