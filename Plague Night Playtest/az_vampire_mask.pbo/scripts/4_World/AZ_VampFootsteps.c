

modded class PlayerBase
{
    protected float m_AZNightFootstepVolume = 0.45;
    protected bool m_AZQuietStep;

    override void Init()
    {
        super.Init();
        RegisterNetSyncVariableFloat("m_AZNightFootstepVolume", 0, 1, 2);
    }

    override void OnScheduledTick(float deltaTime)
    {
        super.OnScheduledTick(deltaTime);
        if (!GetGame().IsServer())
            return;
        float volume = Math.Clamp(AZ_VampState.Cfg().nightFootstepVolume, 0, 1);
        if (volume != m_AZNightFootstepVolume)
        {
            m_AZNightFootstepVolume = volume;
            SetSynchDirty();
        }
    }

    override void OnStepEvent(string pEventType, string pUserString, int pUserInt)
    {

        m_AZQuietStep = !GetGame().IsDedicatedServer() && GetGame().GetPlayer() && GetGame().GetPlayer() != this && m_AZVampire && GetGame().GetWorld().IsNight();
        super.OnStepEvent(pEventType, pUserString, pUserInt);
        m_AZQuietStep = false;
    }

    override AbstractWave PlaySound(SoundObject so, SoundObjectBuilder sob)
    {
        AbstractWave wave = super.PlaySound(so, sob);
        if (wave && m_AZQuietStep)
            wave.SetVolumeRelative(m_AZNightFootstepVolume);
        return wave;
    }
}
