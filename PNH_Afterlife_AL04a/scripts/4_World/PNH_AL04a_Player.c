// AL-04a: observational only. Never restores an infected state or writes a life record.
// Ordering: CF, PNH_Core_v0.10.36, JoinThem, AL03, AL04a.
class PNH_AL04a_Config
{
    int SchemaVersion = 1;
    bool Enabled = false;
    string ApprovedSteamID64 = "";
};

modded class PlayerBase
{
    // Read config only on the rare lifecycle events, never from OnUpdate/JTTick.
    bool PNH_AL04a_Approved()
    {
        if (!GetGame() || !GetGame().IsServer()) return false;
        string path = "$profile:PNH/Afterlife/AL04a.json";
        if (!FileExist(path)) return false;

        ref PNH_AL04a_Config cfg = new PNH_AL04a_Config();
        string error;
        if (!JsonFileLoader<PNH_AL04a_Config>.LoadFile(path, cfg, error) || !cfg) return false;
        if (cfg.SchemaVersion != 1 || !cfg.Enabled || cfg.ApprovedSteamID64.Length() != 17) return false;

        PlayerIdentity identity = GetIdentity();
        if (!identity) return false;
        return identity.GetPlainId() == cfg.ApprovedSteamID64;
    }

    override void JTChoose(bool join)
    {
        int before = m_JTState;
        string beforeId = PNH35_GetCharacterId();
        // Critical: original JoinThem behavior runs exactly once.
        super.JTChoose(join);
        if (!PNH_AL04a_Approved()) return;
        string afterId = PNH35_GetCharacterId();
        bool eligible = before == 1 && IsAlive();
        bool idPresent = beforeId != "" && afterId != "";
        bool sameId = idPresent && beforeId == afterId;
        bool joined = join && before == 1 && m_JTState == 2;
        Print("[PNH_AFTERLIFE][AL04a] event=JT_CHOICE join=" + join.ToString() + " stateBefore=" + before.ToString() + " stateAfter=" + m_JTState.ToString() + " eligible=" + eligible.ToString() + " joined=" + joined.ToString() + " idPresent=" + idPresent.ToString() + " sameCharacterId=" + sameId.ToString() + " NO_WRITE NO_RESTORE NO_LIFE_COMMIT");
    }

    override void EEKilled(Object killer)
    {
        // Save pre-super state because JoinThem changes state 2/3 to 5 in EEKilled.
        int stateBefore = m_JTState;
        bool approvedBefore = PNH_AL04a_Approved();
        bool idBeforePresent = PNH35_GetCharacterId() != "";
        super.EEKilled(killer);
        if (!approvedBefore) return;
        Print("[PNH_AFTERLIFE][AL04a] event=EEKILLED stateBefore=" + stateBefore.ToString() + " stateAfter=" + m_JTState.ToString() + " idBeforePresent=" + idBeforePresent.ToString() + " NO_WRITE NO_RESTORE NO_LIFE_COMMIT");
    }

};
