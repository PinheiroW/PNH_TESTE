// AL-03 PROTOTYPE: read-only, one event per JoinThem choice, approved account only.
// Intended order: CF -> PNH_Core -> JoinThem -> PNH_Afterlife_AL03.
class PNH_AL03_ObserverConfig
{
    int SchemaVersion = 1;
    bool Enabled = false;
    string ApprovedSteamID64 = "";
};

modded class PlayerBase
{
    override void JTChoose(bool join)
    {
        // Capture immediately before the original JoinThem selection handler.
        int stateBefore = m_JTState;
        string idBefore = PNH35_GetCharacterId();

        // MUST execute JoinThem behavior exactly once, even if config is absent.
        super.JTChoose(join);

        // No polling, no writes, no changes to health, damage, inventory, or lifecycle.
        if (!GetGame() || !GetGame().IsServer()) return;
        if (!join || stateBefore != 1) return;

        string configPath = "$profile:PNH/Afterlife/AL03.json";
        if (!FileExist(configPath)) return;

        ref PNH_AL03_ObserverConfig cfg = new PNH_AL03_ObserverConfig();
        string error;
        if (!JsonFileLoader<PNH_AL03_ObserverConfig>.LoadFile(configPath, cfg, error) || !cfg) return;
        if (cfg.SchemaVersion != 1 || !cfg.Enabled || cfg.ApprovedSteamID64.Length() != 17) return;

        PlayerIdentity identity = GetIdentity();
        if (!identity || identity.GetPlainId() != cfg.ApprovedSteamID64) return;

        string idAfter = PNH35_GetCharacterId();
        bool beforePresent = idBefore != "";
        bool afterPresent = idAfter != "";
        bool sameId = beforePresent && afterPresent && idBefore == idAfter;
        bool joined = m_JTState == 2;

        // Deliberately avoid printing account identifiers or full character IDs.
        Print("[PNH_AFTERLIFE][AL03] event=JOIN_CHOICE stateBefore=" + stateBefore.ToString() + " stateAfter=" + m_JTState.ToString() + " joined=" + joined.ToString() + " idBeforePresent=" + beforePresent.ToString() + " idAfterPresent=" + afterPresent.ToString() + " sameCharacterId=" + sameId.ToString() + " mode=READ_ONLY NO_WRITE NO_LIFE_COMMIT");
    }
};
