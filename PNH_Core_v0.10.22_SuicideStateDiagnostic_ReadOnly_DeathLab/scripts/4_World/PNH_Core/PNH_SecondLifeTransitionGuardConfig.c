// v0.9.4 second life LAB - opt-in, fail closed.
class PNH_SecondLifeTransitionGuardConfig
{
    int SchemaVersion = 1;
    bool Enabled = false;
    bool JournalWriteApproved = false;
    string ApprovedSteamID64 = "";
    int ExpectedSourceLifeId = 2;
    int ExpectedSourceRevision = 2;
};
