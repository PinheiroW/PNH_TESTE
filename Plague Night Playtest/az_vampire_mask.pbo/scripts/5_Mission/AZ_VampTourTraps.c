







modded class MissionServer
{
	static int s_AZTrapWait;

	override void OnInit()
	{
		super.OnInit();
		s_AZTrapWait = 0;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZTrapBuild, 10000, false);
	}


	void AZTrapBuild()
	{
		if (!AZ_VampCrypt.s_Built)
		{
			s_AZTrapWait++;
			if (s_AZTrapWait < 60)
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZTrapBuild, 5000, false);
			else
				Print("[AZ_Vamp] crypt traps: no crypt after 5 min, none built");
			return;
		}


		AZ_VampCryptTraps.SealRoom(!AZ_VampCrypt.EntryOn());
		AZ_VampCryptTraps.Build();
		AZ_VampCryptLurkers.Build();
		AZ_VampCryptCage.Build();
		AZ_VampCandleHall.Build();
		AZ_VampCryptSarcophagus.Build();
		Print("[AZ_Vamp] crypt build done");
		string probe = AZ_VampState.DIR + "/trap_probe.txt";
		if (FileExist(probe))
		{
			Print("[AZ_Vamp] trap probe requested");
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampCryptTraps.Probe, 20000, false);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampCryptCage.Probe, 65000, false);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampCandleHall.Probe, 115000, false);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampCryptSarcophagus.Probe, 140000, false);


			vector river = AZ_VampCryptLurkers.RiverSpot();
			vector toApse = AZ_VampCryptLurkers.RiverFace();
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampCryptLurkers.RiverProbe, 160000, false, river, toApse);
		}
	}

	string HwTrapLabel(int step)
	{
		if (step == 98)
			return "trap_plate";
		if (step == 99)
			return "trap_spikes";
		if (step == 160)
			return "trap_boulder";
		if (step == 161)
			return "trap_boulder_roll";
		if (step == 162)
			return "trap_lurkers";
		if (step == 163)
			return "trap_cage";
		if (step == 164)
			return "trap_candles";
		return "";
	}

	override string HwStepLabel(int step)
	{
		if (step == 98 || step == 99 || (step >= 160 && step <= 164))
			return HwTrapLabel(step);
		return super.HwStepLabel(step);
	}

	override int HwNextStep(int step)
	{
		if (step == 97)
			return 98;
		if (step == 98)
			return 99;
		if (step == 99)
			return 160;
		if (step == 160)
			return 161;
		if (step == 161)
			return 162;
		if (step == 162)
			return 163;
		if (step == 163)
			return 164;
		if (step == 164)
			return super.HwNextStep(97);
		return super.HwNextStep(step);
	}

	override void HwStep(PlayerBase p, int step)
	{
		if (step != 98 && step != 99 && (step < 160 || step > 164))
		{
			super.HwStep(p, step);
			return;
		}
		if (!p)
			return;
		if (!HwWanted(step) || !AZ_VampCryptTraps.s_Built)
		{
			if (HwWanted(step))
				Print("[AZ_Vamp] TEST trap shot " + HwTrapLabel(step) + ": traps not built");
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 100, false, p, HwNextStep(step));
			return;
		}
		if (step == 162)
		{
			HwLurkerStep(p);
			return;
		}
		if (step == 163)
		{
			HwCageStep(p);
			return;
		}
		if (step == 164)
		{
			HwCandleStep(p);
			return;
		}
		if (step >= 160)
		{
			HwBoulderStep(p, step);
			return;
		}

		AZ_VampCryptTraps.ForceArm();
		AZ_VampCryptTraps.s_WedgedUntil = GetGame().GetTime() + 60000;


		if (step == 99 && AZ_VampCryptTraps.s_Spikes)
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AZ_VampCryptTraps.s_Spikes.SetTrapState, 4000 + 2700, false, AZ_VampCryptTraps.UP);
		vector stand = AZ_VampCrypt.Hall(-10.5, 2.5, 0.05);

		float sy = AZ_VampCryptTraps.SPIKE_Y;
		vector cam = AZ_VampCrypt.Hall(AZ_VampCryptTraps.SPIKE_F - 2.6, AZ_VampCryptTraps.SPIKE_R - 0.6, sy + 1.6);
		if (step == 99)
			cam = AZ_VampCrypt.Hall(AZ_VampCryptTraps.SPIKE_F - 1.6, AZ_VampCryptTraps.SPIKE_R - 0.3, sy + 1.3);
		vector look = AZ_VampCrypt.Hall(AZ_VampCryptTraps.SPIKE_F, AZ_VampCryptTraps.SPIKE_R, sy + 0.15);
		p.SetPosition(stand);
		vector face = look - stand;
		face[1] = 0;
		p.SetDirection(face.Normalized());
		string label = HwTrapLabel(step);
		Print("[AZ_Vamp] TEST trap " + label + " at " + stand + " spikes static " + (AZ_VampCryptTraps.s_Spikes != null));
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 4000, false, p, label, cam, look);
		if (step == 99)
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AZ_VampCryptTraps.ForceArm, 9700, false);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 10000, false, p, HwNextStep(step));
	}



	void HwCandleStep(PlayerBase p)
	{
		AZ_VampCandleHall.s_ReadyAt = 0;
		if (AZ_VampCandleHall.s_Next)
			AZ_VampCandleHall.s_Next.Clear();
		vector stand = AZ_VampCrypt.Hall(-16.2, 4.8, 0.05);
		vector cam = AZ_VampCrypt.Hall(-16.3, 4.6, 1.8);
		vector look = AZ_VampCrypt.Hall(-19.0, -2.0, 0.8);
		p.SetPosition(stand);
		Print("[AZ_Vamp] TEST trap candles, built " + AZ_VampCandleHall.s_Built);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 4000, false, p, "trap_candles", cam, look);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwCandleWalkIn, 8000, false, p);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 12000, false, p, HwNextStep(164));
	}

	void HwCandleWalkIn(PlayerBase p)
	{
		if (!p)
			return;
		if (!AZ_VampCryptTraps.IsHuman(p))
			AZ_VampEvent.Cure(p);
		p.SetPosition(AZ_VampCrypt.Hall(-18.0, 3.0, 0.05));
		Print("[AZ_Vamp] TEST trap candles: tester steps into the gallery, human " + AZ_VampCryptTraps.IsHuman(p) + " in stretch " + AZ_VampCandleHall.InStretch(p.GetPosition()) + " traps on " + AZ_VampCryptTraps.On());
	}


	void HwCageStep(PlayerBase p)
	{
		AZ_VampCryptTraps.s_AlarmReadyAt = GetGame().GetTime() + 60000;
		vector stand = AZ_VampCrypt.Hall(-11.4, -1.6, 0.05);
		vector cam = AZ_VampCrypt.Hall(-12.0, -0.4, 2.0);
		vector look = AZ_VampCrypt.Hall(AZ_VampCryptCage.CAGE_F, AZ_VampCryptCage.CAGE_R + 0.6, 1.0);
		p.SetPosition(stand);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AZ_VampCryptCage.ForceDown, 2500, false);
		Print("[AZ_Vamp] TEST trap cage, cage " + (AZ_VampCryptCage.s_Cage != null));
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 4000, false, p, "trap_cage", cam, look);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AZ_VampCryptCage.ForceArm, 9800, false);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 10000, false, p, HwNextStep(163));
	}



	void HwLurkerStep(PlayerBase p)
	{
		AZ_VampCryptTraps.s_AlarmReadyAt = GetGame().GetTime() + 60000;
		vector stand = AZ_VampCrypt.Hall(-7.5, -11.4, 0.05);
		vector cam = AZ_VampCrypt.Hall(-9.2, -11.3, 1.7);
		vector look = AZ_VampCrypt.Hall(-12.3, -11.95, 0.3);
		p.SetPosition(stand);
		Print("[AZ_Vamp] TEST trap lurkers, sleepers placed " + (AZ_VampCryptLurkers.s_Sleepers != null));
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 4000, false, p, "trap_lurkers", cam, look);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 10000, false, p, HwNextStep(162));
	}



	void HwBoulderStep(PlayerBase p, int step)
	{
		AZ_VampCryptTraps.ForceArm();
		AZ_VampCryptTraps.s_AlarmReadyAt = GetGame().GetTime() + 60000;
		AZ_VampCryptTraps.s_WedgedUntil = GetGame().GetTime() + 60000;
		vector stand = AZ_VampCrypt.Hall(-11.2, -0.7, 0.05);
		vector cam = AZ_VampCrypt.Hall(-12.1, -0.6, 1.7);
		vector look = AZ_VampCrypt.Hall(-7.5, AZ_VampCryptTraps.CW_R, 1.9);
		p.SetPosition(stand);
		vector face = look - stand;
		face[1] = 0;
		p.SetDirection(face.Normalized());
		string label = HwTrapLabel(step);
		int shotAt = 4000;
		if (step == 161)
		{


			int releaseAt = shotAt + 3000 - AZ_VampCryptTraps.RUMBLE_MS - 1250;
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AZ_VampCryptTraps.Release, releaseAt, false, "tour");
			float f = AZ_VampCryptTraps.BallF(1250);
			Print("[AZ_Vamp] TEST trap boulder predicted at shot time: " + AZ_VampCrypt.Hall(f, AZ_VampCryptTraps.CW_R, AZ_VampCryptTraps.BallY(f)));
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AZ_VampCryptTraps.ForceArm, 9500, false);
		}
		Print("[AZ_Vamp] TEST trap " + label + " at " + stand + " ball " + (AZ_VampCryptTraps.s_Ball != null));
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, shotAt, false, p, label, cam, look);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 10000, false, p, HwNextStep(step));
	}
}
