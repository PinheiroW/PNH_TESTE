// One-shot explicitly authorized isolated lab JSON write; never a real death journal.
class PNH_LabStagingTestConfig
{
    int SchemaVersion = 1;
    bool Enabled = false;
    bool LabWriteRequested = false;
    string ApprovedSteamID64 = "";
    string ApprovalPhrase = "";
};
