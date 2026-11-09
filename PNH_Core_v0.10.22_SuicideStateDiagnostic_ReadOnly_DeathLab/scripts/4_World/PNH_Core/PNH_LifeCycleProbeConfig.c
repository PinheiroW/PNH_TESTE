// PNH_Core v0.5 LAB ONLY. Fictional account / life, no player identity or inventory.
class PNH_LifeCycleProbeConfig
{
    int SchemaVersion = 1;
    bool Enabled = false;
    string Mode = "CREATE";
};

class PNH_LifeCycleProbeRecord
{
    int SchemaVersion = 1;
    string AccountKey = "LAB_ACCOUNT";
    int LifeId = 1;
    string State = "INFECTED";
    int Revision = 1;
};
