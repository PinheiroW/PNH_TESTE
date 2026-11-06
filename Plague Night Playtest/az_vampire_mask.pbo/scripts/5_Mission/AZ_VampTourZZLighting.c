





modded class MissionServer
{
	static const int LT_FIRST = 500;
	static const int LT_LAST = 516;


	static const int LT_CAMP = 560;
	static const int LT_RIVER = 561;


	static const int NIGHT_FIRST = 574;
	static const int NIGHT_LAST = 594;
	static const string NIGHT_CAMS = "street|field|face|torch|forest|sky";




	static const int FOG_FIRST = 562;
	static const int FOG_LAST = 573;
	static const string FOG_STATES = "ABC";

	float FogRate(int st)
	{
		if (st == 0)
			return 0.0;
		if (st == 1)
			return AZ_Crypt_Moon_Grate.MIST_ON;
		return 0.6;
	}

	string FogCam(int c)
	{
		switch (c)
		{
			case 0: return "stair";
			case 1: return "river";
			case 2: return "hall";
		}
		return "dais";
	}

	string LtLabel(int step)
	{
		if (step >= NIGHT_FIRST && step <= NIGHT_LAST)
		{
			int ni = step - NIGHT_FIRST;
			if (ni == 12)
				return "lt_n223_dusk";
			if (ni == 13)
				return "lt_n225_dusk";
			if (ni == 20)
				return "lt_n0_dusk";
			array<string> ncams = new array<string>;
			NIGHT_CAMS.Split("|", ncams);
			string nset = "223";
			if (ni >= 6)
				nset = "225";
			if (ni >= 14)
			{
				nset = "0";
				ni = ni - 14;
			}
			return "lt_n" + nset + "_" + ncams[ni % 6];
		}
		if (step >= FOG_FIRST && step <= FOG_FIRST + 11)
		{
			int fi = step - FOG_FIRST;
			return "lt_fog" + FOG_STATES.Get(fi / 4) + "_" + FogCam(fi % 4);
		}
		switch (step)
		{
			case 500: return "lt_ante";
			case 501: return "lt_reveal";
			case 502: return "lt_camper";
			case 503: return "lt_mouth";
			case 504: return "lt_nave";
			case 505: return "lt_altar";
			case 506: return "lt_countess";
			case 507: return "lt_sweep_back";
			case 508: return "lt_sweep_mid_back";
			case 509: return "lt_sweep_mid_side";
			case 510: return "lt_gallery_door";
			case 511: return "lt_reveal_v";
			case 512: return "lt_altar_v";
			case 513: return "lt_countess_v";
			case 514: return "lt_ante_v";
			case 560: return "lt_camp13";
			case 561: return "lt_river22";
			case 516: return "lt_hold";
			case 515: return "lt_vorkuta";
		}
		return "";
	}

	bool LtMine(int step)
	{
		return (step >= LT_FIRST && step <= LT_LAST) || (step >= FOG_FIRST && step <= FOG_LAST) || step == LT_CAMP || step == LT_RIVER || (step >= NIGHT_FIRST && step <= NIGHT_LAST);
	}

	override string HwStepLabel(int step)
	{
		if (LtMine(step))
			return LtLabel(step);
		return super.HwStepLabel(step);
	}

	override int HwNextStep(int step)
	{
		if (step == 340)
			return LT_FIRST;
		if (step >= LT_FIRST && step < LT_LAST)
			return step + 1;
		if (step == LT_LAST)
			return FOG_FIRST;
		if (step >= FOG_FIRST && step < FOG_LAST)
			return step + 1;
		if (step == FOG_LAST)
			return LT_CAMP;
		if (step == LT_CAMP)
			return LT_RIVER;
		if (step == LT_RIVER)
			return NIGHT_FIRST;
		if (step >= NIGHT_FIRST && step < NIGHT_LAST)
			return step + 1;
		if (step == NIGHT_LAST)
			return super.HwNextStep(340);
		return super.HwNextStep(step);
	}

	override void HwStep(PlayerBase p, int step)
	{
		if (!LtMine(step))
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
		if (step >= NIGHT_FIRST && step <= NIGHT_LAST)
		{
			NightStep(p, step);
			return;
		}
		LtStep(p, step);
	}

	protected AZ_VampTorch m_LtTorch;
	protected float m_LtMist = -1;


	void NightStep(PlayerBase p, int step)
	{
		int ni = step - NIGHT_FIRST;
		int lset = 223;
		if (ni >= 6 && ni != 12)
			lset = AZ_VampNightLighting.ID;
		if (ni >= 14)
			lset = 0;
		bool dusk = ni == 12 || ni == 13 || ni == 20;
		if (dusk)
			GetGame().GetWorld().SetDate(2011, 11, 10, 16, 45);
		else
			GetGame().GetWorld().SetDate(2011, 11, 10, 23, 30);
		AZ_VampProgress.s_BloodMoon = false;
		if (p.m_AZVampire)
			AZ_VampEvent.Cure(p);
		EntityAI held = p.GetHumanInventory().GetEntityInHands();
		if (held)
			held.Delete();
		Weather w = GetGame().GetWeather();
		w.GetOvercast().SetLimits(0, 1);
		w.GetOvercast().Set(0.0, 0, 900);
		if (m_LtMist < 0)
			m_LtMist = AZ_VampState.Cfg().nightMist;
		AZ_VampState.Cfg().nightMist = 0;
		w.GetFog().SetLimits(0, 1);
		w.SetDynVolFogDistanceDensity(0, 0);
		w.GetFog().Set(0.0, 0, 900);
		if (p.GetIdentity())
			GetGame().RPCSingleParam(p, AZ_VampLightTune.RPC, new Param1<string>("lightcfg:" + lset), true, p.GetIdentity());
		if (m_LtTorch)
		{
			GetGame().ObjectDelete(m_LtTorch);
			m_LtTorch = null;
		}
		vector stand, cam, look;
		int cami = ni % 6;
		if (ni >= 14)
			cami = (ni - 14) % 6;
		if (dusk)
			cami = 1;
		switch (cami)
		{
			case 0:
				stand = Vector(6906.131, 0, 11164.6);
				look = Vector(6906.131, 1.0, 11185.0);
				break;
			case 1:
				stand = Vector(8805, 0, 9851);
				look = Vector(8905, 0, 9831);
				break;
			case 2:
			case 3:
				stand = Vector(8805, 0, 9851);
				look = Vector(8905, 0, 9849);
				break;
			case 4:
				stand = Vector(7598, 0, 8866);
				look = Vector(7624, 0, 8840);
				break;
			case 5:
				stand = Vector(8805, 0, 9851);
				look = Vector(8905, 0, 9849);
				break;
		}
		stand[1] = GetGame().SurfaceY(stand[0], stand[2]);
		look[1] = look[1] + GetGame().SurfaceY(look[0], look[2]) + 1.0;
		p.SetPosition(stand);
		vector face = look - stand;
		face[1] = 0;
		p.SetDirection(face.Normalized());
		cam = stand + face.Normalized() * 0.5 + Vector(0, 1.7, 0);
		if (cami == 2 || cami == 3)
		{
			cam = stand + face.Normalized() * 1.3 + Vector(0, 1.65, 0);
			look = stand + Vector(0, 1.6, 0);
		}
		if (cami == 3)
			m_LtTorch = AZ_VampTorch.Plant(stand + face.Normalized() * 0.6 + Vector(0.7, 0, 0), 0);
		if (cami == 5)
			look = cam + face.Normalized() * 7 + Vector(0, 7, 0);
		int settle = 6000;
		if (cami == 0 || ni == 12 || ni == 20)
			settle = 12000;
		Print("[AZ_Vamp] TEST night " + HwStepLabel(step) + " set " + lset + " at " + stand);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, settle, false, p, HwStepLabel(step), cam, look);
		int next = settle + 6000;
		if (step == NIGHT_LAST)
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(NightEnd, next - 500, false, p);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, next, false, p, HwNextStep(step));
	}

	void NightEnd(PlayerBase p)
	{
		if (m_LtTorch)
		{
			GetGame().ObjectDelete(m_LtTorch);
			m_LtTorch = null;
		}
		if (p && p.GetIdentity())
			GetGame().RPCSingleParam(p, AZ_VampLightTune.RPC, new Param1<string>("lightcfg:223"), true, p.GetIdentity());
		if (m_LtMist >= 0)
		{
			AZ_VampState.Cfg().nightMist = m_LtMist;
			m_LtMist = -1;
		}
	}

	void LtStep(PlayerBase p, int step)
	{

		GetGame().GetWorld().SetDate(2011, 11, 5, 23, 0);
		AZ_VampProgress.s_BloodMoon = false;
		bool vamp = step >= 511 && step <= 514;
		int fogI = step - FOG_FIRST;
		bool fog = fogI >= 0 && fogI < 12;
		if (vamp && !p.m_AZVampire)
			AZ_VampEvent.MakeVampire(p);
		else if (!vamp && p.m_AZVampire)
			AZ_VampEvent.Cure(p);
		float ap = AZ_VampCrypt.APSE_F;
		vector stand, cam, look;
		switch (step)
		{
			case 500:
			case 514:
				stand = AZ_VampCrypt.Hall(-19.0, -3.0, 0.1);
				cam = AZ_VampCrypt.Hall(-19.2, -4.0, 1.65);
				look = AZ_VampCrypt.Hall(-6.0, 2.0, 1.0);
				break;
			case 501:
			case 511:

				stand = AZ_VampCrypt.Nave(2.2, 0, 0.05);
				cam = AZ_VampCrypt.Nave(2.2, 0, 1.65);
				look = AZ_VampCrypt.Nave(AZ_VampCrypt.CountessF(), 0, 4.0);
				break;
			case 502:
				stand = AZ_VampCrypt.Nave(2.0, 0, 0.05);
				cam = AZ_VampCrypt.Nave(0.2, 0.3, 1.65);
				look = AZ_VampCrypt.Nave(2.0, 0, 1.1);
				break;
			case 503:
				stand = AZ_VampCrypt.Nave(2.0, 0, 0.05);
				cam = AZ_VampCrypt.Nave(5.5, 0.8, 1.65);
				look = AZ_VampCrypt.Nave(0, 0, 0.9);
				break;
			case 504:
				AZ_VampCrypt.ShotSpot(2, stand, cam, look);
				break;
			case 505:
			case 512:
				AZ_VampCrypt.ShotSpot(3, stand, cam, look);
				break;
			case 506:
			case 513:
				stand = AZ_VampCrypt.Nave(ap + 2.5, -1.8, AZ_VampCrypt.DAIS_Y + 0.1);
				cam = AZ_VampCrypt.Nave(ap + 2.5, -2.2, AZ_VampCrypt.DAIS_Y + 1.6);
				look = AZ_VampCrypt.Nave(AZ_VampCrypt.CountessF(), 0, 4.0);
				break;
			case 507:
				stand = AZ_VampCrypt.Hall(0.15, 0.8, 0.1);
				cam = AZ_VampCrypt.Hall(0.4, 0, 1.6);
				look = AZ_VampCrypt.Hall(-10.0, 0, 1.2);
				break;
			case 508:
				stand = AZ_VampCrypt.Nave(26.0, -5.0, 0.1);
				cam = AZ_VampCrypt.Nave(26.0, -4.6, 1.6);
				look = AZ_VampCrypt.Nave(0, 0, 3.0);
				break;
			case 509:
				stand = AZ_VampCrypt.Nave(26.0, -5.0, 0.1);
				cam = AZ_VampCrypt.Nave(26.0, -4.6, 1.6);
				look = AZ_VampCrypt.Nave(30.0, 8.0, 4.0);
				break;
			case 515:
				vector lan = Vector(6906.131, 0, 11171.649);
				lan[1] = GetGame().SurfaceY(lan[0], lan[2]);
				stand = lan + Vector(0, 0, -7.0);
				stand[1] = GetGame().SurfaceY(stand[0], stand[2]);
				cam = stand + Vector(0, 1.7, 0);
				look = lan + Vector(0, 1.0, 0);
				break;
			case 516:
				stand = AZ_VampCrypt.Nave(ap + 2.5, -1.8, AZ_VampCrypt.DAIS_Y + 0.1);
				cam = AZ_VampCrypt.Nave(ap + 2.5, -2.2, AZ_VampCrypt.DAIS_Y + 1.6);
				look = AZ_VampCrypt.Nave(AZ_VampCrypt.CountessF(), 0, 4.6);
				break;
			case 562:
			case 566:
			case 570:
				AZ_VampCrypt.ShotSpot(1, stand, cam, look);
				break;
			case 563:
			case 567:
			case 571:
				AZ_VampCrypt.ShotSpot(2, stand, cam, look);
				break;
			case 564:
			case 568:
			case 572:
				AZ_VampCrypt.ShotSpot(3, stand, cam, look);
				break;
			case 565:
			case 569:
			case 573:
				stand = AZ_VampCrypt.Nave(ap + 2.5, -1.8, AZ_VampCrypt.DAIS_Y + 0.1);
				cam = AZ_VampCrypt.Nave(ap + 2.5, -2.2, AZ_VampCrypt.DAIS_Y + 1.6);
				look = AZ_VampCrypt.Nave(AZ_VampCrypt.ALTAR_F + 1.0, 0.5, 0.9);
				break;
			case 560:

				stand = AZ_VampCrypt.Nave(13.0, -3.0, 0.1);
				cam = AZ_VampCrypt.Nave(2.2, -1.8, 1.7);
				look = AZ_VampCrypt.Nave(13.0, -3.0, 1.1);
				break;
			case 561:
				stand = AZ_VampCrypt.Nave(18.5, -2.8, 0.1);
				cam = AZ_VampCrypt.Nave(18.5, -3.2, 1.6);
				look = AZ_VampCrypt.Nave(24.0, 0, -AZ_VampCrypt.NAVE_DEPTH + AZ_VampCrypt.BLOOD_MIN);
				break;
			case 510:
				stand = AZ_VampCrypt.Nave(32.0, -4.8, 0.1);
				cam = AZ_VampCrypt.Nave(32.0, -4.4, 1.6);
				look = AZ_VampCrypt.Nave(36.0, -12.0, 2.0);
				break;
		}
		p.SetPosition(stand);
		vector face = look - stand;
		face[1] = 0;
		p.SetDirection(face.Normalized());
		string label = LtLabel(step);
		int settle = 6000;
		if (step == 500 || step == 511 || step == 514)
			settle = 10000;
		if (step == 515)
			settle = 25000;
		if (fog && fogI % 4 == 0)
		{
			settle = 15000;
			if (p.GetIdentity())
				GetGame().RPCSingleParam(p, AZ_VampLightTune.RPC, new Param1<string>("mist:" + FogRate(fogI / 4)), true, p.GetIdentity());
		}
		if (fog && fogI == 11)
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(LtMistBack, settle + 5500, false, p);
		Print("[AZ_Vamp] TEST lighting " + label + " at " + stand + " inside " + AZ_VampCrypt.Inside(stand) + " vampire " + p.m_AZVampire);
		if (step == 516)
		{


			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 10000, false, p, "lt_hold_idle", cam, look);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(LtEmbers, 27000, false, p);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 27500, false, p, "lt_hold_stare", cam, look);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 46000, false, p, HwNextStep(step));
			return;
		}
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, settle, false, p, label, cam, look);
		int next = settle + 6000;
		if (step == 515)
			next = settle + 35000;
		if (step == LT_RIVER)
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(LtCure, next - 500, false, p);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, next, false, p, HwNextStep(step));
	}

	void LtEmbers(PlayerBase p)
	{
		if (p && p.GetIdentity())
			GetGame().RPCSingleParam(p, PlayerBase.RPC_EMBERS, new Param1<int>(16000), true, p.GetIdentity());
	}

	void LtMistBack(PlayerBase p)
	{
		if (p && p.GetIdentity())
			GetGame().RPCSingleParam(p, AZ_VampLightTune.RPC, new Param1<string>("mist:" + AZ_VampState.Cfg().cryptMist), true, p.GetIdentity());
	}

	void LtCure(PlayerBase p)
	{
		if (p && p.m_AZVampire)
			AZ_VampEvent.Cure(p);
	}
}
