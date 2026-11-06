// AZ-2652: server observation only; no names, raw identifiers or gameplay writes.
class AZ_VampCryptLog
{
    static void Observe(PlayerBase p, string route = "crypt_box")
    {
        if (!GetGame() || !GetGame().IsServer() || !p)
            return;
        if (!p.IsAlive() || !p.IsPlayerSelected() || p.IsPlayerDisconnected())
        {
            p.AZ_ResetCryptLog();
            return;
        }
        PlayerIdentity identity = p.GetIdentity();
        if (!identity || identity.GetPlainId() == "")
            return;
        // Inside() may call Site(); only enter after normal lifecycle initialization.
        if (!AZ_VampCrypt.IsReady())
            return;
        bool inside = AZ_VampCrypt.Inside(p.GetPosition());
        // Reconcile restart/reconnect presence silently, without claiming a crossing.
        if (!p.m_AZCryptLogSeen)
        {
            p.m_AZCryptLogSeen = true;
            p.m_AZCryptLogInside = inside;
            return;
        }
        if (inside == p.m_AZCryptLogInside)
            return;
        p.m_AZCryptLogInside = inside;
        string pid = AZ_VampEvidence.SteamHash(identity.GetPlainId());
        string role = "human";
        if (p.m_AZVampire) role = "vampire";
        string kind = "leave";
        if (inside) kind = "arrive";
        Print("[CRYPT] " + kind + " pid=" + pid + " role=" + role + " route=" + route + " t=" + GetGame().GetTime());
    }
}

modded class PlayerBase
{
    bool m_AZCryptLogSeen;
    bool m_AZCryptLogInside;

    void AZ_ResetCryptLog()
    {
        m_AZCryptLogSeen = false;
        m_AZCryptLogInside = false;
    }

    override void EEKilled(Object killer)
    {
        AZ_ResetCryptLog();
        super.EEKilled(killer);
    }

    override void OnDisconnect()
    {
        AZ_ResetCryptLog();
        super.OnDisconnect();
    }

    override void OnScheduledTick(float deltaTime)
    {
        super.OnScheduledTick(deltaTime);
        // Covers direct/admin/external teleports and ordinary box crossings.
        AZ_VampCryptLog.Observe(this);
    }
}
