// v0.8.3. Compatibility with existing v0.8.2 JSON (missing fields use OFF defaults).
class PNH_SafeLifeCommitConfig
{
    int SchemaVersion = 1;
    bool Enabled = false;
    bool ApplyEnabled = false;
    bool RecoveryEnabled = false;
    string RecoverySteamID64 = "";
    int ExpectedSourceLifeId = 0;
    int ExpectedSourceRevision = 0;
};
