




class AZ_VampDarkOrb
{
	static const float MODEL_R = 0.275;


	static Particle Heart(vector centre, float scale)
	{
		Particle p = Particle.PlayInWorld(ParticleList.AZ_VAMP_ORB_HEART, centre);
		if (p)
			p.ScaleParticleParamFromOriginal(EmitorParam.SIZE, scale * MODEL_R / 0.88);
		return p;
	}


	static float Beat()
	{
		float ph = Math.ModFloat(GetGame().GetTime() * 0.001, 1.25) / 1.25;
		float b1 = (ph - 0.06) / 0.07;
		float b2 = (ph - 0.30) / 0.08;
		return Math.Pow(2.718, -b1 * b1) + 0.6 * Math.Pow(2.718, -b2 * b2);
	}

	static void Keep(Particle heart, vector centre, float scale)
	{
		if (!heart)
			return;

		heart.ScaleParticleParamFromOriginal(EmitorParam.SIZE, scale * MODEL_R / 0.88 * (0.85 + 0.6 * Beat()));
		vector toCam = GetGame().GetCurrentCameraPosition() - centre;
		if (toCam.Length() < 0.01)
			return;
		heart.SetPosition(centre + toCam.Normalized() * (scale * MODEL_R + 0.04));
	}


	static EffectSound Hum(Object orb)
	{
		if (!orb || !GetGame().ConfigIsExisting("CfgSoundSets AZ_OrbHum_SoundSet"))
			return null;
		return SEffectManager.PlaySoundOnObject("AZ_OrbHum_SoundSet", orb, 2.0, 2.0, true);
	}

	static void HumStop(EffectSound hum)
	{
		if (!hum)
			return;
		hum.SoundStop();
		SEffectManager.DestroyEffect(hum);
	}


	static vector Under(vector centre, float scale)
	{
		return centre - Vector(0, scale * MODEL_R + 0.05, 0);
	}
}
