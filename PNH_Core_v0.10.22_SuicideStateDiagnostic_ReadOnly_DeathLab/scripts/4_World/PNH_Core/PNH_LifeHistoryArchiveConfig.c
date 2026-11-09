// v0.9.1 - Fail-closed archive options. OFF by default.
class PNH_LifeHistoryArchiveConfig
{
    int SchemaVersion = 1;
    bool Enabled = false;
    bool ArchiveEnabled = false;
    string ApprovedSteamID64 = "";
    int ExpectedSourceLifeId = 0;
    int ExpectedSourceRevision = 0;
};
