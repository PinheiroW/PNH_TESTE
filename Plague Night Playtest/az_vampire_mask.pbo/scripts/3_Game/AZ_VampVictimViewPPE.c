// AZ-2569: the PPE half of the victim dim view. It lives in 3_Game because DayZDiag refuses
// "modded class PPERequester_UnconEffects" from 5_Mission (Unknown type). 5_Mission/AZ_VampVictimView.c drives it.
class AZ_VampUnconCap
{
	static float s_Vignette = 2.0;	// vanilla uncon vignette is ~2.0 (black); 2.0 means no cap
}

class AZ_PPEVampDim extends PPERequester_GameplayBase
{
	static const int L_DIM = 150;

	void SetLevel(float blur, float saturation)
	{
		SetTargetValueFloat(PostProcessEffectType.GaussFilter, PPEGaussFilter.PARAM_INTENSITY, false, blur, L_DIM, PPOperators.HIGHEST);
		SetTargetValueFloat(PostProcessEffectType.Glow, PPEGlow.PARAM_SATURATION, false, saturation, L_DIM, PPOperators.LOWEST);
		if (!m_IsRunning)
			Start();
		else
			SetRequesterUpdating(true);
	}

	static AZ_PPEVampDim Get()
	{
		AZ_PPEVampDim r = AZ_PPEVampDim.Cast(PPERequesterBank.GetRequester(AZ_PPEVampDim));
		if (!r)
		{
			PPERequesterBank.RegisterRequester(AZ_PPEVampDim);
			r = AZ_PPEVampDim.Cast(PPERequesterBank.GetRequester(AZ_PPEVampDim));
		}
		return r;
	}
}

// Vanilla re-Starts this every frame while unconscious with param1 = vignette (2.0 = black). Cap it while dimmed.
// The param object is PlayerBase.m_UnconParam, rewritten by vanilla before each Start, so editing it in place is safe.
modded class PPERequester_UnconEffects
{
	override protected void OnStart(Param par = null)
	{
		if (AZ_VampUnconCap.s_Vignette < 2.0 && par)
		{
			Param1<float> p = Param1<float>.Cast(par);
			if (p && p.param1 > AZ_VampUnconCap.s_Vignette)
				p.param1 = AZ_VampUnconCap.s_Vignette;
		}
		super.OnStart(par);
	}
}
