// v0.10.8: opt-in, read-only transition plan. No life/diary writes.
class PNH_Life3TransitionPlanConfig
{
    int SchemaVersion = 1;
    bool Enabled = false;
    int MaxHistoryChecks = 64;
};
