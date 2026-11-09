// PNH_Core v0.10.19 - per-PlayerBase in-memory damage -> EEKilled correlation.
// NOT death causation, fatal interception, infected transformation or persistence.
class PNH_FatalDamageCorrelationState
{
    int DamageEvents = 0;
    int LastDamageAtMs = -1;
    string LastZone = "";
    string LastAmmo = "";
    int LastDamageType = -1;
    float LastCalculatedHealth = 0.0;
    float LastGlobalHealth = 0.0;
    bool LastSuperAllows = false;
    bool DeathObserved = false;
};

class PNH_FatalDamageCorrelation
{
    protected static const string CONFIG = "$profile:PNH/Core/FatalDamageCorrelation.json";
    protected static bool s_Enabled = false;
    protected static int s_DamageLogs = 0;
    protected static int s_DeathLogs = 0;
    protected static const int MAX_DAMAGE_LOGS = 20;
    protected static const int MAX_DEATH_LOGS = 10;
    protected static const int RECENT_WINDOW_MS = 15000;

    static void Initialize()
    {
        s_Enabled = false;
        s_DamageLogs = 0;
        s_DeathLogs = 0;
        if (!GetGame() || !GetGame().IsServer()) return;
        if (!FileExist("$profile:PNH")) MakeDirectory("$profile:PNH");
        if (!FileExist("$profile:PNH/Core")) MakeDirectory("$profile:PNH/Core");
        ref PNH_FatalDamageCorrelationConfig cfg = new PNH_FatalDamageCorrelationConfig();
        string err;
        if (!FileExist(CONFIG))
        {
            if (JsonFileLoader<PNH_FatalDamageCorrelationConfig>.SaveFile(CONFIG, cfg, err))
                Print("[PNH_CORE][FDC] FatalDamageCorrelation.json created (OFF)");
            else
                Print("[PNH_CORE][FDC][ERROR] Config creation failed; OFF");
            return;
        }
        if (!JsonFileLoader<PNH_FatalDamageCorrelationConfig>.LoadFile(CONFIG, cfg, err) || !cfg || cfg.SchemaVersion != 1)
        {
            Print("[PNH_CORE][FDC][ERROR] Invalid config; OFF");
            return;
        }
        s_Enabled = cfg.Enabled;
        Print("[PNH_CORE][FDC] Enabled=" + s_Enabled.ToString() + " mode=ENTITY_MEMORY_ONLY recentWindowMs=15000 NO_CAUSE_PROOF NO_DAMAGE_CHANGE NO_WRITE");
    }

    static bool IsEnabled()
    {
        return s_Enabled;
    }

    static void OnDamage(PlayerBase player, PNH_FatalDamageCorrelationState state, TotalDamageResult result, int damageType, string dmgZone, string ammo, bool vanillaAllows)
    {
        if (!s_Enabled || !GetGame() || !GetGame().IsServer() || !player || !state || !result) return;
        if (!player.GetIdentity() || state.DeathObserved) return;
        state.DamageEvents++;
        state.LastDamageAtMs = GetGame().GetTime();
        state.LastZone = dmgZone;
        state.LastAmmo = ammo;
        state.LastDamageType = damageType;
        state.LastCalculatedHealth = result.GetDamage(dmgZone, "Health");
        state.LastGlobalHealth = player.GetHealth("", "Health");
        state.LastSuperAllows = vanillaAllows;
        if (s_DamageLogs >= MAX_DAMAGE_LOGS) return;
        s_DamageLogs++;
        Print("[PNH_CORE][FDC] DAMAGE_OBSERVED zone=" + dmgZone + " damageType=" + damageType.ToString() + " ammo=" + ammo + " calculatedHealth=" + state.LastCalculatedHealth.ToString() + " globalHealthBefore=" + state.LastGlobalHealth.ToString() + " superAllows=" + vanillaAllows.ToString() + " entityEventCount=" + state.DamageEvents.ToString() + " NO_WRITE");
    }

    static void OnKilled(PlayerBase player, PNH_FatalDamageCorrelationState state)
    {
        if (!s_Enabled || !GetGame() || !GetGame().IsServer() || !player) return;
        if (!player.GetIdentity()) return;
        if (state && state.DeathObserved) return;
        if (state) state.DeathObserved = true;
        // Read-only classification of THIS entity, before vanilla super.
        PNH_DeathClassification.OnDeath(player, state);
        if (s_DeathLogs >= MAX_DEATH_LOGS) return;
        s_DeathLogs++;
        if (!state || state.DamageEvents == 0 || state.LastDamageAtMs < 0)
        {
            Print("[PNH_CORE][FDC] EEKILLED_PRE_SUPER correlation=NO_PRIOR_DAMAGE_OBSERVED cause=UNKNOWN NO_CAUSE_PROOF NO_WRITE");
            return;
        }
        int elapsed = GetGame().GetTime() - state.LastDamageAtMs;
        if (elapsed < 0) elapsed = 0;
        string proximity = "STALE_LAST_DAMAGE";
        if (elapsed <= RECENT_WINDOW_MS) proximity = "RECENT_DAMAGE_OBSERVED";
        Print("[PNH_CORE][FDC] EEKILLED_PRE_SUPER correlation=" + proximity + " elapsedMs=" + elapsed.ToString() + " entityEventCount=" + state.DamageEvents.ToString() + " lastZone=" + state.LastZone + " lastAmmo=" + state.LastAmmo + " lastType=" + state.LastDamageType.ToString() + " lastCalculatedHealth=" + state.LastCalculatedHealth.ToString() + " lastGlobalHealthBefore=" + state.LastGlobalHealth.ToString() + " lastSuperAllows=" + state.LastSuperAllows.ToString() + " cause=UNCONFIRMED NO_CAUSE_PROOF NO_WRITE");
    }
};
