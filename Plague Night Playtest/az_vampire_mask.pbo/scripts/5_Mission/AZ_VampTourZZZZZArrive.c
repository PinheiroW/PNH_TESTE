











modded class MissionServer
{
	override string HwStepLabel(int step)
	{
		if (step >= 700 && step <= 705)
			return "hw_arrive";
		if (step >= 710 && step <= 712)
			return "hw_skull";
		return super.HwStepLabel(step);
	}

	override int HwNextStep(int step)
	{
		if (step == AZ_VampStanceTest.LAST)
			return 700;
		if (step >= 700 && step < 705)
			return step + 1;
		if (step == 705)
			return 710;
		if (step >= 710 && step < 712)
			return step + 1;
		if (step == 712)
			return super.HwNextStep(AZ_VampStanceTest.LAST);
		return super.HwNextStep(step);
	}

	override void HwStep(PlayerBase p, int step)
	{
		if (step >= 710 && step <= 712)
		{
			HwSkullStep(p, step);
			return;
		}
		if (step < 700 || step > 705)
		{
			super.HwStep(p, step);
			return;
		}
		if (!p)
			return;
		int nextStep = HwNextStep(step);
		if (!HwWanted(step))
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 100, false, p, nextStep);
			return;
		}
		int next = 12000;
		vector at;
		array<vector> slots;
		switch (step)
		{
			case 700:
			{
				AZ_VampSnap.s_Trace = true;
				p.SetAllowDamage(false);
				slots = AZ_VampCrypt.MouthSlots();
				p.SetPosition(slots[0]);
				p.SetDirection(-AZ_VampCrypt.Fwd(AZ_VampCrypt.DOOR_YAW));
				next = 15000;
				break;
			}
			case 701:
			{
				AZ_VampSnap.s_NoHold = true;
				slots = AZ_VampCryptLedge.Slots();
				at = slots[1];
				AZ_VampCrypt.MoveTo(p, at, AZ_VampCryptLedge.Facing(at));
				break;
			}
			case 703:
			{
				AZ_VampSnap.s_NoHold = false;
				slots = AZ_VampCryptLedge.Slots();
				at = slots[1];
				AZ_VampCrypt.MoveTo(p, at, AZ_VampCryptLedge.Facing(at));
				break;
			}
			case 702:
			{
				AZ_VampSnap.s_NoHold = false;
				slots = AZ_VampCrypt.MouthSlots();
				at = slots[0];
				AZ_VampCrypt.MoveTo(p, at, -AZ_VampCrypt.Fwd(AZ_VampCrypt.DOOR_YAW));
				next = 15000;
				break;
			}
			case 704:
			{
				slots = AZ_VampCryptExit.VorkutaSlots();
				at = slots[1];
				AZ_VampCrypt.MoveTo(p, at, AZ_VampCryptExit.VorkutaFwd());
				break;
			}
			case 705:
			{
				AZ_VampSnap.s_Trace = false;
				AZ_VampSnap.s_NoHold = false;
				p.SetAllowDamage(true);
				next = 1000;
				break;
			}
		}
		bool hold = !AZ_VampSnap.s_NoHold;
		Print("[AZ_Vamp] TEST arrive step " + step + " hold " + hold + " to " + at + " from " + p.GetPosition());
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, next, false, p, nextStep);
	}





	void HwSkullStep(PlayerBase p, int step)
	{
		if (!p)
			return;
		int nextStep = HwNextStep(step);
		if (!HwWanted(step))
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 100, false, p, nextStep);
			return;
		}
		Object wall = AZ_VampCrypt.s_DoorWall;
		if (!wall)
		{
			Print("[AZ_Vamp] TEST skull: no door wall");
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 1000, false, p, nextStep);
			return;
		}
		float yaw = AZ_VampCrypt.HALL_YAW + 180;
		vector side = AZ_VampCrypt.Side(yaw);
		vector fwd = AZ_VampCrypt.Fwd(yaw);
		vector wp = wall.GetPosition();
		vector cam;
		vector look;
		switch (step)
		{
			case 710:
			{
				p.SetAllowDamage(false);
				vector sk = AZ_VampCrypt.SkullPos();
				vector stand = sk + side * 0.6;
				stand[1] = wp[1];
				bool found = AZ_VampSnap.FloorUnder(stand + Vector(0, 0.05, 0), p);
				vector face = sk - stand;
				face[1] = 0;
				AZ_VampSnap.Far(p, stand + Vector(0, 0.05, 0), face.Normalized());
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwSkullPress, 3000, false, p);
				cam = stand + Vector(0, 1.6, 0) - face.Normalized() * 1.5;
				look = wp - side * 2.0 + Vector(0, 1.2, 0);
				Print("[AZ_Vamp] TEST skull 710: stand " + stand + " found " + found + " skull " + sk);
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 7000, false, p, "hw_skull_door", cam, look);
				break;
			}
			case 711:
			{
				vector coat = wp - side * 5.2 + fwd * 1.3;
				array<Object> near = new array<Object>;
				GetGame().GetObjectsAtPosition(coat, 1.5, near, null);
				string got = "none";
				foreach (Object o : near)
				{
					if (o && o.GetType() == AZ_VampCostumes.GREATCOAT)
						got = o.GetType() + " at " + o.GetPosition();
				}
				Print("[AZ_Vamp] TEST skull 711: coat " + got);
				vector inRoom = wp - side * 2.6;
				inRoom[1] = wp[1] + 0.05;
				AZ_VampSnap.Far(p, inRoom, -side);
				cam = wp - side * 1.6 + Vector(0, 1.6, 0);
				look = coat;
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 4000, false, p, "hw_skull_room", cam, look);
				break;
			}
			case 712:
			{
				cam = wp - side * 2.0 + Vector(0, 1.6, 0) + fwd * 1.5;
				look = wp - side * 6.0 + Vector(0, 1.0, 0) - fwd * 1.5;
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 1000, false, p, "hw_skull_far", cam, look);
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(p.SetAllowDamage, 6000, false, true);
				break;
			}
		}
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 12000, false, p, nextStep);
	}

	void HwSkullPress(PlayerBase p)
	{
		bool at = AZ_VampCrypt.AtSkull(p);
		AZ_VampCrypt.PressSkull(p);
		bool open = AZ_VampCrypt.s_Slab && AZ_VampCrypt.s_Slab.IsOpen();
		Print("[AZ_Vamp] TEST skull 710: at skull " + at + ", slab " + (AZ_VampCrypt.s_Slab != null) + " open " + open + ", cryptEntrance " + AZ_VampCrypt.EntryOn());
	}
}
