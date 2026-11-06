








modded class MissionServer
{
	string HwPortalLabel(int step)
	{
		if (step == 170)
			return "crypt_portal";
		if (step == 171)
			return "crypt_portal_wide";
		return "";
	}

	override string HwStepLabel(int step)
	{
		if (step == 170 || step == 171)
			return HwPortalLabel(step);
		return super.HwStepLabel(step);
	}

	override int HwNextStep(int step)
	{
		if (step == 154)
			return 170;
		if (step == 170)
			return 171;
		if (step == 171)
			return super.HwNextStep(154);
		return super.HwNextStep(step);
	}


	void HwPortalSwap()
	{
		vector origin = AZ_VampCrypt.DoorOrigin();
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition(origin, 3, objs, null);
		bool have = false;
		foreach (Object o : objs)
		{
			if (!o)
				continue;
			if (o.IsKindOf("Land_Underground_Entrance"))
				GetGame().ObjectDelete(o);
			else if (o.IsKindOf("AZ_Crypt_Portal"))
				have = true;
		}
		if (!have)
		{
			Object pt = GetGame().CreateObjectEx("AZ_Crypt_Portal", origin, ECE_KEEPHEIGHT | ECE_CREATEPHYSICS | ECE_NOLIFETIME);
			if (pt)
			{
				pt.SetPosition(origin);
				pt.SetOrientation(Vector(AZ_VampCrypt.DOOR_YAW, 0, 0));
			}
			Print("[AZ_Vamp] TEST portal spawned " + (pt != null) + " at " + origin);
		}

		for (float z = 8.0; z >= -18.0; z -= 2.0)
		{
			string row = "[AZ_Vamp] TEST portal grid z " + z + ":";
			for (float x = -14.0; x <= 14.0; x += 2.0)
			{
				vector g = AZ_VampCrypt.Door(x, z, true);
				float dy = g[1] - origin[1];
				row = row + " " + dy.ToString();
			}
			Print(row);
		}
	}

	override void HwStep(PlayerBase p, int step)
	{
		if (step != 170 && step != 171)
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
		if (step == 170)
			HwPortalSwap();
		float front = AZ_VampCrypt.DOOR_FRONT;
		vector stand = AZ_VampCrypt.Door(2.5, -front - 9.0, true);
		vector cam = AZ_VampCrypt.Door(1.2, -front - 10.5, true);
		cam[1] = cam[1] + 1.7;
		vector look = AZ_VampCrypt.Door(0, -front, false);
		look[1] = look[1] + 2.6;
		if (step == 171)
		{
			stand = AZ_VampCrypt.Door(8.0, -front - 18.0, true);
			cam = AZ_VampCrypt.Door(7.0, -front - 20.0, true);
			cam[1] = cam[1] + 1.8;
			look[1] = look[1] + 1.0;
		}
		p.SetPosition(stand);
		vector face = look - stand;
		face[1] = 0;
		p.SetDirection(face.Normalized());
		string label = HwPortalLabel(step);
		Print("[AZ_Vamp] TEST crypt " + label + " at " + stand);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 5000, false, p, label, cam, look);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 11000, false, p, HwNextStep(step));
	}
}
