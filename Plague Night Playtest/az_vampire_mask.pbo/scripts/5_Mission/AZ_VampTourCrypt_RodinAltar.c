






modded class MissionServer
{
	static const string RA_LABELS[6] = {"altar_cur_wide", "altar_cur_close", "altar_cur_head", "altar_rodin_wide", "altar_rodin_close", "altar_rodin_head"};


	static const float RA_POSE[18] = {
		-3.2, -1.6, 1.7,    0.0, 0.1, 0.6,
		-0.9, -0.5, 1.45,   0.0, 0.3, 0.9,
		-1.3, 2.7, 1.2,     0.0, 1.1, 0.75};
	protected Object m_RaCarved;
	protected Object m_RaRodin;

	override string HwStepLabel(int step)
	{
		if (step >= 345 && step <= 350)
			return RA_LABELS[step - 345];
		return super.HwStepLabel(step);
	}

	override int HwNextStep(int step)
	{
		if (step == 316 && s_HwOnly)
			return 345;
		if (step >= 345 && step < 350)
			return step + 1;
		if (step == 350)
			return super.HwNextStep(316);
		return super.HwNextStep(step);
	}

	override void HwStep(PlayerBase p, int step)
	{
		if (step < 345 || step > 350)
		{
			super.HwStep(p, step);
			return;
		}
		if (!p)
			return;
		RaSwap(step >= 348);
		if (!HwWanted(step))
		{
			if (step == 350)
				RaSwap(false);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 100, false, p, HwNextStep(step));
			return;
		}
		int k = ((step - 345) % 3) * 6;

		float f = AZ_VampCrypt.ALTAR_F;
		float y = AZ_VampCrypt.DAIS_Y;
		vector cam = AZ_VampCrypt.Nave(f + RA_POSE[k], RA_POSE[k + 1], y + RA_POSE[k + 2]);
		vector look = AZ_VampCrypt.Nave(f + RA_POSE[k + 3], RA_POSE[k + 4], y + RA_POSE[k + 5]);
		vector stand = AZ_VampCrypt.Nave(f - 4.5, -2.5, y + 0.05);
		p.SetPosition(stand);
		Print("[AZ_Vamp] TEST crypt " + RA_LABELS[step - 345] + " cam " + cam + " look " + look + " rodin " + (m_RaRodin != null));
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 4000, false, p, RA_LABELS[step - 345], cam, look);
		if (step == 350)
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(RaSwap, 9500, false, false);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 10000, false, p, HwNextStep(step));
	}




	void RaClear()
	{
		if (!m_RaRodin)
			return;
		vector mm[2];
		m_RaRodin.ClippingInfo(mm);
		vector lo = mm[0];
		vector hi = mm[1];
		Print("[AZ_Vamp] TEST rodin altar box: " + (hi[0] - lo[0]) + " x " + (hi[2] - lo[2]) + " m, top " + hi[1] + " (local min " + lo + " max " + hi + ")");
		AZ_VampRitualSite s = AZ_VampRitual.CryptSite();
		if (!s)
		{
			Print("[AZ_Vamp] TEST rodin altar clearance: no ritual site");
			return;
		}
		for (int k = 0; k < 4; k++)
		{
			vector kp = AZ_VampRitual.Ring(s, AZ_VampRitual.KNEEL_R, 45 + k * 90);
			Print("[AZ_Vamp] TEST rodin altar clearance: kneel point " + k + " " + RaGap(kp, lo, hi) + " m");
		}
		Print("[AZ_Vamp] TEST rodin altar clearance: the Elder's spot past the head " + RaGap(s.m_Head, lo, hi) + " m");
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition3D(m_RaRodin.GetPosition(), 6.0, objs, null);
		foreach (Object o : objs)
		{
			if (o && o.GetType() == "AZ_VampAltarBasin")
				Print("[AZ_Vamp] TEST rodin altar clearance: the basin stair threshold (AZ_VampAltarBasin) " + RaGap(o.GetPosition(), lo, hi) + " m");
		}
	}



	void AltarClearance()
	{
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition3D(AZ_VampCrypt.Nave(AZ_VampCrypt.ALTAR_F, 0, AZ_VampCrypt.DAIS_Y), 2.0, objs, null);
		foreach (Object o : objs)
		{
			if (o && o.IsKindOf("AZ_VampAltar_Blood"))
			{
				Print("[AZ_Vamp] TEST altar built: " + o.GetType());
				m_RaRodin = o;
				RaClear();
				m_RaRodin = null;
				return;
			}
		}
		Print("[AZ_Vamp] TEST altar built: none found at the altar spot");
	}


	override void FeedRun(PlayerBase p)
	{
		AltarClearance();
		super.FeedRun(p);
	}


	float RaGap(vector world, vector lo, vector hi)
	{
		vector l = m_RaRodin.WorldToModel(world);
		float dx = 0;
		if (l[0] < lo[0])
			dx = lo[0] - l[0];
		else if (l[0] > hi[0])
			dx = l[0] - hi[0];
		float dz = 0;
		if (l[2] < lo[2])
			dz = lo[2] - l[2];
		else if (l[2] > hi[2])
			dz = l[2] - hi[2];
		return Math.Sqrt(dx * dx + dz * dz);
	}


	void RaSwap(bool rodin)
	{
		vector at = AZ_VampCrypt.Nave(AZ_VampCrypt.ALTAR_F, 0, AZ_VampCrypt.DAIS_Y);
		if (!m_RaCarved)
		{
			array<Object> objs = new array<Object>;
			GetGame().GetObjectsAtPosition3D(at, 2.0, objs, null);
			foreach (Object o : objs)
			{
				if (o && o.GetType() == "AZ_Crypt_Altar")
					m_RaCarved = o;
			}
		}
		if (rodin && !m_RaRodin)
		{
			if (m_RaCarved)
			{
				vector down = at - Vector(0, 50, 0);
					m_RaCarved.SetPosition(down);
				m_RaCarved.Update();
			}
			m_RaRodin = AZ_VampCrypt.NaveProp("AZ_Crypt_Altar_Rodin", AZ_VampCrypt.ALTAR_F, 0, AZ_VampCrypt.DAIS_Y, 180);
			Print("[AZ_Vamp] TEST rodin altar in: carved " + (m_RaCarved != null) + " rodin " + (m_RaRodin != null));
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(RaClear, 1500, false);
		}
		else if (!rodin && m_RaRodin)
		{
			GetGame().ObjectDelete(m_RaRodin);
			m_RaRodin = null;
			if (m_RaCarved)
			{
				m_RaCarved.SetPosition(at);
				m_RaCarved.Update();
			}
			Print("[AZ_Vamp] TEST rodin altar out, carved back " + (m_RaCarved != null));
		}
	}
}
