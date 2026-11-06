





class AZ_VampEyePair
{
	AZ_VampEyeLight l;
	AZ_VampEyeLight r;
	int seen;
}

modded class MissionGameplay
{
	ref map<PlayerBase, ref AZ_VampEyePair> m_AZVampEyes = new map<PlayerBase, ref AZ_VampEyePair>;
	int m_AZVampEyeFrame;
	ref array<Man> m_AZVampEyeMen = new array<Man>;
	static const float EYE_FORWARD = 0.015;

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		if (!GetGame().IsClient())
			return;
		m_AZVampEyeFrame++;
		PlayerBase me = PlayerBase.Cast(GetGame().GetPlayer());
		m_AZVampEyeMen.Clear();
		GetGame().GetPlayers(m_AZVampEyeMen);
		foreach (Man m : m_AZVampEyeMen)
		{
			PlayerBase p = PlayerBase.Cast(m);
			if (!p || !AZ_VampEyesWanted(p, me))
				continue;
			AZ_VampEyePair pair = m_AZVampEyes.Get(p);
			if (!pair)
			{
				pair = new AZ_VampEyePair();
				pair.l = AZ_VampEyeLight.Cast(AZ_VampLightGate.Light(AZ_VampEyeLight, p.GetPosition(), 0.5));
				pair.r = AZ_VampEyeLight.Cast(AZ_VampLightGate.Light(AZ_VampEyeLight, p.GetPosition(), 0.5));
				m_AZVampEyes.Set(p, pair);
			}
			pair.seen = m_AZVampEyeFrame;
			AZ_VampEyesPlace(p, pair);
		}

		for (int i = m_AZVampEyes.Count() - 1; i >= 0; i--)
		{
			AZ_VampEyePair old = m_AZVampEyes.GetElement(i);
			if (old.seen == m_AZVampEyeFrame)
				continue;
			if (old.l)
				old.l.Destroy();
			if (old.r)
				old.r.Destroy();
			m_AZVampEyes.RemoveElement(i);
		}
	}

	bool AZ_VampEyesWanted(PlayerBase p, PlayerBase me)
	{
		if (!p.IsAlive() || !p.m_AZVampire)
			return false;
		if (p == me && AZ_VampFP.OwnEyeView(p))
			return false;






		if (AZ_VampCrypt.Inside(p.GetPosition()))
			return false;
		EntityAI item = p.FindAttachmentBySlotName("Mask");
		return item && item.IsKindOf("AZ_VampireMask_Bared");
	}

	void AZ_VampEyesPlace(PlayerBase p, AZ_VampEyePair pair)
	{
		int il = p.GetBoneIndexByName("EyeLeft");
		int ir = p.GetBoneIndexByName("EyeRight");
		if (il < 0 || ir < 0)
			return;
		vector a = p.GetBonePositionWS(il);
		vector b = p.GetBonePositionWS(ir);
		vector across = b - a;
		vector fwd = Vector(-across[2], 0, across[0]);
		if (vector.Dot(fwd, p.GetDirection()) < 0)
			fwd = -fwd;
		fwd = fwd.Normalized() * EYE_FORWARD;
		if (pair.l)
			pair.l.SetPosition(a + fwd);
		if (pair.r)
			pair.r.SetPosition(b + fwd);
	}
}
