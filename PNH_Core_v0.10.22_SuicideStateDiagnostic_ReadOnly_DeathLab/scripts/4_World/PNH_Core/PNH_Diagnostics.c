// PNH_Core v0.2 - Server-only JSON-controlled optional diagnostics.
// No timers, per-frame operations, or disk writes after initial load.
class PNH_Diagnostics
{
    protected static ref PNH_DiagnosticsConfig s_Config;
    protected static bool s_Ready = false;
    protected static const string CONFIG_DIR = "$profile:PNH";
    protected static const string CORE_DIR = "$profile:PNH/Core";
    protected static const string CONFIG_PATH = "$profile:PNH/Core/Diagnostics.json";

    static void Initialize()
    {
        // Fail closed before reading configuration.
        s_Ready = false;
        s_Config = new PNH_DiagnosticsConfig();

        if (!FileExist(CONFIG_DIR))
            MakeDirectory(CONFIG_DIR);
        if (!FileExist(CORE_DIR))
            MakeDirectory(CORE_DIR);

        string error;
        if (!FileExist(CONFIG_PATH))
        {
            if (!JsonFileLoader<PNH_DiagnosticsConfig>.SaveFile(CONFIG_PATH, s_Config, error))
                Print("[PNH_CORE][ERROR] Could not create Diagnostics.json: " + error);
            else
                Print("[PNH_CORE] Diagnostics.json created (diagnostics OFF)");
        }
        else
        {
            ref PNH_DiagnosticsConfig loaded = new PNH_DiagnosticsConfig();
            if (!JsonFileLoader<PNH_DiagnosticsConfig>.LoadFile(CONFIG_PATH, loaded, error))
            {
                Print("[PNH_CORE][ERROR] Invalid Diagnostics.json; diagnostics stay OFF: " + error);
                return;
            }
            if (!loaded || loaded.SchemaVersion != 1 || !loaded.Modules || !loaded.Modules.Core || !loaded.Modules.PNH_InfectedRespawn)
            {
                Print("[PNH_CORE][ERROR] Unsupported/incomplete Diagnostics.json; diagnostics stay OFF");
                return;
            }
            s_Config = loaded;
        }

        s_Ready = true;
        Print("[PNH_CORE] Diagnostics configuration ready; enabled=" + s_Config.Enabled.ToString());
    }

    static bool EnabledFor(string moduleName, string channel)
    {
        if (!s_Ready || !s_Config || !s_Config.Enabled || !s_Config.Modules)
            return false;

        PNH_DiagnosticsModuleConfig moduleConfig;
        if (moduleName == "Core")
            moduleConfig = s_Config.Modules.Core;
        else if (moduleName == "PNH_InfectedRespawn")
            moduleConfig = s_Config.Modules.PNH_InfectedRespawn;
        else
            return false;

        if (!moduleConfig || !moduleConfig.Enabled)
            return false;

        if (channel == "State") return moduleConfig.State;
        if (channel == "Damage") return moduleConfig.Damage;
        if (channel == "Persistence") return moduleConfig.Persistence;
        if (channel == "RPC") return moduleConfig.RPC;
        return false;
    }

    static void Log(string moduleName, string channel, string message)
    {
        if (!EnabledFor(moduleName, channel))
            return;
        Print("[PNH_DIAG][" + moduleName + "][" + channel + "] " + message);
    }
};
