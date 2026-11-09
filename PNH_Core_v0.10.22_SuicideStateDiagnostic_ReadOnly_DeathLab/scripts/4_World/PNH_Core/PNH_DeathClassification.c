// PNH_Core v0.10.20 - diagnostic evidence classification only.
// Never authorizes transformation or identifies a verified cause of death.
class PNH_DeathClassification
{
    protected static const string CONFIG = "$profile:PNH/Core/DeathClassification.json";
    protected static bool s_Enabled = false;
    protected static int s_Classified = 0;
    protected static const int MAX_CLASSIFICATION_LOGS = 15;
    protected static const int RECENT_WINDOW_MS = 15000;

    static void Initialize()
    {
        s_Enabled = false;
        s_Classified = 0;
        if (!GetGame() || !GetGame().IsServer()) return;
        if (!FileExist("$profile:PNH")) MakeDirectory("$profile:PNH");
        if (!FileExist("$profile:PNH/Core")) MakeDirectory("$profile:PNH/Core");
        ref PNH_DeathClassificationConfig cfg = new PNH_DeathClassificationConfig();
        string err;
        if (!FileExist(CONFIG))
        {
            if (JsonFileLoader<PNH_DeathClassificationConfig>.SaveFile(CONFIG, cfg, err))
                Print("[PNH_CORE][DCL] DeathClassification.json created (OFF)");
            else
                Print("[PNH_CORE][DCL][ERROR] Config creation failed; OFF");
            return;
        }
        if (!JsonFileLoader<PNH_DeathClassificationConfig>.LoadFile(CONFIG, cfg, err) || !cfg || cfg.SchemaVersion != 1)
        {
            Print("[PNH_CORE][DCL][ERROR] Invalid config; OFF");
            return;
        }
        s_Enabled = cfg.Enabled;
        Print("[PNH_CORE][DCL] Enabled=" + s_Enabled.ToString() + " mode=DEATH_EVIDENCE_ONLY requiresFDC=true NO_CAUSE_PROOF NO_TRANSFORM NO_WRITE");
    }

    static void OnDeath(PlayerBase player, PNH_FatalDamageCorrelationState state)
    {
        if (!s_Enabled || !GetGame() || !GetGame().IsServer() || !player) return;
        if (!player.GetIdentity()) return;
        if (s_Classified >= MAX_CLASSIFICATION_LOGS) return;
        s_Classified++;

        string classification = "INSUFFICIENT_EVIDENCE";
        string evidence = "RECENT_NONLETHAL_OR_UNCERTAIN";
        int elapsed = -1;
        int count = 0;
        if (!state || state.DamageEvents <= 0 || state.LastDamageAtMs < 0)
        {
            classification = "DEATH_WITHOUT_OBSERVED_DAMAGE";
            evidence = "NO_CAPTURED_DAMAGE";
        }
        else
        {
            count = state.DamageEvents;
            elapsed = GetGame().GetTime() - state.LastDamageAtMs;
            if (elapsed < 0) elapsed = 0;
            if (elapsed > RECENT_WINDOW_MS)
            {
                classification = "STALE_DAMAGE";
                evidence = "OUTSIDE_15S_WINDOW";
            }
            else if (state.LastSuperAllows && state.LastGlobalHealth > 0.0 && state.LastCalculatedHealth >= state.LastGlobalHealth)
            {
                classification = "POTENTIALLY_LETHAL_DAMAGE";
                evidence = "HEALTH_COMPARISON_HEURISTIC";
            }
        }

        Print("[PNH_CORE][DCL] EEKILLED_PRE_SUPER classification=" + classification + " evidence=" + evidence + " elapsedMs=" + elapsed.ToString() + " capturedEvents=" + count.ToString() + " eligibility=UNDETERMINED cause=UNCONFIRMED NO_CAUSE_PROOF NO_TRANSFORM NO_WRITE");
    }
};
