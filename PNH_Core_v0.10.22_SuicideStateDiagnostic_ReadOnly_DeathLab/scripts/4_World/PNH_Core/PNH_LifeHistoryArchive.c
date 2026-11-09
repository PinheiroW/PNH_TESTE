// PNH_Core v0.9.1 - Non-destructive, explicitly approved archive of a COMMITTED journal.
// Never deletes or rotates the active journal. Never writes player or vanilla persistence.
class PNH_LifeHistoryArchive
{
    protected static const string ROOT = "$profile:PNH";
    protected static const string CORE = "$profile:PNH/Core";
    protected static const string CONFIG = "$profile:PNH/Core/LifeHistoryArchive.json";
    protected static const string PLAYERS = "$profile:PNH/Players";
    protected static const string JOURNALS = "$profile:PNH/LifeJournals";
    protected static const string HISTORY = "$profile:PNH/LifeHistory";
    protected static bool s_Enabled = false;
    protected static bool s_Archive = false;
    protected static string s_ApprovedId = "";
    protected static int s_ExpectedLife = 0;
    protected static int s_ExpectedRevision = 0;

    static bool Initialize()
    {
        s_Enabled = false;
        s_Archive = false;
        s_ApprovedId = "";
        s_ExpectedLife = 0;
        s_ExpectedRevision = 0;
        if (!GetGame() || !GetGame().IsServer()) return false;
        if (!FileExist(ROOT)) MakeDirectory(ROOT);
        if (!FileExist(CORE)) MakeDirectory(CORE);
        ref PNH_LifeHistoryArchiveConfig cfg = new PNH_LifeHistoryArchiveConfig();
        string err;
        if (!FileExist(CONFIG))
        {
            if (JsonFileLoader<PNH_LifeHistoryArchiveConfig>.SaveFile(CONFIG, cfg, err))
                Print("[PNH_CORE][ARCHIVE] LifeHistoryArchive.json created (OFF)");
            else
                Print("[PNH_CORE][ARCHIVE][ERROR] Cannot create config: " + err);
            return false;
        }
        if (!JsonFileLoader<PNH_LifeHistoryArchiveConfig>.LoadFile(CONFIG, cfg, err) || !cfg || cfg.SchemaVersion != 1)
        {
            Print("[PNH_CORE][ARCHIVE][ERROR] Invalid config; OFF");
            return false;
        }
        s_Enabled = cfg.Enabled;
        s_Archive = s_Enabled && cfg.ArchiveEnabled;
        s_ApprovedId = cfg.ApprovedSteamID64;
        s_ExpectedLife = cfg.ExpectedSourceLifeId;
        s_ExpectedRevision = cfg.ExpectedSourceRevision;
        Print("[PNH_CORE][ARCHIVE] Enabled=" + s_Enabled.ToString() + " ArchiveEnabled=" + s_Archive.ToString() + " mode=COPY_ONLY");
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

    protected static bool EqualJournal(PNH_TransitionRecord a, PNH_TransitionRecord b)
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
            Print("[PNH_CORE][ARCHIVE][WARN] Invalid SteamID64; NO_WRITE");
            return;
        }
        string suffix = id.Substring(13, 4);
        string playerPath = PLAYERS + "/" + id + ".json";
        string journalPath = JOURNALS + "/" + id + ".json";
        if (!FileExist(playerPath) || !FileExist(journalPath))
        {
            Print("[PNH_CORE][ARCHIVE] SOURCE_MISSING steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        ref PNH_PlayerBindingRecord player = new PNH_PlayerBindingRecord();
        ref PNH_TransitionRecord journal = new PNH_TransitionRecord();
        string err;
        if (!JsonFileLoader<PNH_PlayerBindingRecord>.LoadFile(playerPath, player, err) || !player || !JsonFileLoader<PNH_TransitionRecord>.LoadFile(journalPath, journal, err) || !journal)
        {
            Print("[PNH_CORE][ARCHIVE][ERROR] Invalid JSON; NO_WRITE");
            return;
        }
        if (player.SchemaVersion != 1 || journal.SchemaVersion != 1 || player.SteamID64 != id || journal.SteamID64 != id || journal.Phase != "COMMITTED" || journal.SourceLifeId < 1 || journal.SourceRevision < 1 || journal.SourceLifeId >= 2147483646 || journal.SourceRevision >= 2147483646 || player.LifeId != journal.SourceLifeId + 1 || player.Revision != journal.SourceRevision + 1 || player.State != "SURVIVOR")
        {
            Print("[PNH_CORE][ARCHIVE][ERROR] Source/committed journal mismatch; NO_WRITE");
            return;
        }
        if (journal.SourceState != "SURVIVOR" && journal.SourceState != "PENDING" && journal.SourceState != "INFECTED" && journal.SourceState != "FINAL_DEAD")
        {
            Print("[PNH_CORE][ARCHIVE][ERROR] Invalid source state; NO_WRITE");
            return;
        }
        if (!s_Archive)
        {
            Print("[PNH_CORE][ARCHIVE] ARCHIVE_CANDIDATE steamSuffix=" + suffix + " event=" + eventName + " sourceLife=" + journal.SourceLifeId.ToString() + " NO_WRITE");
            return;
        }
        if (!ValidId(s_ApprovedId) || s_ApprovedId != id || s_ExpectedLife != journal.SourceLifeId || s_ExpectedRevision != journal.SourceRevision)
        {
            Print("[PNH_CORE][ARCHIVE] APPROVAL_REQUIRED steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        string playerDir = HISTORY + "/" + id;
        string destination = playerDir + "/Life_" + journal.SourceLifeId.ToString() + ".json";
        if (FileExist(destination))
        {
            ref PNH_TransitionRecord oldArchive = new PNH_TransitionRecord();
            if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(destination, oldArchive, err) || !EqualJournal(oldArchive, journal))
            {
                Print("[PNH_CORE][ARCHIVE][ERROR] ARCHIVE_CONFLICT steamSuffix=" + suffix + " NO_WRITE");
                return;
            }
            Print("[PNH_CORE][ARCHIVE] ALREADY_ARCHIVED steamSuffix=" + suffix + " sourceLife=" + journal.SourceLifeId.ToString() + " NO_WRITE");
            return;
        }
        if (!FileExist(HISTORY)) MakeDirectory(HISTORY);
        if (!FileExist(playerDir)) MakeDirectory(playerDir);
        if (!FileExist(HISTORY) || !FileExist(playerDir))
        {
            Print("[PNH_CORE][ARCHIVE][ERROR] Directory creation failed; NO_WRITE");
            return;
        }
        if (!CopyFile(journalPath, destination))
        {
            Print("[PNH_CORE][ARCHIVE][ERROR] Copy failed; source left intact");
            return;
        }
        ref PNH_TransitionRecord verified = new PNH_TransitionRecord();
        if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(destination, verified, err) || !EqualJournal(verified, journal))
        {
            Print("[PNH_CORE][ARCHIVE][ERROR] Copy verification failed; active journal untouched; inspect archive");
            return;
        }
        Print("[PNH_CORE][ARCHIVE] ARCHIVE_OK steamSuffix=" + suffix + " sourceLife=" + journal.SourceLifeId.ToString() + " ACTIVE_JOURNAL_UNTOUCHED");
    }
};
