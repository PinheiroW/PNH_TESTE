





modded class AZ_VampConfig
{
	int cryptBloodBags = 6;
}

class AZ_VampCryptBlood
{
	static const float LIFETIME = 3888000;
	static bool s_WasNight;

	static int Cap()
	{
		if (!AZ_VampState.Cfg())
			return 6;
		return Math.Min(AZ_VampState.Cfg().cryptBloodBags, 6);
	}


	static vector Spot(int i, out float yaw)
	{
		yaw = AZ_VampCrypt.HALL_YAW + 20 * i;
		if (i < 3 && AZ_VampCrypt.AnteOn())
			return AZ_VampCrypt.Nave(23.6 + 0.35 * i, 13.75, 0.05);
		if (i < 4)
			return AZ_VampCrypt.Hall(-7.95 + 0.3 * i, -5.85, 0.05);
		float s = 1;
		if (i == 4)
			s = -1;
		return AZ_VampCrypt.Nave(AZ_VampCrypt.ALTAR_F + 0.3, s * 4.7, AZ_VampCrypt.DAIS_Y + 0.33);
	}

	static bool Has(vector pos)
	{
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition(pos, 0.3, objs, null);
		foreach (Object o : objs)
		{
			EntityAI e = EntityAI.Cast(o);
			if (e && e.IsKindOf("BloodBagFull") && !e.GetHierarchyParent())
				return true;
		}
		return false;
	}

	static void Restock()
	{
		if (!GetGame().IsServer() || !AZ_VampCrypt.s_Built)
			return;
		int cap = Cap();
		int n = 0;
		for (int i = 0; i < cap; i++)
		{
			float yaw;
			vector pos = Spot(i, yaw);
			if (!Has(pos))
			{
				ItemBase bag = ItemBase.Cast(GetGame().CreateObjectEx("BloodBagFull", pos, ECE_CREATEPHYSICS | ECE_KEEPHEIGHT));
				if (!bag)
					continue;
				bag.SetOrientation(Vector(yaw, 0, 0));
				bag.SetLiquidType(LIQUID_BLOOD_0_N);
				bag.SetLifetimeMax(LIFETIME);
				bag.SetLifetime(LIFETIME);
			}
			n++;
		}
		Print("[AZ_Vamp] crypt blood: " + n + " of " + cap + " bags in the cold store");
	}


	static void Tick()
	{
		bool night = GetGame().GetWorld().IsNight();
		if (night && !s_WasNight)
			Restock();
		s_WasNight = night;
	}
}
