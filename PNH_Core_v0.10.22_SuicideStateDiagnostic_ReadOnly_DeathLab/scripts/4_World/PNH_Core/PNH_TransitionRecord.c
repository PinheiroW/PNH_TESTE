// Independent journal. Does NOT represent the current authoritative LifeId.
class PNH_TransitionRecord
{
    int SchemaVersion = 1;
    string SteamID64 = "";
    int SourceLifeId = 0;
    int SourceRevision = 0;
    string SourceState = "";
    string Phase = "DEATH_RECORDED";
};
