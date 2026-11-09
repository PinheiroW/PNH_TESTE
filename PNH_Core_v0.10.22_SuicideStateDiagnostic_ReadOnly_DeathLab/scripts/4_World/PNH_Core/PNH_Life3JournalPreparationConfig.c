// v0.10.9: opt-in, read-only journal preparation. No player/journal writes.
class PNH_Life3JournalPreparationConfig
{
    int SchemaVersion = 1;
    bool Enabled = false;
    int MaxHistoryChecks = 64;
};
