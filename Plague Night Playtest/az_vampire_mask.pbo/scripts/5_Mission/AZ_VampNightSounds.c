



modded class MissionGameplay
{
	protected float m_AZScreamIn = 240;
	protected ref EffectSound m_AZScream;

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		m_AZScreamIn -= timeslice;
		if (m_AZScreamIn > 0)
			return;
		m_AZScreamIn = Math.RandomFloat(300, 660);
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p || !p.IsAlive() || !p.m_AZEventOn || p.m_AZVampire || !GetGame().GetWorld().IsNight())
			return;
		float a = Math.RandomFloat(0, Math.PI2);
		vector at = p.GetPosition() + Vector(Math.Sin(a), 0, Math.Cos(a)) * Math.RandomFloat(150, 300);
		at[1] = GetGame().SurfaceY(at[0], at[2]) + 1.5;
		m_AZScream = SEffectManager.PlaySound("AZ_VampNightScream_SoundSet", at);
		Print("[AZ_Vamp] night scream at " + Math.Round(vector.Distance(at, p.GetPosition())) + " m");
	}
}
