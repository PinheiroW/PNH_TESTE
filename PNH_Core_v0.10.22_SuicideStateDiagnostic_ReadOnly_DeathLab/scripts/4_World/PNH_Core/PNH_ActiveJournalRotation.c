// PNH_Core v0.9.2 - Explicitly approved rotation of a COMMITTED journal.
// Requires verified history copy, current player at source+1, and separate .rotation.bak.
// Does not change player records, LifeId, inventory, or vanilla persistence.
class PNH_ActiveJournalRotation
{
    protected static const string ROOT = "$profile:PNH";
    protected static const string CORE = "$profile:PNH/Core";
    protected static const string CONFIG = "$profile:PNH/Core/ActiveJournalRotation.json";
    protected static const string PLAYERS = "$profile:PNH/Players";
    protected static const string JOURNALS = "$profile:PNH/LifeJournals";
    protected static const string HISTORY = "$profile:PNH/LifeHistory";

    protected static bool s_Enabled = false;
    protected static bool s_Rotate = false;
    protected static string s_ApprovedId = "";
    protected static int s_ExpectedLife = 0;
    protected static int s_ExpectedRevision = 0;

    static bool Initialize()
    {
        s_Enabled = false;
        s_Rotate = false;
        s_ApprovedId = "";
        s_ExpectedLife = 0;
        s_ExpectedRevision = 0;
        if (!GetGame() || !GetGame().IsServer()) return false;
        if (!FileExist(ROOT)) MakeDirectory(ROOT);
        if (!FileExist(CORE)) MakeDirectory(CORE);
        ref PNH_ActiveJournalRotationConfig cfg = new PNH_ActiveJournalRotationConfig();
        string err;
        if (!FileExist(CONFIG))
        {
            if (JsonFileLoader<PNH_ActiveJournalRotationConfig>.SaveFile(CONFIG, cfg, err))
                Print("[PNH_CORE][ROTATE] ActiveJournalRotation.json created (OFF)");
            else
                Print("[PNH_CORE][ROTATE][ERROR] Cannot create configuration: " + err);
            return false;
        }
        if (!JsonFileLoader<PNH_ActiveJournalRotationConfig>.LoadFile(CONFIG, cfg, err) || !cfg || cfg.SchemaVersion != 1)
        {
            Print("[PNH_CORE][ROTATE][ERROR] Invalid config; OFF");
            return false;
        }
        s_Enabled = cfg.Enabled;
        s_Rotate = s_Enabled && cfg.RotationEnabled;
        s_ApprovedId = cfg.ApprovedSteamID64;
        s_ExpectedLife = cfg.ExpectedSourceLifeId;
        s_ExpectedRevision = cfg.ExpectedSourceRevision;
        Print("[PNH_CORE][ROTATE] Enabled=" + s_Enabled.ToString() + " RotationEnabled=" + s_Rotate.ToString() + " mode=EXPLICIT_SINGLE_ROTATION_LAB");
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
            Print("[PNH_CORE][ROTATE][WARN] Invalid identity; NO_WRITE");
            return;
        }
        string suffix = id.Substring(13, 4);
        string playerPath = PLAYERS + "/" + id + ".json";
        string journalPath = JOURNALS + "/" + id + ".json";
        if (!FileExist(playerPath))
        {
            Print("[PNH_CORE][ROTATE][WARN] PLAYER_MISSING steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        if (!FileExist(journalPath))
        {
            Print("[PNH_CORE][ROTATE] NO_ACTIVE_JOURNAL steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        ref PNH_PlayerBindingRecord player = new PNH_PlayerBindingRecord();
        ref PNH_TransitionRecord journal = new PNH_TransitionRecord();
        string err;
        if (!JsonFileLoader<PNH_PlayerBindingRecord>.LoadFile(playerPath, player, err) || !player || !JsonFileLoader<PNH_TransitionRecord>.LoadFile(journalPath, journal, err) || !journal)
        {
            Print("[PNH_CORE][ROTATE][ERROR] Source JSON unreadable; NO_WRITE");
            return;
        }
        if (player.SchemaVersion != 1 || player.SteamID64 != id || journal.SchemaVersion != 1 || journal.SteamID64 != id || journal.Phase != "COMMITTED" || journal.SourceLifeId < 1 || journal.SourceRevision < 1 || journal.SourceLifeId >= 2147483646 || journal.SourceRevision >= 2147483646 || player.LifeId != journal.SourceLifeId + 1 || player.Revision != journal.SourceRevision + 1 || player.State != "SURVIVOR")
        {
            Print("[PNH_CORE][ROTATE][ERROR] Source/player mismatch; NO_WRITE");
            return;
        }
        if (journal.SourceState != "SURVIVOR" && journal.SourceState != "PENDING" && journal.SourceState != "INFECTED" && journal.SourceState != "FINAL_DEAD")
        {
            Print("[PNH_CORE][ROTATE][ERROR] Invalid source state; NO_WRITE");
            return;
        }
        string archivePath = HISTORY + "/" + id + "/Life_" + journal.SourceLifeId.ToString() + ".json";
        if (!FileExist(archivePath))
        {
            Print("[PNH_CORE][ROTATE][ERROR] ARCHIVE_MISSING steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        ref PNH_TransitionRecord archived = new PNH_TransitionRecord();
        if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(archivePath, archived, err) || !Equivalent(archived, journal))
        {
            Print("[PNH_CORE][ROTATE][ERROR] ARCHIVE_MISMATCH steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        if (!s_Rotate)
        {
            Print("[PNH_CORE][ROTATE] ROTATION_CANDIDATE steamSuffix=" + suffix + " event=" + eventName + " currentLife=" + player.LifeId.ToString() + " archivedLife=" + journal.SourceLifeId.ToString() + " NO_WRITE");
            return;
        }
        if (!ValidId(s_ApprovedId) || s_ApprovedId != id || s_ExpectedLife != journal.SourceLifeId || s_ExpectedRevision != journal.SourceRevision)
        {
            Print("[PNH_CORE][ROTATE] APPROVAL_REQUIRED steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        // Do not overwrite the separate rotation backup; a pre-existing copy must match.
        string backupPath = journalPath + ".rotation.bak";
        if (FileExist(backupPath))
        {
            ref PNH_TransitionRecord prior = new PNH_TransitionRecord();
            if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(backupPath, prior, err) || !Equivalent(prior, journal))
            {
                Print("[PNH_CORE][ROTATE][ERROR] ROTATION_BACKUP_CONFLICT steamSuffix=" + suffix + " NO_WRITE");
                return;
            }
        }
        else
        {
            if (!CopyFile(journalPath, backupPath))
            {
                Print("[PNH_CORE][ROTATE][ERROR] Backup copy failed; NO_WRITE");
                return;
            }
        }
        ref PNH_TransitionRecord verified = new PNH_TransitionRecord();
        if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(backupPath, verified, err) || !Equivalent(verified, journal))
        {
            Print("[PNH_CORE][ROTATE][ERROR] Backup verification failed; active journal retained");
            return;
        }
        if (!DeleteFile(journalPath))
        {
            Print("[PNH_CORE][ROTATE][ERROR] Delete failed; active journal retained if present");
            return;
        }
        if (FileExist(journalPath))
        {
            Print("[PNH_CORE][ROTATE][ERROR] Active journal still present; inspect manually");
            return;
        }
        Print("[PNH_CORE][ROTATE] ROTATION_OK steamSuffix=" + suffix + " archivedLife=" + journal.SourceLifeId.ToString() + " currentLife=" + player.LifeId.ToString() + " PLAYER_UNTOUCHED");
    }
};
