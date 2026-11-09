// v0.10.4: disabled by default. Write requires explicit identity approval.
class PNH_ProtectedJournalRotationConfig
{
    int SchemaVersion = 1;
    bool Enabled = false;
    bool RotationEnabled = false;
    string ApprovedSteamID64 = "";
    int MaxHistoryChecks = 64;
};
