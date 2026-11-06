



class AZ_PPEVampHunger extends PPERequester_GameplayBase
{
	static const int L_VIGNETTE = 400;

	void SetHunger(float h)
	{
		SetTargetValueFloat(PostProcessEffectType.Glow, PPEGlow.PARAM_VIGNETTE, false, 0.45 * h, L_VIGNETTE, PPOperators.HIGHEST);
		SetTargetValueColor(PostProcessEffectType.Glow, PPEGlow.PARAM_VIGNETTECOLOR, {0.35 * h, 0.0, 0.0, 0.0}, L_VIGNETTE, PPOperators.ADD);
	}

	static AZ_PPEVampHunger Get()
	{
		AZ_PPEVampHunger r = AZ_PPEVampHunger.Cast(PPERequesterBank.GetRequester(AZ_PPEVampHunger));
		if (!r)
		{
			PPERequesterBank.RegisterRequester(AZ_PPEVampHunger);
			r = AZ_PPEVampHunger.Cast(PPERequesterBank.GetRequester(AZ_PPEVampHunger));
		}
		return r;
	}
}

modded class BloodLoss
{
	bool m_AZVampVision;
	float m_AZVampHungerSet = -1;

	override void OnUpdateClient(PlayerBase player, float deltatime)
	{
		if (!player || !player.m_AZVampire || !player.IsPlayerSelected() || !player.GetTransferValues())
		{
			if (m_AZVampVision)
			{
				m_AZVampVision = false;
				m_AZVampHungerSet = -1;
				m_BloodSet = -1;
				AZ_PPEVampHunger r = AZ_PPEVampHunger.Get();
				if (r)
					r.Stop();
			}
			super.OnUpdateClient(player, deltatime);
			return;
		}

		#ifndef NO_GUI
		if (!m_AZVampVision)
		{
			m_AZVampVision = true;
			if (m_RequesterBloodLoss)
				m_RequesterBloodLoss.Stop();
		}

		float b = player.GetTransferValues().GetBlood();
		float h = Math.Clamp((0.25 - b) / 0.225, 0, 1);
		if (player.AZ_SightUnderground())
			h = 0;
		if (Math.AbsFloat(h - m_AZVampHungerSet) > 0.02)
		{
			m_AZVampHungerSet = h;
			AZ_PPEVampHunger req = AZ_PPEVampHunger.Get();
			if (req)
			{
				if (h > 0)
					req.SetHunger(h);
				else
					req.Stop();
			}
		}
		#endif
	}
}



class AZ_PPEVampSight extends PPERequester_GameplayBase
{
	static const int L_EXPOSURE = 600;
	static const float EXPOSURE = 2.5;

	void SetSight(float k)
	{
		SetTargetValueFloat(PPEExceptions.EXPOSURE, PPEExposureNative.PARAM_INTENSITY, false, EXPOSURE * k, L_EXPOSURE, PPOperators.ADD);
	}

	static AZ_PPEVampSight Get()
	{
		AZ_PPEVampSight r = AZ_PPEVampSight.Cast(PPERequesterBank.GetRequester(AZ_PPEVampSight));
		if (!r)
		{
			PPERequesterBank.RegisterRequester(AZ_PPEVampSight);
			r = AZ_PPEVampSight.Cast(PPERequesterBank.GetRequester(AZ_PPEVampSight));
		}
		return r;
	}
}

modded class PlayerBase
{
	float m_AZVampSight;
	float m_AZVampSightSet = -1;


	bool AZ_SightUnderground()
	{
		return AZ_VampCryptLook.s_In || m_UndergroundPresence > EUndergroundPresence.NONE || AZ_VampCrypt.Inside(GetPosition());
	}

	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);
		if (!GetGame().IsClient() || !IsControlledPlayer())
			return;
		float want = 0;
		if (m_AZVampire && IsAlive() && GetGame().GetWorld().IsNight())
			want = 1;

		if (want > 0 && AZ_SightUnderground())
		{
			want = 0;
			m_AZVampSight = 0;
		}
		if (want > m_AZVampSight)
			m_AZVampSight = Math.Min(want, m_AZVampSight + pDt * 0.25);
		else
			m_AZVampSight = Math.Max(want, m_AZVampSight - pDt * 0.25);
		if (Math.AbsFloat(m_AZVampSight - m_AZVampSightSet) < 0.02 && !(m_AZVampSight == 0 && m_AZVampSightSet > 0))
			return;
		m_AZVampSightSet = m_AZVampSight;
		AZ_PPEVampSight r = AZ_PPEVampSight.Get();
		if (!r)
			return;
		if (m_AZVampSight <= 0)
			r.Stop();
		else
			r.SetSight(m_AZVampSight);
	}
}
