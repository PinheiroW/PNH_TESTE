// Player animation on the infected rig, with a bounded, prevalidated trajectory.
// Native StartCommand_Vault requires native AI vault-link data and cannot be used
// safely with the Humans' script-selected destinations.
class HumansVaultCommand extends DayZInfectedCommandScript
{
    Human_Base m_Actor;
    vector m_Start;
    vector m_End;
    float m_Time;
    int m_VaultCommand;

    void HumansVaultCommand(DayZInfected pInfected)
    {
        m_Actor = Human_Base.Cast(pInfected);
        m_Start = m_Actor.GetPosition();
        m_End = m_Actor.m_HumanVaultLanding;
        m_VaultCommand = m_Actor.GetAnimInterface().BindCommand("CMD_VAULT");
    }

    override void OnActivate()
    {
        PreAnim_CallCommand(m_VaultCommand, m_Actor.m_HumanVaultType, 0);
    }

    override void PrePhysUpdate(float pDt)
    {
        PrePhys_SetTranslation(vector.Zero);
    }

    override bool PostPhysUpdate(float pDt)
    {
        m_Time += pDt;
        float travelTime = 1.05;
        float duration = 1.85;
        if (m_Actor.m_HumanVaultType != 1) { travelTime = 1.7; duration = 2.4; }
        float t = Math.Clamp((m_Time - 0.2) / travelTime, 0, 1);
        vector position = Human_Base.HumanVaultPosition(m_Start, m_End, t);
        if (m_Actor.m_HumanVaultType != 1) position = HumansVehicleClimb.Position(m_Start, m_End, t, m_Actor.m_HumanVaultType == 0);
        PostPhys_SetPosition(position);
        PostPhys_LockRotation();
        // The infected AI agent otherwise keeps its navmesh transform while
        // this command changes the animation body's physics position.
        m_Actor.SetPosition(position);
        return m_Time < duration;
    }
}
