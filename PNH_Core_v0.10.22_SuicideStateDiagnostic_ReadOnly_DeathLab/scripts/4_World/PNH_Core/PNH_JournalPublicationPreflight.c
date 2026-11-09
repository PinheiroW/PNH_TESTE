// v0.10.14 -- READ ONLY security boundary before future official publication.
// A LAB_TEST_ONLY artifact is never a trusted death and must NEVER be promoted.
// This class NEVER writes/deletes/moves any player, journal, backup, or lab file.
class PNH_JournalPublicationPreflight
{
    protected static const string CONFIG = "$profile:PNH/Core/JournalPublicationPreflight.json";
    protected static bool s_Enabled = false;

    static bool Initialize()
    {
        s_Enabled = false;
        if (!GetGame() || !GetGame().IsServer()) return false;
        if (!FileExist("$profile:PNH")) MakeDirectory("$profile:PNH");
        if (!FileExist("$profile:PNH/Core")) MakeDirectory("$profile:PNH/Core");
        ref PNH_JournalPublicationPreflightConfig cfg = new PNH_JournalPublicationPreflightConfig();
        string err;
        if (!FileExist(CONFIG))
        {
            if (JsonFileLoader<PNH_JournalPublicationPreflightConfig>.SaveFile(CONFIG, cfg, err))
                Print("[PNH_CORE][JPP] JournalPublicationPreflight.json created (OFF)");
            else
                Print("[PNH_CORE][JPP][ERROR] Config creation failed");
            return false;
        }
        if (!JsonFileLoader<PNH_JournalPublicationPreflightConfig>.LoadFile(CONFIG, cfg, err) || !cfg || cfg.SchemaVersion != 1)
        {
            Print("[PNH_CORE][JPP][ERROR] Invalid config; OFF");
            return false;
        }
        s_Enabled = cfg.Enabled;
        Print("[PNH_CORE][JPP] Enabled=" + s_Enabled.ToString() + " mode=READ_ONLY publish=NOT_IMPLEMENTED NO_WRITE");
        return s_Enabled;
    }

    protected static bool ValidId(string id)
    {
        if (id.Length() != 17) return false;
        for (int i = 0; i < 17; i++)
            if ("0123456789".IndexOf(id.Substring(i, 1)) == -1) return false;
        return true;
    }

    static void OnCFEvent(string eventName, CF_EventArgs args)
    {
        if (!s_Enabled || !GetGame() || !GetGame().IsServer()) return;
        if (eventName != "CLIENT_READY") return;
        CF_EventPlayerArgs e = CF_EventPlayerArgs.Cast(args);
        if (!e) return;
        PlayerIdentity identity = e.Identity;
        if (!identity && e.Player) identity = e.Player.GetIdentity();
        if (!identity) return;
        string id = identity.GetPlainId();
        if (!ValidId(id)) return;
        string suffix = id.Substring(13, 4);
        string active = "$profile:PNH/LifeJournals/" + id + ".json";
        string partial = active + ".create.tmp";
        string lab = "$profile:PNH/LabStaging/" + id + ".life3.labtest.tmp";
        string player = "$profile:PNH/Players/" + id + ".json";
        if (FileExist(active) || FileExist(partial) || FileExist(active + ".tmp") || FileExist(active + ".pending") || FileExist(active + ".staging") || FileExist(active + ".new"))
        {
            Print("[PNH_CORE][JPP][WARN] OFFICIAL_OR_PARTIAL_PRESENT steamSuffix=" + suffix + " MANUAL_REVIEW NO_WRITE");
            return;
        }
        if (!FileExist(player))
        {
            Print("[PNH_CORE][JPP][WARN] PLAYER_MISSING steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        string err;
        ref PNH_PlayerBindingRecord p = new PNH_PlayerBindingRecord();
        if (!JsonFileLoader<PNH_PlayerBindingRecord>.LoadFile(player, p, err) || !p || p.SteamID64 != id || p.LifeId != 3 || p.Revision != 3 || p.State != "SURVIVOR")
        {
            Print("[PNH_CORE][JPP][WARN] PLAYER_NOT_LIFE3 steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        if (!FileExist(lab))
        {
            Print("[PNH_CORE][JPP] LAB_ARTIFACT_ABSENT steamSuffix=" + suffix + " finalJournal=ABSENT NO_WRITE");
            return;
        }
        ref PNH_TransitionRecord test = new PNH_TransitionRecord();
        if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(lab, test, err) || !test)
        {
            Print("[PNH_CORE][JPP][WARN] LAB_ARTIFACT_INVALID steamSuffix=" + suffix + " MANUAL_REVIEW NO_WRITE");
            return;
        }
        if (test.SchemaVersion != 1 || test.SteamID64 != id || test.SourceLifeId != 3 || test.SourceRevision != 3 || test.SourceState != "SURVIVOR" || test.Phase != "LAB_TEST_ONLY")
        {
            Print("[PNH_CORE][JPP][WARN] LAB_ARTIFACT_CONFLICT steamSuffix=" + suffix + " MANUAL_REVIEW NO_WRITE");
            return;
        }
        // Intentional rejection: even a valid lab artifact CANNOT become a death journal.
        Print("[PNH_CORE][JPP] LAB_ONLY_REJECTED steamSuffix=" + suffix + " phase=LAB_TEST_ONLY finalJournal=ABSENT publish=DENIED NO_WRITE");
    }
};
