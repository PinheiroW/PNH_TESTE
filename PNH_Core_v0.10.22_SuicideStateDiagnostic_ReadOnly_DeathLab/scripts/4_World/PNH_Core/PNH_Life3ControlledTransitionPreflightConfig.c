// v0.10.7 - opt-in, read-only life3->life4 preflight. NEVER authorizes or performs writes.
class PNH_Life3ControlledTransitionPreflightConfig
{
    int SchemaVersion = 1;
    bool Enabled = false;
    int MaxHistoryChecks = 64;
};
