// PNH_Core v0.4 - opt-in LAB probe configuration (server only).
// Run is disabled by default. Never stores player data.
class PNH_StorageProbeConfig
{
    int SchemaVersion = 1;
    bool Enabled = false;
};

class PNH_StorageProbeRecord
{
    int SchemaVersion = 1;
    int Sequence = 0;
    string Marker = "PNH_STORAGE_LAB";
};
