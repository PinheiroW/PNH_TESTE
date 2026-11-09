// PNH_Core v0.4 - isolated storage proof of concept.
// Data is for the lab only. NOT suitable yet for player state persistence.
class PNH_StorageProbe
{
    protected static const string BASE_DIR = "$profile:PNH";
    protected static const string CORE_DIR = "$profile:PNH/Core";
    protected static const string LAB_DIR = "$profile:PNH/Core/Lab";
    protected static const string SETTINGS_PATH = "$profile:PNH/Core/StorageProbe.json";
    protected static const string RECORD_PATH = "$profile:PNH/Core/Lab/StorageProbeRecord.json";

    static void Initialize()
    {
        if (!GetGame() || !GetGame().IsServer())
            return;

        if (!FileExist(BASE_DIR)) MakeDirectory(BASE_DIR);
        if (!FileExist(CORE_DIR)) MakeDirectory(CORE_DIR);
        if (!FileExist(LAB_DIR)) MakeDirectory(LAB_DIR);

        ref PNH_StorageProbeConfig cfg = new PNH_StorageProbeConfig();
        string err;
        if (!FileExist(SETTINGS_PATH))
        {
            if (!JsonFileLoader<PNH_StorageProbeConfig>.SaveFile(SETTINGS_PATH, cfg, err))
            {
                Print("[PNH_CORE][STORAGE][ERROR] Cannot create StorageProbe.json: " + err);
                return;
            }
            Print("[PNH_CORE][STORAGE] StorageProbe.json created (probe OFF)");
        }
        else
        {
            if (!JsonFileLoader<PNH_StorageProbeConfig>.LoadFile(SETTINGS_PATH, cfg, err))
            {
                Print("[PNH_CORE][STORAGE][ERROR] Invalid StorageProbe.json; probe not run: " + err);
                return;
            }
        }

        if (!cfg || cfg.SchemaVersion != 1)
        {
            Print("[PNH_CORE][STORAGE][ERROR] Unsupported StorageProbe.json; probe not run");
            return;
        }
        Print("[PNH_CORE][STORAGE] Probe enabled=" + cfg.Enabled.ToString());
        if (!cfg.Enabled)
            return;

        RunProbe();
    }

    protected static void RunProbe()
    {
        ref PNH_StorageProbeRecord record = new PNH_StorageProbeRecord();
        string err;
        if (FileExist(RECORD_PATH))
        {
            if (!JsonFileLoader<PNH_StorageProbeRecord>.LoadFile(RECORD_PATH, record, err))
            {
                Print("[PNH_CORE][STORAGE][ERROR] Record corrupt/unreadable; refusing overwrite: " + err);
                return;
            }
            if (!record || record.SchemaVersion != 1 || record.Marker != "PNH_STORAGE_LAB" || record.Sequence < 0)
            {
                Print("[PNH_CORE][STORAGE][ERROR] Record invalid; refusing overwrite");
                return;
            }
            Print("[PNH_CORE][STORAGE] Previous sequence=" + record.Sequence.ToString());
        }
        else
        {
            Print("[PNH_CORE][STORAGE] No previous record; creating lab record");
        }

        record.Sequence = record.Sequence + 1;
        if (!JsonFileLoader<PNH_StorageProbeRecord>.SaveFile(RECORD_PATH, record, err))
        {
            Print("[PNH_CORE][STORAGE][ERROR] Save failed: " + err);
            return;
        }

        // Re-read from disk to confirm correct serialization in this lab run.
        ref PNH_StorageProbeRecord verified = new PNH_StorageProbeRecord();
        if (!JsonFileLoader<PNH_StorageProbeRecord>.LoadFile(RECORD_PATH, verified, err))
        {
            Print("[PNH_CORE][STORAGE][ERROR] Verification read failed: " + err);
            return;
        }
        if (!verified || verified.SchemaVersion != 1 || verified.Marker != "PNH_STORAGE_LAB" || verified.Sequence != record.Sequence)
        {
            Print("[PNH_CORE][STORAGE][ERROR] Verification mismatch");
            return;
        }
        Print("[PNH_CORE][STORAGE] SAVE+LOAD OK; sequence=" + verified.Sequence.ToString());
        PNH_Diagnostics.Log("Core", "Persistence", "Storage lab probe passed");
    }
};
