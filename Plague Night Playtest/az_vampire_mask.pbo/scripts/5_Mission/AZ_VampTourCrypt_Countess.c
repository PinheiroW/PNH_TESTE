









modded class MissionServer
{
	static const string CTS_LABELS[5] = {"countess_cu_chalice", "countess_cu_hand", "countess_cu_face", "countess_cu_slit", "countess_cu_hem"};

	static const float CTS_POSE[30] = {
		-1.75, -1.35, 1.95,   -0.93, -0.30, 1.68,
		1.30, -1.55, 2.15,    0.60, -0.43, 2.03,
		0.15, -1.30, 2.70,    0.07, -0.28, 2.64,
		0.35, -1.70, 1.30,    0.15, -0.40, 1.40,
		0.20, -2.10, 0.60,    0.00, -0.75, 0.25};



	static vector CtsAt(float x, float y, float z)
	{
		float s = AZ_VampCrypt.CountessScale();
		return AZ_VampCrypt.Nave(AZ_VampCrypt.CountessF() + y * s, x * s, AZ_VampCrypt.CountessFeet() + z * s);
	}

	override string HwStepLabel(int step)
	{
		if (step == 330)
			return "countess_kneel";
		if (step >= 331 && step <= 335)
			return CTS_LABELS[step - 331];
		if (step == 336)
			return "kneel_back";
		if (step == 337)
			return "drum_eyes";
		if (step == 338)
			return "door_from_church";
		return super.HwStepLabel(step);
	}

	override int HwNextStep(int step)
	{
		if (step == 316 && s_HwOnly)
			return 330;
		if (step >= 330 && step < 338)
			return step + 1;
		if (step == 338)
			return super.HwNextStep(316);
		return super.HwNextStep(step);
	}

	override void HwStep(PlayerBase p, int step)
	{
		if (step >= 331 && step <= 335)
		{
			CtsStep(p, step);
			return;
		}
		if (step == 336)
		{
			KneelBackStep(p, step);
			return;
		}
		if (step == 337)
		{
			DrumEyesStep(p, step);
			return;
		}
		if (step == 338)
		{
			DoorFromChurchStep(p, step);
			return;
		}
		if (step != 330)
		{
			super.HwStep(p, step);
			return;
		}
		if (!p)
			return;
		if (!HwWanted(step))
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 100, false, p, HwNextStep(step));
			return;
		}

		float kf = AZ_VampCrypt.ALTAR_F - 1.56;


		vector stand = AZ_VampCrypt.Nave(kf - 1.0, 1.56, AZ_VampCrypt.DAIS_Y + 0.05);
		vector cam = AZ_VampCrypt.Nave(kf, 1.5, AZ_VampCrypt.DAIS_Y + 0.95);
		vector look = CtsAt(0.07, -0.28, 2.64);
		p.SetPosition(stand);
		vector face = look - stand;
		face[1] = 0;
		p.SetDirection(face.Normalized());
		Print("[AZ_Vamp] TEST crypt countess_kneel at " + stand + " inside " + AZ_VampCrypt.Inside(stand));

		vector altar = AZ_VampCrypt.Nave(AZ_VampCrypt.ALTAR_F, 0, AZ_VampCrypt.DAIS_Y);
		AZ_VampCrypt.StatueEyes(altar, true);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 4000, false, p, "countess_kneel", cam, look);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AZ_VampCrypt.StatueEyes, 9500, false, altar, false);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 10000, false, p, HwNextStep(step));
	}

	void CtsStep(PlayerBase p, int step)
	{
		if (!p)
			return;
		if (!HwWanted(step))
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 100, false, p, HwNextStep(step));
			return;
		}
		int k = (step - 331) * 6;

		vector cam = CtsAt(CTS_POSE[k], CTS_POSE[k + 1], CTS_POSE[k + 2]);
		vector look = CtsAt(CTS_POSE[k + 3], CTS_POSE[k + 4], CTS_POSE[k + 5]);
		float kf = AZ_VampCrypt.ALTAR_F - 1.56;
		vector stand = AZ_VampCrypt.Nave(kf - 1.0, 1.56, AZ_VampCrypt.DAIS_Y + 0.05);
		p.SetPosition(stand);
		Print("[AZ_Vamp] TEST crypt " + CTS_LABELS[step - 331] + " cam " + cam + " look " + look);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 4000, false, p, CTS_LABELS[step - 331], cam, look);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 10000, false, p, HwNextStep(step));
	}



	void KneelBackStep(PlayerBase p, int step)
	{
		if (!p)
			return;
		AZ_VampRitualSite ks = AZ_VampRitual.CryptSite();
		if (!HwWanted(step) || !ks || ks.m_Kneel.Count() == 0)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 100, false, p, HwNextStep(step));
			return;
		}
		vector spot = ks.m_Kneel[0];
		vector cam = spot + Vector(0, 1.3, 0);
		vector look = AZ_VampCrypt.Nave(AZ_VampCrypt.CountessF() - 1.0, 0, AZ_VampCrypt.DAIS_Y + 0.1);
		vector away = spot - ks.m_Center;
		away[1] = 0;
		away.Normalize();
		p.SetPosition(spot + away * 1.2);
		Print("[AZ_Vamp] TEST crypt kneel_back cam " + cam + " look " + look);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 4000, false, p, "kneel_back", cam, look);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 8000, false, p, HwNextStep(step));
	}



	void DrumEyesStep(PlayerBase p, int step)
	{
		if (!p)
			return;
		AZ_VampRitualSite ks = AZ_VampRitual.CryptSite();
		AZ_VampAltar_Drum drum = null;
		if (ks)
			drum = AZ_VampAltar_Drum.Cast(ks.m_Slab);
		if (!HwWanted(step) || !drum)
		{
			if (HwWanted(step))
				Print("[AZ_Vamp] TEST crypt drum_eyes SKIP: no drum altar (layout box)");
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 100, false, p, HwNextStep(step));
			return;
		}
		float df = AZ_VampCrypt.ALTAR_F;
		vector cam = AZ_VampCrypt.Nave(df - 2.4, 0.4, AZ_VampCrypt.DAIS_Y + 0.8);
		vector look = AZ_VampCrypt.Nave(df - 1.3, 0, AZ_VampCrypt.DAIS_Y + 0.45);
		p.SetPosition(AZ_VampCrypt.Nave(df - 3.6, 1.2, AZ_VampCrypt.DAIS_Y + 0.05));
		drum.AZ_SetEyeLevel(3);
		Print("[AZ_Vamp] TEST crypt drum_eyes cam " + cam + " look " + look + " level " + drum.AZ_EyeLevel());
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 4000, false, p, "drum_eyes", cam, look);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(drum.AZ_SetEyeLevel, 9000, false, 0);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 9500, false, p, HwNextStep(step));
	}



	void DoorFromChurchStep(PlayerBase p, int step)
	{
		if (!p)
			return;
		if (!HwWanted(step))
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 100, false, p, HwNextStep(step));
			return;
		}
		vector mouth = AZ_VampCrypt.Mouth();
		vector toChurch = Vector(AZ_VampDoorSurvey.CHURCH_X, 0, AZ_VampDoorSurvey.CHURCH_Z) - mouth;
		toChurch[1] = 0;
		toChurch.Normalize();
		vector cam = mouth + toChurch * 12.0;
		float cy = GetGame().SurfaceY(cam[0], cam[2]) + 1.7;
		cam[1] = cy;
		vector look = AZ_VampCrypt.DoorOrigin() + Vector(0, 1.0, 0);
		p.SetPosition(cam - toChurch * -1.5 + Vector(0, -1.65, 0));
		GetGame().GetWorld().SetDate(2011, 11, 5, 22, 0);
		Print("[AZ_Vamp] TEST door_from_church cam " + cam + " look " + look + " mouth " + mouth);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 6000, false, p, "door_from_church", cam, look);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 10000, false, p, HwNextStep(step));
	}
}
