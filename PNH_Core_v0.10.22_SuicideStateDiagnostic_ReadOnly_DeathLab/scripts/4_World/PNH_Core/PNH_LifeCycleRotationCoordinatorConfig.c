// v0.10.3: read-only rotation preflight; disabled by default.
class PNH_LifeCycleRotationCoordinatorConfig
{
    int SchemaVersion = 1;
    bool Enabled = false;
    int MaxHistoryChecks = 64;
};
