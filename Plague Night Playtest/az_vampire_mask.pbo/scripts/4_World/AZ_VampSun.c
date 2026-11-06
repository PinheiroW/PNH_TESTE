



class AZ_VampSun
{
	static const float LENGTH = 50.0;
	static const float RADIUS = 0.05;


	static vector AZ_Cross(vector a, vector b)
	{
		return Vector(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]);
	}


	static bool SunUp()
	{
		if (AZ_VampState.s_ProbeDay)
			return false;
		return !GetGame().GetWorld().IsNight() && GetGame().GetWorld().GetSunOrMoon() >= 0.5;
	}

	static bool Exposed(PlayerBase p, out vector from, out vector to)
	{
		from = MiscGameplayFunctions.GetPlayerHeadPosition(p);
		to = from - GetGame().GetWorld().GetSunOrMoonDirection() * LENGTH;
		if (!SunUp() || p.IsInVehicle())
			return false;
		return ExposedFrom(from, to);
	}



	static bool ExposedFrom(vector from, vector to)
	{


		vector up = (to - from).Normalized();
		vector fwd = vector.Up;
		if (Math.AbsFloat(vector.Dot(fwd, up)) > 0.99)
			fwd = vector.Aside;
		vector right = AZ_Cross(fwd, up).Normalized();
		vector back = AZ_Cross(up, right).Normalized();
		vector m[4];
		m[0] = right;
		m[1] = up;
		m[2] = back;
		m[3] = (from + to) * 0.5;

		PhxInteractionLayers mask;
		mask |= PhxInteractionLayers.ITEM_LARGE;
		mask |= PhxInteractionLayers.BUILDING;
		mask |= PhxInteractionLayers.VEHICLE;
		mask |= PhxInteractionLayers.TERRAIN;
		mask |= PhxInteractionLayers.ROADWAY;
		ShadowDetectionCollisionCallback cb = new ShadowDetectionCollisionCallback();
		return !DayZPhysics.CapsuleOverlapBullet(m, RADIUS, LENGTH, mask, cb);
	}

	static bool Exposed(PlayerBase p)
	{
		vector a, b;
		return Exposed(p, a, b);
	}



	static float WeatherShield()
	{
		Weather w = GetGame().GetWeather();
		float overcast = w.GetOvercast().GetActual();
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		float multiplier = 1;
		if (overcast > 0.85)
			multiplier = cfg.sunCloudHeavyMult;
		else if (overcast > 0.6)
			multiplier = cfg.sunCloudLightMult;
		if (w.GetFog().GetActual() > 0.5 || w.GetSnowfall().GetActual() > 0.5)
			multiplier = Math.Min(multiplier, cfg.sunFogSnowMult);
#ifdef NAMALSK_SURVIVAL
		NamEventManager events = NamEventManager.GetInstance();
		if (events && events.IsEventActive(Blizzard))
			multiplier = Math.Min(multiplier, cfg.sunCloudHeavyMult);
#endif
		// Weather buys time outdoors; it must never grant sun immunity.
		return Math.Clamp(multiplier, 0.05, 1);
	}



	static float SkinCover(PlayerBase p)
	{
		float k = 1.0;
		if (p.FindAttachmentBySlotName("Headgear"))
			k -= 0.25;
		if (p.FindAttachmentBySlotName("Gloves"))
			k -= 0.25;
		if (p.FindAttachmentBySlotName("Eyewear"))
			k -= 0.25;

		EntityAI body = p.FindAttachmentBySlotName("Body");
		if (body && body.GetType().IndexOf("AZ_VampElder") == 0)
			k -= 0.5;

		EntityAI back = p.FindAttachmentBySlotName("Back");
		if (back && back.IsKindOf("GhillieSuit_ColorBase"))
			k -= 0.85;
		else if (back && back.IsKindOf("GhillieTop_ColorBase"))
			k -= 0.5;
		else if (back && back.IsKindOf("GhillieBushrag_ColorBase"))
			k -= 0.25;
		// Additive clothing bonuses cannot make any outfit sun-proof.
		return Math.Max(Math.Clamp(AZ_VampState.Cfg().sunClothingBurnFloor, 0, 1), k);
	}



	static bool DiagHit(vector from, vector dir, float len, PhxInteractionLayers mask)
	{
		vector to = from + dir * len;
		vector up = dir;
		vector fwd = vector.Up;
		if (Math.AbsFloat(vector.Dot(fwd, up)) > 0.99)
			fwd = vector.Aside;
		vector right = AZ_Cross(fwd, up).Normalized();
		vector back = AZ_Cross(up, right).Normalized();
		vector m[4];
		m[0] = right;
		m[1] = up;
		m[2] = back;
		m[3] = (from + to) * 0.5;
		ShadowDetectionCollisionCallback cb = new ShadowDetectionCollisionCallback();
		return DayZPhysics.CapsuleOverlapBullet(m, RADIUS, len, mask, cb);
	}

	static void DiagGrid(string label)
	{
		vector c = "6904 0 10958";
		vector dir = -GetGame().GetWorld().GetSunOrMoonDirection();
		dir.Normalize();
		array<int> layers = {PhxInteractionLayers.TERRAIN, PhxInteractionLayers.BUILDING, PhxInteractionLayers.ITEM_LARGE, PhxInteractionLayers.VEHICLE, PhxInteractionLayers.ROADWAY};
		array<string> names = {"terrain", "building", "itemLarge", "vehicle", "roadway"};
		int total = 0, open50 = 0, open10 = 0, openUp = 0;
		array<int> hits = {0, 0, 0, 0, 0};
		PhxInteractionLayers all = PhxInteractionLayers.TERRAIN | PhxInteractionLayers.BUILDING | PhxInteractionLayers.ITEM_LARGE | PhxInteractionLayers.VEHICLE | PhxInteractionLayers.ROADWAY;
		for (int i = -4; i <= 4; i++)
		{
			for (int j = -4; j <= 4; j++)
			{
				vector at = c + Vector(i * 50, 0, j * 50);
				at[1] = GetGame().SurfaceY(at[0], at[2]) + 1.6;
				total++;
				if (!DiagHit(at, dir, LENGTH, all))
					open50++;
				if (!DiagHit(at, dir, 10, all))
					open10++;
				if (!DiagHit(at, vector.Up, LENGTH, all))
					openUp++;
				for (int k = 0; k < layers.Count(); k++)
				{
					if (DiagHit(at, dir, LENGTH, layers[k]))
						hits[k] = hits[k] + 1;
				}
			}
		}
		string per = "";
		for (int n = 0; n < names.Count(); n++)
			per += " " + names[n] + "=" + hits[n];
		Print("[AZ_Vamp] sun diag " + label + " dir " + dir + " of " + total + ": open50 " + open50 + " open10 " + open10 + " openStraightUp " + openUp + " | blocked by" + per);
	}

	static void NoonCheck(int n)
	{
		World w = GetGame().GetWorld();
		if (n == 0)
			w.SetDate(2011, 11, 5, 12, 0);
		if (n == 2)
			DiagGrid("5Nov12h");
		if (n == 3)
			w.SetDate(2011, 6, 21, 12, 0);
		if (n == 5)
		{
			DiagGrid("21Jun12h");
			Print("[AZ_Vamp] sun check done");
			return;
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(NoonCheck, 5000, false, n + 1);
	}
}
