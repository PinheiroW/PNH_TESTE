// v0.10.16 - memory-only candidate after FINAL vanilla EEKilled.
// NOT an infected transformation trigger. No health, entity, journal or player mutations.
class PNH_DeathTransitionCandidate
{
    protected static const string CONFIG = "$profile:PNH/Core/DeathTransitionCandidate.json";
    protected static bool s_Enabled = false;
    protected static ref array<string> s_ObservedKeys;

    static void Initialize()
    {
        s_Enabled = false;
        s_ObservedKeys = new array<string>();
        if (!GetGame() || !GetGame().IsServer()) return;
        if (!FileExist("$profile:PNH")) MakeDirectory("$profile:PNH");
        if (!FileExist("$profile:PNH/Core")) MakeDirectory("$profile:PNH/Core");
        ref PNH_DeathTransitionCandidateConfig cfg = new PNH_DeathTransitionCandidateConfig();
        string err;
        if (!FileExist(CONFIG))
        {
            if (JsonFileLoader<PNH_DeathTransitionCandidateConfig>.SaveFile(CONFIG, cfg, err))
                Print("[PNH_CORE][DTC] DeathTransitionCandidate.json created (OFF)");
            else
                Print("[PNH_CORE][DTC][ERROR] Config creation failed");
            return;
        }
        if (!JsonFileLoader<PNH_DeathTransitionCandidateConfig>.LoadFile(CONFIG, cfg, err) || !cfg || cfg.SchemaVersion != 1)
        {
            Print("[PNH_CORE][DTC][ERROR] Invalid config; OFF");
            return;
        }
        s_Enabled = cfg.Enabled;
        Print("[PNH_CORE][DTC] Enabled=" + s_Enabled.ToString() + " mode=FINAL_DEATH_MEMORY_ONLY transform=NONE journal=NONE commit=NONE NO_WRITE");
    }

    static void OnFinalDeath(string id, PNH_PlayerBindingRecord record)
    {
        if (!s_Enabled || !GetGame() || !GetGame().IsServer() || !record) return;
        if (id.Length() != 17) return;
        string suffix = id.Substring(13, 4);
        if (record.SchemaVersion != 1 || record.SteamID64 != id || record.LifeId < 1 || record.Revision < 1 || record.State != "SURVIVOR")
        {
            Print("[PNH_CORE][DTC] CANDIDATE_REJECTED reason=INVALID_RECORD steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        // A final death is not permitted to become a zombie here.
        // The final journal must still be absent; no use of the lab test artifact.
        if (FileExist("$profile:PNH/LifeJournals/" + id + ".json"))
        {
            Print("[PNH_CORE][DTC] CANDIDATE_REJECTED reason=ACTIVE_JOURNAL_EXISTS steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        string key = id + ":" + record.LifeId.ToString() + ":" + record.Revision.ToString();
        if (!s_ObservedKeys) s_ObservedKeys = new array<string>();
        if (s_ObservedKeys.Find(key) != -1)
        {
            Print("[PNH_CORE][DTC] CANDIDATE_DUPLICATE_IGNORED steamSuffix=" + suffix + " NO_WRITE");
            return;
        }
        s_ObservedKeys.Insert(key);
        Print("[PNH_CORE][DTC] FINAL_DEATH_CANDIDATE_IN_MEMORY steamSuffix=" + suffix + " sourceLife=" + record.LifeId.ToString() + " revision=" + record.Revision.ToString() + " state=SURVIVOR nextLife=" + (record.LifeId + 1).ToString() + " TRANSFORM_NOT_AUTHORIZED NO_WRITE NO_COMMIT");
    }
};
