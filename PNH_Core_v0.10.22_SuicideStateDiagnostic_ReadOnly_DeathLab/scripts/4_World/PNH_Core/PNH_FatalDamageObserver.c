// PNH_Core v0.10.18 - passive damage diagnostic, NO damage/health/life mutations.
// DiagnosticMode is for isolated DeathLab only; never enable on production.
class PNH_FatalDamageObserver
{
    protected static const string CONFIG = "$profile:PNH/Core/FatalDamageObserver.json";
    protected static bool s_Enabled = false;
    protected static bool s_DiagnosticMode = false;
    protected static int s_Logged = 0;
    protected static int s_DiagnosticLogged = 0;
    protected static bool s_LimitNoted = false;
    protected static const int MAX_LOGS_PER_PROCESS = 40;
    protected static const int MAX_DIAGNOSTIC_LOGS = 20;

    static void Initialize()
    {
        s_Enabled = false;
        s_DiagnosticMode = false;
        s_Logged = 0;
        s_DiagnosticLogged = 0;
        s_LimitNoted = false;
        if (!GetGame() || !GetGame().IsServer()) return;
        if (!FileExist("$profile:PNH")) MakeDirectory("$profile:PNH");
        if (!FileExist("$profile:PNH/Core")) MakeDirectory("$profile:PNH/Core");

        ref PNH_FatalDamageObserverConfig cfg = new PNH_FatalDamageObserverConfig();
        string err;
        if (!FileExist(CONFIG))
        {
            if (JsonFileLoader<PNH_FatalDamageObserverConfig>.SaveFile(CONFIG, cfg, err))
                Print("[PNH_CORE][FDO] FatalDamageObserver.json created (OFF)");
            else
                Print("[PNH_CORE][FDO][ERROR] Config creation failed; OFF");
            return;
        }
        if (!JsonFileLoader<PNH_FatalDamageObserverConfig>.LoadFile(CONFIG, cfg, err) || !cfg || cfg.SchemaVersion != 1)
        {
            Print("[PNH_CORE][FDO][ERROR] Invalid config; OFF");
            return;
        }
        s_Enabled = cfg.Enabled;
        s_DiagnosticMode = cfg.DiagnosticMode;
        Print("[PNH_CORE][FDO] Enabled=" + s_Enabled.ToString() + " DiagnosticMode=" + s_DiagnosticMode.ToString() + " maxNormal=40 maxDiagnostic=20 NO_DAMAGE_CHANGE NO_WRITE");
    }

    static void Observe(PlayerBase player, TotalDamageResult result, int damageType, string dmgZone, string ammo, bool vanillaAllows)
    {
        if (!s_Enabled || !GetGame() || !GetGame().IsServer() || !player || !result) return;
        if (!player.GetIdentity()) return;
        if (s_Logged >= MAX_LOGS_PER_PROCESS)
        {
            if (!s_LimitNoted)
            {
                s_LimitNoted = true;
                Print("[PNH_CORE][FDO] LOG_LIMIT_REACHED NO_WRITE");
            }
            return;
        }

        // These are damage calculation values, NOT an authoritative fatality decision.
        float hitHealth = result.GetDamage(dmgZone, "Health");
        float currentGlobal = player.GetHealth("", "Health");
        bool isHead = dmgZone == "Head" || dmgZone == "Brain";
        bool meetsNormalFilter = isHead || hitHealth >= 30.0;
        bool heuristicLethal = hitHealth >= currentGlobal && currentGlobal > 0.0;

        if (!meetsNormalFilter)
        {
            if (!s_DiagnosticMode) return;
            if (s_DiagnosticLogged >= MAX_DIAGNOSTIC_LOGS) return;
            s_DiagnosticLogged++;
            s_Logged++;
            Print("[PNH_CORE][FDO] DAMAGE_DIAGNOSTIC zone=" + dmgZone + " damageType=" + damageType.ToString() + " ammo=" + ammo + " calculatedHealth=" + hitHealth.ToString() + " currentGlobalHealth=" + currentGlobal.ToString() + " superAllows=" + vanillaAllows.ToString() + " belowNormalFilter=true heuristicLethal=" + heuristicLethal.ToString() + " NOT_FATAL_PROOF NO_WRITE");
            return;
        }

        s_Logged++;
        Print("[PNH_CORE][FDO] DAMAGE_CALCULATED zone=" + dmgZone + " damageType=" + damageType.ToString() + " ammo=" + ammo + " calculatedHealth=" + hitHealth.ToString() + " currentGlobalHealth=" + currentGlobal.ToString() + " superAllows=" + vanillaAllows.ToString() + " heuristicLethal=" + heuristicLethal.ToString() + " HEURISTIC_ONLY NOT_FATAL_PROOF NO_WRITE");
    }
};
