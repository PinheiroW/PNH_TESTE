// PNH_Core v0.8.1 - durable, single-transition LAB journal.
// Never writes to PNH/Players; never changes vanilla death, inventory or respawn.
// A journal is not proof of a safe new LifeId; completion is only observed.
class PNH_TransitionJournal
{
    protected static const string ROOT = "$profile:PNH";
    protected static const string CORE = "$profile:PNH/Core";
    protected static const string JOURNALS = "$profile:PNH/LifeJournals";
    protected static const string PLAYERS = "$profile:PNH/Players";
    protected static const string SETTINGS = "$profile:PNH/Core/TransitionJournal.json";
    protected static bool s_Enabled = false;
    protected static bool s_WriteEnabled = false;

    static bool Initialize()
    {
        s_Enabled = false;
        s_WriteEnabled = false;
        if (!GetGame() || !GetGame().IsServer()) return false;
        if (!FileExist(ROOT)) MakeDirectory(ROOT);
        if (!FileExist(CORE)) MakeDirectory(CORE);
        ref PNH_TransitionConfig cfg = new PNH_TransitionConfig();
        string err;
        if (!FileExist(SETTINGS))
        {
            if (JsonFileLoader<PNH_TransitionConfig>.SaveFile(SETTINGS, cfg, err))
                Print("[PNH_CORE][TX] TransitionJournal.json created (OFF)");
            else
                Print("[PNH_CORE][TX][ERROR] Cannot create config: " + err);
            return false;
        }
        if (!JsonFileLoader<PNH_TransitionConfig>.LoadFile(SETTINGS, cfg, err) || !cfg || cfg.SchemaVersion != 1)
        {
            Print("[PNH_CORE][TX][ERROR] Invalid config; OFF");
            return false;
        }
        s_Enabled = cfg.Enabled;
        s_WriteEnabled = s_Enabled && cfg.WriteEnabled;
        Print("[PNH_CORE][TX] Enabled=" + s_Enabled.ToString() + " WriteEnabled=" + s_WriteEnabled.ToString() + " mode=JOURNAL_ONLY");
        if (s_WriteEnabled && !FileExist(JOURNALS)) MakeDirectory(JOURNALS);
        return s_Enabled;
    }

    static bool IsEnabled() { return s_Enabled; }
    static bool CanWrite() { return s_Enabled && s_WriteEnabled; }

    protected static bool SteamValid(string id)
    {
        if (id.Length() != 17) return false;
        string digits = "0123456789";
        for (int i = 0; i < 17; i++)
            if (digits.IndexOf(id.Substring(i, 1)) == -1) return false;
        return true;
    }

    protected static bool Account(PlayerIdentity identity, out string id)
    {
        id = "";
        if (!identity) return false;
        id = identity.GetPlainId();
        return SteamValid(id);
    }

    protected static bool Baseline(string id, out PNH_PlayerBindingRecord record)
    {
        record = new PNH_PlayerBindingRecord();
        string path = PLAYERS + "/" + id + ".json";
        if (!FileExist(path))
        {
            Print("[PNH_CORE][TX][WARN] No player baseline; blocked");
            return false;
        }
        string err;
        if (!JsonFileLoader<PNH_PlayerBindingRecord>.LoadFile(path, record, err) || !record)
        {
            Print("[PNH_CORE][TX][ERROR] Player baseline unreadable; blocked");
            return false;
        }
        if (record.SchemaVersion != 1 || record.SteamID64 != id || record.LifeId < 1 || record.Revision < 1)
        {
            Print("[PNH_CORE][TX][ERROR] Player baseline invalid; blocked");
            return false;
        }
        if (record.State != "SURVIVOR" && record.State != "PENDING" && record.State != "INFECTED" && record.State != "FINAL_DEAD")
        {
            Print("[PNH_CORE][TX][ERROR] Player state unsupported; blocked");
            return false;
        }
        return true;
    }

    protected static bool JournalValid(PNH_TransitionRecord r, string id)
    {
        if (!r || r.SchemaVersion != 1 || r.SteamID64 != id || r.SourceLifeId < 1 || r.SourceRevision < 1) return false;
        if (r.SourceState != "SURVIVOR" && r.SourceState != "PENDING" && r.SourceState != "INFECTED" && r.SourceState != "FINAL_DEAD") return false;
        return r.Phase == "DEATH_RECORDED" || r.Phase == "NEW_OBSERVED" || r.Phase == "COMMITTED";
    }

    // Backup existing known-good file before rewrite. Neither copy nor save is guaranteed atomic.
    protected static bool SaveVerified(string path, PNH_TransitionRecord data, bool existed)
    {
        string error;
        if (existed)
        {
            if (!CopyFile(path, path + ".bak"))
            {
                Print("[PNH_CORE][TX][ERROR] Backup failed; write blocked");
                return false;
            }
        }
        if (!JsonFileLoader<PNH_TransitionRecord>.SaveFile(path, data, error))
        {
            Print("[PNH_CORE][TX][ERROR] Write failed; inspect journal and backup: " + error);
            return false;
        }
        ref PNH_TransitionRecord verified = new PNH_TransitionRecord();
        if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(path, verified, error) || !JournalValid(verified, data.SteamID64) || verified.Phase != data.Phase || verified.SourceLifeId != data.SourceLifeId || verified.SourceRevision != data.SourceRevision)
        {
            Print("[PNH_CORE][TX][ERROR] Postwrite validation failed; inspect journal and backup");
            return false;
        }
        return true;
    }

    // Used only after player target LifeId has been verified. Mark journal completion.
    static bool MarkCommitted(string id, PNH_TransitionRecord previous)
    {
        if (!s_WriteEnabled || !SteamValid(id)) return false;
        string path = JOURNALS + "/" + id + ".json";
        if (!FileExist(path)) return false;
        ref PNH_TransitionRecord current = new PNH_TransitionRecord();
        string err;
        if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(path, current, err) || !JournalValid(current, id)) return false;
        if (current.SourceLifeId != previous.SourceLifeId || current.SourceRevision != previous.SourceRevision || current.SourceState != previous.SourceState) return false;
        if (current.Phase == "COMMITTED") return true;
        if (current.Phase != "NEW_OBSERVED") return false;
        ref PNH_PlayerBindingRecord baseline;
        if (!Baseline(id, baseline)) return false;
        // The record may already be target life 3; use journal source and current authorized config.
        if (current.SourceLifeId >= 2 && !PNH_SecondLifeTransitionGuard.AllowCommitMarker(id, current)) return false;
        current.Phase = "COMMITTED";
        return SaveVerified(path, current, true);
    }

    static void OnKilled(PlayerBase player, PlayerIdentity identity)
    {
        if (!s_Enabled || !GetGame() || !GetGame().IsServer()) return;
        string id;
        if (!Account(identity, id))
        {
            Print("[PNH_CORE][TX][WARN] DEATH missing valid SteamID64; blocked");
            return;
        }
        string suffix = id.Substring(13, 4);
        ref PNH_PlayerBindingRecord baseline;
        if (!Baseline(id, baseline)) return;
        string path = JOURNALS + "/" + id + ".json";
        if (FileExist(path))
        {
            ref PNH_TransitionRecord existing = new PNH_TransitionRecord();
            string err;
            if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(path, existing, err) || !JournalValid(existing, id))
            {
                Print("[PNH_CORE][TX][ERROR] Existing journal invalid; NEVER overwrite");
                return;
            }
            Print("[PNH_CORE][TX] DEATH_REPEATED_OR_PRIOR steamSuffix=" + suffix + " phase=" + existing.Phase + " NO_WRITE");
            return;
        }
        if (!s_WriteEnabled)
        {
            Print("[PNH_CORE][TX] DEATH_DRY_RUN steamSuffix=" + suffix + " life=" + baseline.LifeId.ToString() + " NO_WRITE");
            return;
        }
        if (!PNH_SecondLifeTransitionGuard.AllowJournalWrite(id, baseline)) return;
        if (!FileExist(JOURNALS)) MakeDirectory(JOURNALS);
        ref PNH_TransitionRecord next = new PNH_TransitionRecord();
        next.SteamID64 = id;
        next.SourceLifeId = baseline.LifeId;
        next.SourceRevision = baseline.Revision;
        next.SourceState = baseline.State;
        if (SaveVerified(path, next, false))
            Print("[PNH_CORE][TX] DEATH_JOURNALED steamSuffix=" + suffix + " life=" + baseline.LifeId.ToString() + " phase=DEATH_RECORDED");
    }

    // Returns true ONLY when this CLIENT_NEW freshly advances a valid journal.
    static bool OnCFEvent(string name, CF_EventArgs args)
    {
        if (!s_Enabled || !GetGame() || !GetGame().IsServer()) return false;
        CF_EventPlayerArgs e = CF_EventPlayerArgs.Cast(args);
        if (!e) return false;
        PlayerIdentity identity = e.Identity;
        if (!identity && e.Player) identity = e.Player.GetIdentity();
        string id;
        if (!Account(identity, id))
        {
            Print("[PNH_CORE][TX][WARN] " + name + " invalid identity; blocked");
            return false;
        }
        string suffix = id.Substring(13, 4);
        string path = JOURNALS + "/" + id + ".json";
        if (!FileExist(path))
        {
            Print("[PNH_CORE][TX] " + name + " steamSuffix=" + suffix + " noJournal NO_WRITE");
            return false;
        }
        ref PNH_TransitionRecord current = new PNH_TransitionRecord();
        string err;
        if (!JsonFileLoader<PNH_TransitionRecord>.LoadFile(path, current, err) || !JournalValid(current, id))
        {
            Print("[PNH_CORE][TX][ERROR] Journal invalid; NEVER overwrite");
            return false;
        }
        ref PNH_PlayerBindingRecord baseline;
        if (!Baseline(id, baseline)) return false;
        if (current.Phase == "COMMITTED")
        {
            Print("[PNH_CORE][TX] ALREADY_COMMITTED steamSuffix=" + suffix + " NO_WRITE");
            return false;
        }
        if (baseline.LifeId != current.SourceLifeId || baseline.Revision != current.SourceRevision || baseline.State != current.SourceState)
        {
            Print("[PNH_CORE][TX][ERROR] Baseline changed relative to journal; blocked");
            return false;
        }
        if (name == "CLIENT_READY")
        {
            Print("[PNH_CORE][TX] READY_JOURNAL_FOUND steamSuffix=" + suffix + " phase=" + current.Phase + " NO_WRITE");
            return false;
        }
        if (name != "CLIENT_NEW") return false;
        if (current.Phase == "NEW_OBSERVED")
        {
            Print("[PNH_CORE][TX] NEW_DUPLICATE steamSuffix=" + suffix + " alreadyObserved=true NO_WRITE");
            return false;
        }
        if (!s_WriteEnabled)
        {
            Print("[PNH_CORE][TX] NEW_DRY_RUN steamSuffix=" + suffix + " correlated=true NO_WRITE");
            return false;
        }
        if (!PNH_SecondLifeTransitionGuard.AllowJournalWrite(id, baseline)) return false;
        current.Phase = "NEW_OBSERVED";
        if (SaveVerified(path, current, true))
        {
            Print("[PNH_CORE][TX] NEW_JOURNALED steamSuffix=" + suffix + " phase=NEW_OBSERVED baselineLifeUnchanged=" + baseline.LifeId.ToString());
            return true;
        }
        return false;
    }
};
