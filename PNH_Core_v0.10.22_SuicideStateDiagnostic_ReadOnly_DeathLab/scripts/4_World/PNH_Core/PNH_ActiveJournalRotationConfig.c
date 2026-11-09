// v0.9.2 - One-shot rotation laboratory. Disabled by default.
class PNH_ActiveJournalRotationConfig
{
    int SchemaVersion = 1;
    bool Enabled = false;
    bool RotationEnabled = false;
    string ApprovedSteamID64 = "";
    int ExpectedSourceLifeId = 0;
    int ExpectedSourceRevision = 0;
};
