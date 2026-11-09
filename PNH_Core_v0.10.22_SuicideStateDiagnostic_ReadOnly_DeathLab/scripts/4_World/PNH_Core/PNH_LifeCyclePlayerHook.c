// PNH_Core v0.8.1 - minimal observation hook. Vanilla behavior remains authoritative.
modded class PlayerBase
{
    // Per-entity state: never keyed solely by persistent LifeId/Revision.
    protected ref PNH_FatalDamageCorrelationState m_PNH_FDCState;
    protected ref PNH_DeathMechanismState m_PNH_DMOState;
    // v0.10.17: passive server-side damage observation. Never override vanilla decision.
    override bool EEOnDamageCalculated(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
    {
        bool vanillaAllows = super.EEOnDamageCalculated(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
        if (GetGame() && GetGame().IsServer())
            PNH_FatalDamageObserver.Observe(this, damageResult, damageType, dmgZone, ammo, vanillaAllows);
        if (GetGame() && GetGame().IsServer() && PNH_FatalDamageCorrelation.IsEnabled() && GetIdentity())
        {
            if (!m_PNH_FDCState) m_PNH_FDCState = new PNH_FatalDamageCorrelationState();
            PNH_FatalDamageCorrelation.OnDamage(this, m_PNH_FDCState, damageResult, damageType, dmgZone, ammo, vanillaAllows);
        }
        if (GetGame() && GetGame().IsServer() && PNH_DeathMechanismObserver.IsEnabled() && GetIdentity())
        {
            if (!m_PNH_DMOState) m_PNH_DMOState = new PNH_DeathMechanismState();
            PNH_DeathMechanismObserver.OnDamage(this, m_PNH_DMOState, damageResult, damageType, dmgZone, ammo, vanillaAllows);
        }
        return vanillaAllows;
    }

    override void EEKilled(Object killer)
    {
        // Capture identity before vanilla cleanup may detach it.
        if (GetGame() && GetGame().IsServer() && PNH_LifeCycleMonitor.IsEnabled())
            PNH_LifeCycleMonitor.OnKilled(this, GetIdentity());
        if (GetGame() && GetGame().IsServer() && PNH_TransitionJournal.IsEnabled())
            PNH_TransitionJournal.OnKilled(this, GetIdentity());
        // v0.10.15: additional read-only observer, never a commit trigger.
        if (GetGame() && GetGame().IsServer())
            PNH_DeathEventObserver.OnPlayerKilled(this);
        // Correlation uses this exact PlayerBase instance; EEKilled occurs before vanilla super here.
        if (GetGame() && GetGame().IsServer())
            PNH_FatalDamageCorrelation.OnKilled(this, m_PNH_FDCState);
        if (GetGame() && GetGame().IsServer())
            PNH_DeathMechanismObserver.OnKilled(this, m_PNH_DMOState);
        super.EEKilled(killer);
    }
};
