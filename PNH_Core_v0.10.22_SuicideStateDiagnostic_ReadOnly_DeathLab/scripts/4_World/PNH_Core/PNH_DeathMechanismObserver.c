// PNH_Core v0.10.22 - passive read-only damage/death vitals and suicide-state snapshots.
// Blood/Health/Shock are OBSERVATIONS only; never infer cause of death.
// No timers, periodic ticks, journal writes, transform or health mutations.
class PNH_DeathMechanismState
{
    int EventCount = 0;
    int LastAtMs = -1;
    string LastZone = "";
    string LastAmmo = "";
    int LastDamageType = -1;
    float LastCalculatedHealth = 0.0;
    float HealthBefore = -1.0;
    float BloodBefore = -1.0;
    float ShockBefore = -1.0;
    bool SuperAllows = false;
    bool HeadZoneEverObserved = false;
    bool DeathObserved = false;
};

class PNH_DeathMechanismObserver
{
    protected static const string CONFIG = "$profile:PNH/Core/DeathMechanismObserver.json";
    protected static bool s_Enabled = false;
    protected static int s_DamageLogCount = 0;
    protected static int s_DeathLogCount = 0;
    protected static const int MAX_DAMAGE_LOGS = 12;
    protected static const int MAX_DEATH_LOGS = 12;

    static void Initialize()
    {
        s_Enabled = false;
        s_DamageLogCount = 0;
        s_DeathLogCount = 0;
        if (!GetGame() || !GetGame().IsServer()) return;
        if (!FileExist("$profile:PNH")) MakeDirectory("$profile:PNH");
        if (!FileExist("$profile:PNH/Core")) MakeDirectory("$profile:PNH/Core");
        ref PNH_DeathMechanismObserverConfig cfg = new PNH_DeathMechanismObserverConfig();
        string err;
        if (!FileExist(CONFIG))
        {
            if (JsonFileLoader<PNH_DeathMechanismObserverConfig>.SaveFile(CONFIG, cfg, err))
                Print("[PNH_CORE][DMO] DeathMechanismObserver.json created (OFF)");
            else
                Print("[PNH_CORE][DMO][ERROR] Config creation failed; OFF");
            return;
        }
        if (!JsonFileLoader<PNH_DeathMechanismObserverConfig>.LoadFile(CONFIG, cfg, err) || !cfg || cfg.SchemaVersion != 1)
        {
            Print("[PNH_CORE][DMO][ERROR] Invalid config; OFF");
            return;
        }
        s_Enabled = cfg.Enabled;
        Print("[PNH_CORE][DMO] Enabled=" + s_Enabled.ToString() + " mode=VITALS_AT_DAMAGE_AND_DEATH_ONLY noTimers=true NO_CAUSE_PROOF NO_DAMAGE_CHANGE NO_WRITE");
    }

    static bool IsEnabled()
    {
        return s_Enabled;
    }

    static void OnDamage(PlayerBase player, PNH_DeathMechanismState state, TotalDamageResult result, int damageType, string dmgZone, string ammo, bool vanillaAllows)
    {
        if (!s_Enabled || !GetGame() || !GetGame().IsServer() || !player || !state || !result) return;
        if (!player.GetIdentity() || state.DeathObserved) return;
        state.EventCount++;
        state.LastAtMs = GetGame().GetTime();
        state.LastZone = dmgZone;
        state.LastAmmo = ammo;
        state.LastDamageType = damageType;
        state.LastCalculatedHealth = result.GetDamage(dmgZone, "Health");
        state.HealthBefore = player.GetHealth("", "Health");
        state.BloodBefore = player.GetHealth("", "Blood");
        state.ShockBefore = player.GetHealth("", "Shock");
        state.SuperAllows = vanillaAllows;
        if (dmgZone == "Head" || dmgZone == "Brain") state.HeadZoneEverObserved = true;
        if (s_DamageLogCount >= MAX_DAMAGE_LOGS) return;
        s_DamageLogCount++;
        Print("[PNH_CORE][DMO] DAMAGE_SNAPSHOT zone=" + state.LastZone + " ammo=" + state.LastAmmo + " damageType=" + damageType.ToString() + " calculatedHealth=" + state.LastCalculatedHealth.ToString() + " healthBefore=" + state.HealthBefore.ToString() + " bloodBefore=" + state.BloodBefore.ToString() + " shockBefore=" + state.ShockBefore.ToString() + " superAllows=" + vanillaAllows.ToString() + " headZoneObserved=" + state.HeadZoneEverObserved.ToString() + " entityEventCount=" + state.EventCount.ToString() + " NO_WRITE");
    }

    static void OnKilled(PlayerBase player, PNH_DeathMechanismState state)
    {
        if (!s_Enabled || !GetGame() || !GetGame().IsServer() || !player) return;
        if (!player.GetIdentity()) return;
        if (state && state.DeathObserved) return;
        if (state) state.DeathObserved = true;
        if (s_DeathLogCount >= MAX_DEATH_LOGS) return;
        bool suicideState = player.CommitedSuicide();
        s_DeathLogCount++;
        int count = 0;
        int elapsed = -1;
        string lastZone = "NONE";
        string lastAmmo = "NONE";
        bool observedHead = false;
        float bloodAtLastHit = -1.0;
        if (state)
        {
            count = state.EventCount;
            observedHead = state.HeadZoneEverObserved;
            bloodAtLastHit = state.BloodBefore;
            if (count > 0)
            {
                lastZone = state.LastZone;
                lastAmmo = state.LastAmmo;
                elapsed = GetGame().GetTime() - state.LastAtMs;
                if (elapsed < 0) elapsed = 0;
            }
        }
        Print("[PNH_CORE][DMO] EEKILLED_PRE_SUPER health=" + player.GetHealth("", "Health").ToString() + " blood=" + player.GetHealth("", "Blood").ToString() + " shock=" + player.GetHealth("", "Shock").ToString() + " capturedEvents=" + count.ToString() + " elapsedMs=" + elapsed.ToString() + " lastZone=" + lastZone + " lastAmmo=" + lastAmmo + " bloodAtLastDamage=" + bloodAtLastHit.ToString() + " observedHeadZone=" + observedHead.ToString() + " suicideState=" + suicideState.ToString() + " headCause=UNKNOWN bleedingCause=UNKNOWN cause=UNCONFIRMED NO_CAUSE_PROOF NO_TRANSFORM NO_WRITE");
    }
};
