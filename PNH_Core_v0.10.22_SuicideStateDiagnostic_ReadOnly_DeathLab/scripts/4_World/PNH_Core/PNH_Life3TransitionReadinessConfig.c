// v0.10.6 - read-only preflight for the next transition, disabled by default.
class PNH_Life3TransitionReadinessConfig
{
    int SchemaVersion = 1;
    bool Enabled = false;
    int MaxHistoryChecks = 64;
};
