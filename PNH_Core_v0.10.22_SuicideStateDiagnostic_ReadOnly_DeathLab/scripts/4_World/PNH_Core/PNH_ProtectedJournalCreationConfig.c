// v0.10.10: creation contract preflight ONLY. WriteEnabled is rejected when true.
class PNH_ProtectedJournalCreationConfig
{
    int SchemaVersion = 1;
    bool Enabled = false;
    bool WriteEnabled = false;
    string ApprovedSteamID64 = "";
    int MaxHistoryChecks = 64;
};
