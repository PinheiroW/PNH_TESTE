// PNH_Core v0.9.3 - SECOND LIFE PREFLIGHT - strictly read only.
// Does NOT write or rotate journals, update players, change death, respawn or inventory.
// Requires verified archived COMMITTED Life_1 and matching rotation backup.
class PNH_SecondLifePreflight
{
    protected static const string ROOT = "$profile:PNH";
    protected static const string CORE = "$profile:PNH/Core";
    protected static const string SETTINGS = "$profile:PNH/Core/SecondLifePreflight.json";
    protected static bool s_Enabled = false;
    protected static int s_ExpectedLife = 2;
    protected static int s_ExpectedRevision = 2;

    static bool Initialize()
    {
        s_Enabled = false;
        s_ExpectedLife = 2;
        s_ExpectedRevision = 2;
        if (!GetGame() || !GetGame().IsServer()) return false;
        if (!FileExist(ROOT)) MakeDirectory(ROOT);
        if (!FileExist(CORE)) MakeDirectory(CORE);
        ref PNH_SecondLifePreflightConfig cfg = new PNH_SecondLifePreflightConfig();
        string err;
        if (!FileExist(SETTINGS))
        {
            if (JsonFileLoader<PNH_SecondLifePreflightConfig>.SaveFile(SETTINGS, cfg, err))
                Print("[PNH_CORE][LIFE2] SecondLifePreflight.json created (OFF)");
            else
                Print("[PNH_CORE][LIFE2][ERROR] Could not create config: " + err);
            return false;
        }
        if (!JsonFileLoader<PNH_SecondLifePreflightConfig>.LoadFile(SETTINGS, cfg, err) || !cfg || cfg.SchemaVersion != 1 || cfg.ExpectedCurrentLifeId != 2 || cfg.ExpectedCurrentRevision != 2)
        {
            Print("[PNH_CORE][LIFE2][ERROR] Invalid config, expected life=2 revision=2; OFF");
            return false;
        }
        s_Enabled = cfg.Enabled;
        s_ExpectedLife = cfg.ExpectedCurrentLifeId;
        s_ExpectedRevision = cfg.ExpectedCurrentRevision;
        Print("[PNH_CORE][LIFE2] Enabled=" + s_Enabled.ToString() + " mode=READ_ONLY");
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

    protected static bool SameJournal(PNH_TransitionRecord a, PNH_TransitionRecord b)
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
            Print("[PNH_CORE][LIFE2][WARN] Invalid SteamID64; NO_WRITE");
            return;
        }
        string suffix = id.Substring(13, 4);
        string playerPath = "$profile:PNH/Players/" + id + ".json";
        string journalPath = "$profile:PNH/LifeJournals/" + id + ".json";
        string archivePath = "$profile:PNH/LifeHistory/" + id + "/Life_1.json";
        string backupPath = journalPath + ".rotation.bak";
        if (FileExist(journalPath))
        {
            Print("[PNH_CORE][LIFE2][WARN] ACTIVE_JOURNAL_EXISTS steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        if (!FileExist(playerPath) || !FileExist(archivePath) || !FileExist(backupPath))
        {
            Print("[PNH_CORE][LIFE2][WARN] REQUIRED_FILE_MISSING steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        ref PNH_PlayerBindingRecord p = new PNH_PlayerBindingRecord();
        ref PNH_TransitionRecord a = new PNH_TransitionRecord();
        ref PNH_TransitionRecord b = new PNH_TransitionRecord();
        string err;
        if (!JsonFileLoader<PNH_PlayerBindingRecord>.LoadFile(playerPath, p, err) || !p || !JsonFileLoader<PNH_TransitionRecord>.LoadFile(archivePath, a, err) || !a || !JsonFileLoader<PNH_TransitionRecord>.LoadFile(backupPath, b, err) || !b)
        {
            Print("[PNH_CORE][LIFE2][ERROR] JSON_UNREADABLE steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        if (p.SchemaVersion != 1 || p.SteamID64 != id || p.LifeId != s_ExpectedLife || p.Revision != s_ExpectedRevision || p.State != "SURVIVOR" || a.SchemaVersion != 1 || a.SteamID64 != id || a.SourceLifeId != 1 || a.SourceRevision != 1 || a.SourceState != "SURVIVOR" || a.Phase != "COMMITTED" || !SameJournal(a, b))
        {
            Print("[PNH_CORE][LIFE2][ERROR] LIFE_HISTORY_MISMATCH steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        Print("[PNH_CORE][LIFE2] NEXT_TRANSITION_CANDIDATE steamSuffix=" + suffix + " event=" + eventName + " currentLife=" + p.LifeId.ToString() + " revision=" + p.Revision.ToString() + " archivedLife=1 activeJournal=ABSENT NO_WRITE");
    }
};
