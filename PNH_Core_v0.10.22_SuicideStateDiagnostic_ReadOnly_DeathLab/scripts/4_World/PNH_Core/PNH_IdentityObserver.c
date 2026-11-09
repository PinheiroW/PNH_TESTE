// PNH_Core v0.6 - opt-in observer for real server-side player lifecycle events.
// This is a read-only LAB. No player persistence, no mutation and no per-frame work.
class PNH_IdentityObserver
{
    protected static const string ROOT_DIR = "$profile:PNH";
    protected static const string CORE_DIR = "$profile:PNH/Core";
    protected static const string CONFIG_PATH = "$profile:PNH/Core/IdentityObserver.json";

    static bool Initialize()
    {
        if (!GetGame() || !GetGame().IsServer())
            return false;

        if (!FileExist(ROOT_DIR)) MakeDirectory(ROOT_DIR);
        if (!FileExist(CORE_DIR)) MakeDirectory(CORE_DIR);

        ref PNH_IdentityObserverConfig cfg = new PNH_IdentityObserverConfig();
        string err;
        if (!FileExist(CONFIG_PATH))
        {
            if (!JsonFileLoader<PNH_IdentityObserverConfig>.SaveFile(CONFIG_PATH, cfg, err))
                Print("[PNH_CORE][IDENTITY][ERROR] Cannot create IdentityObserver.json: " + err);
            else
                Print("[PNH_CORE][IDENTITY] IdentityObserver.json created (observer OFF)");
            return false;
        }

        if (!JsonFileLoader<PNH_IdentityObserverConfig>.LoadFile(CONFIG_PATH, cfg, err))
        {
            Print("[PNH_CORE][IDENTITY][ERROR] Invalid IdentityObserver.json; observer OFF: " + err);
            return false;
        }
        if (!cfg || cfg.SchemaVersion != 1)
        {
            Print("[PNH_CORE][IDENTITY][ERROR] Unsupported IdentityObserver.json; observer OFF");
            return false;
        }
        Print("[PNH_CORE][IDENTITY] Observer enabled=" + cfg.Enabled.ToString());
        return cfg.Enabled;
    }

    static void Observe(string eventName, CF_EventArgs args)
    {
        if (!GetGame() || !GetGame().IsServer()) return;
        CF_EventPlayerArgs playerArgs = CF_EventPlayerArgs.Cast(args);
        if (!playerArgs)
        {
            Print("[PNH_CORE][IDENTITY][WARN] " + eventName + " missing event data");
            return;
        }

        PlayerBase player = playerArgs.Player;
        PlayerIdentity identity = playerArgs.Identity;
        if (!identity && player)
            identity = player.GetIdentity();
        if (!identity)
        {
            Print("[PNH_CORE][IDENTITY][WARN] " + eventName + " identity unavailable");
            return;
        }

        // GetId is a hashed stable account identifier. Never use GetPlainId in logs.
        // Do not infer life identity from the session-only GetPlayerId.
        string accountKey = identity.GetId();
        if (accountKey == "")
        {
            Print("[PNH_CORE][IDENTITY][WARN] " + eventName + " empty account key");
            return;
        }

        if (!player)
        {
            Print("[PNH_CORE][IDENTITY] " + eventName + " account=" + accountKey + " player=null");
            return;
        }
        Print("[PNH_CORE][IDENTITY] " + eventName + " account=" + accountKey + " player=" + player.ToString() + " alive=" + player.IsAlive().ToString());
    }
};
