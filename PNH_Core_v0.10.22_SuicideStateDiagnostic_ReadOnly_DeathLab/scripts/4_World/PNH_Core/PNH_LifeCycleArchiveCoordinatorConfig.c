// v0.10.2 — dynamic sourceLife archive with one explicit LAB identity approval.
// ArchiveEnabled is OFF by default; never deletes or rotates the source journal.
class PNH_LifeCycleArchiveCoordinatorConfig
{
    int SchemaVersion = 1;
    bool Enabled = false;
    bool ArchiveEnabled = false;
    string ApprovedSteamID64 = "";
    int MaxHistoryChecks = 64;
};
