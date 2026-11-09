// PNH_Core v0.5 - opt-in, isolated simulated character-life persistence probe.
// Does not touch real players, inventory, native persistence, or production state.
class PNH_LifeCycleProbe
{
    protected static const string ROOT = "$profile:PNH";
    protected static const string CORE = "$profile:PNH/Core";
    protected static const string LAB = "$profile:PNH/Core/Lab";
    protected static const string SETTINGS = "$profile:PNH/Core/LifeCycleProbe.json";
    protected static const string RECORD = "$profile:PNH/Core/Lab/LifeCycleProbeRecord.json";

    static void Initialize()
    {
        if (!GetGame() || !GetGame().IsServer()) return;
        if (!FileExist(ROOT)) MakeDirectory(ROOT);
        if (!FileExist(CORE)) MakeDirectory(CORE);
        if (!FileExist(LAB)) MakeDirectory(LAB);

        ref PNH_LifeCycleProbeConfig cfg = new PNH_LifeCycleProbeConfig();
        string err;
        if (!FileExist(SETTINGS))
        {
            if (!JsonFileLoader<PNH_LifeCycleProbeConfig>.SaveFile(SETTINGS, cfg, err))
                Print("[PNH_CORE][LIFE][ERROR] Config create failed: " + err);
            else
                Print("[PNH_CORE][LIFE] LifeCycleProbe.json created (probe OFF)");
            return;
        }
        if (!JsonFileLoader<PNH_LifeCycleProbeConfig>.LoadFile(SETTINGS, cfg, err))
        {
            Print("[PNH_CORE][LIFE][ERROR] Config invalid, probe blocked: " + err);
            return;
        }
        if (!cfg || cfg.SchemaVersion != 1)
        {
            Print("[PNH_CORE][LIFE][ERROR] Unsupported config version");
            return;
        }
        if (!cfg.Enabled) return;

        ref PNH_LifeCycleProbeRecord record = new PNH_LifeCycleProbeRecord();
        bool exists = FileExist(RECORD);
        if (exists)
        {
            if (!JsonFileLoader<PNH_LifeCycleProbeRecord>.LoadFile(RECORD, record, err))
            {
                Print("[PNH_CORE][LIFE][ERROR] Unreadable record; refusing overwrite: " + err);
                return;
            }
            if (!Valid(record))
            {
                Print("[PNH_CORE][LIFE][ERROR] Invalid record; refusing overwrite");
                return;
            }
        }

        if (cfg.Mode == "CREATE")
        {
            if (exists)
            {
                Print("[PNH_CORE][LIFE] CREATE skipped: lab record already exists");
                return;
            }
            record = new PNH_LifeCycleProbeRecord();
            SaveVerify(record, "CREATE");
            return;
        }
        if (!exists)
        {
            Print("[PNH_CORE][LIFE][ERROR] No lab record for mode " + cfg.Mode);
            return;
        }
        if (cfg.Mode == "VERIFY_INFECTED")
        {
            if (record.LifeId == 1 && record.State == "INFECTED" && record.Revision == 1)
                Print("[PNH_CORE][LIFE] VERIFY_INFECTED OK; previous life remains infected after restart");
            else
                Print("[PNH_CORE][LIFE][ERROR] VERIFY_INFECTED mismatch");
            return;
        }
        if (cfg.Mode == "NEW_LIFE")
        {
            if (record.LifeId == 2 && record.State == "SURVIVOR")
            {
                Print("[PNH_CORE][LIFE] NEW_LIFE already applied; no duplicate transition");
                return;
            }
            if (record.LifeId != 1 || record.State != "INFECTED")
            {
                Print("[PNH_CORE][LIFE][ERROR] NEW_LIFE refused: unexpected previous state");
                return;
            }
            record.LifeId = 2;
            record.State = "SURVIVOR";
            record.Revision = record.Revision + 1;
            SaveVerify(record, "NEW_LIFE");
            return;
        }
        if (cfg.Mode == "VERIFY_NEW_LIFE")
        {
            if (record.LifeId == 2 && record.State == "SURVIVOR" && record.Revision == 2)
                Print("[PNH_CORE][LIFE] VERIFY_NEW_LIFE OK; old INFECTED state not inherited");
            else
                Print("[PNH_CORE][LIFE][ERROR] VERIFY_NEW_LIFE mismatch");
            return;
        }
        Print("[PNH_CORE][LIFE][ERROR] Unknown mode: " + cfg.Mode);
    }

    protected static bool Valid(PNH_LifeCycleProbeRecord record)
    {
        if (!record || record.SchemaVersion != 1 || record.AccountKey != "LAB_ACCOUNT") return false;
        if (record.LifeId < 1 || record.LifeId > 2) return false;
        if (record.Revision < 1 || record.Revision > 2) return false;
        if (record.State != "SURVIVOR" && record.State != "INFECTED") return false;
        return true;
    }

    protected static void SaveVerify(PNH_LifeCycleProbeRecord record, string operation)
    {
        string err;
        if (!JsonFileLoader<PNH_LifeCycleProbeRecord>.SaveFile(RECORD, record, err))
        {
            Print("[PNH_CORE][LIFE][ERROR] " + operation + " write failed: " + err);
            return;
        }
        ref PNH_LifeCycleProbeRecord loaded = new PNH_LifeCycleProbeRecord();
        if (!JsonFileLoader<PNH_LifeCycleProbeRecord>.LoadFile(RECORD, loaded, err))
        {
            Print("[PNH_CORE][LIFE][ERROR] " + operation + " verify read failed: " + err);
            return;
        }
        if (!Valid(loaded) || loaded.LifeId != record.LifeId || loaded.State != record.State || loaded.Revision != record.Revision)
        {
            Print("[PNH_CORE][LIFE][ERROR] " + operation + " verify mismatch");
            return;
        }
        Print("[PNH_CORE][LIFE] " + operation + " OK; life=" + loaded.LifeId.ToString() + " state=" + loaded.State + " revision=" + loaded.Revision.ToString());
        PNH_Diagnostics.Log("Core", "Persistence", "Life cycle lab operation succeeded");
    }
};
