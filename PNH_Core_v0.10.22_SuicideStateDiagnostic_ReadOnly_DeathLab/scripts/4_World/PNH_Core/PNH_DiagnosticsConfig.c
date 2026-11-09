// PNH_Core v0.2 - JSON data model. Fields intentionally match JSON names.
class PNH_DiagnosticsModuleConfig
{
    bool Enabled = false;
    bool Damage = false;
    bool State = false;
    bool Persistence = false;
    bool RPC = false;
};

class PNH_DiagnosticsModulesConfig
{
    ref PNH_DiagnosticsModuleConfig Core;
    ref PNH_DiagnosticsModuleConfig PNH_InfectedRespawn;

    void PNH_DiagnosticsModulesConfig()
    {
        Core = new PNH_DiagnosticsModuleConfig();
        PNH_InfectedRespawn = new PNH_DiagnosticsModuleConfig();
    }
};

class PNH_DiagnosticsConfig
{
    int SchemaVersion = 1;
    bool Enabled = false;
    ref PNH_DiagnosticsModulesConfig Modules;

    void PNH_DiagnosticsConfig()
    {
        Modules = new PNH_DiagnosticsModulesConfig();
    }
};
