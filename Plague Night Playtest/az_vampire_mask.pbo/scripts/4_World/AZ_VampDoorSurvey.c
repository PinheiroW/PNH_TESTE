







class AZ_VampDoorSurvey
{
	static const float CHURCH_X = 5515;
	static const float CHURCH_Z = 8143;
	static const float HALF = 60.0;
	static const float STEP = 2.0;
	static const float RISE_M = 3.5;
	static const float FALL_M = 1.0;
	static const float FRONT = 7.9;

	static int s_NoRise;
	static int s_NoApron;
	static int s_Holy;
	static int s_Blocked;
	static float s_BestRise;
	static string s_BestAt;

	static void Arm()
	{
		if (!GetGame().IsServer())
			return;
		if (FileExist(AZ_VampState.DIR + "/door_survey_flat.txt"))
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(FlatRun, 20000, false);
		if (!FileExist(AZ_VampState.DIR + "/door_survey.txt"))
			return;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Run, 20000, false);
	}









	static const float FLAT_X = 5490;
	static const float FLAT_Z = 8190;
	static const float FLAT_R = 15.0;
	static const float FLAT_STEP = 0.5;
	static const float FLAT_FALL = 0.25;
	static const float FP_W = 4.2;
	static const float FP_D = 3.5;
	static const float HOLY_MARGIN = 5.0;

	static void FlatRun()
	{
		FlatSweep(FLAT_X, FLAT_Z, FLAT_R);
		if (s_FlatBest > FLAT_FALL)
		{
			Print("[AZ_Vamp] door survey FLAT: nothing within " + FLAT_R + " m of " + FLAT_X + "/" + FLAT_Z + ", widening to 40 m round 5497/8177");
			FlatSweep(5497, 8177, 40.0);
		}
		if (s_FlatBest > FLAT_FALL)
			Print("[AZ_Vamp] door survey FLAT: NONE qualifies (best fall " + s_FlatBest + " at " + s_FlatBestLine + "): the door stays at the bluff");
		else
			Print("[AZ_Vamp] door survey FLAT BEST " + s_FlatBestLine);
	}

	static float s_FlatBest = 99;
	static string s_FlatBestLine;

	static void FlatSweep(float cx, float cz, float rad)
	{
		Object church = null;
		float churchR = 12.0;
		vector mm[2];
		vector c = Vector(CHURCH_X, 0, CHURCH_Z);
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition(c, 30, objs, null);
		foreach (Object o : objs)
		{
			if (o && o.GetType().Contains("Church"))
			{
				church = o;
				churchR = o.ClippingInfo(mm);
				c = o.GetPosition();
				break;
			}
		}
		Print("[AZ_Vamp] door survey FLAT: church " + church + " at " + c + " bounding radius " + churchR + ", sweep r " + rad + " round " + cx + "/" + cz);
		int tried = 0;
		int ok = 0;
		int noFlat = 0;
		int holy = 0;
		int blocked = 0;
		for (float dx = -rad; dx <= rad; dx += FLAT_STEP)
		{
			for (float dz = -rad; dz <= rad; dz += FLAT_STEP)
			{
				if (dx * dx + dz * dz > rad * rad)
					continue;
				tried++;
				vector f = Vector(cx + dx, 0, cz + dz);
				vector toC = c - f;
				toC[1] = 0;
				float yaw = Math.Atan2(-toC[0], -toC[2]) * Math.RAD2DEG;
				float a = yaw * Math.DEG2RAD;
				vector fwd = Vector(Math.Sin(a), 0, Math.Cos(a));
				vector side = Vector(Math.Cos(a), 0, -Math.Sin(a));

				float lo = 9999;
				float hi = -9999;
				for (float u = -FP_W * 0.5; u <= FP_W * 0.5 + 0.01; u += 0.5)
				{
					for (float v = 0; v <= FP_D + 0.01; v += 0.5)
					{
						vector q = f + side * u;
						q = q + fwd * v;
						float qy = GetGame().SurfaceY(q[0], q[2]);
						if (qy < lo)
							lo = qy;
						if (qy > hi)
							hi = qy;
					}
				}
				float fall = hi - lo;
				if (fall > FLAT_FALL)
				{
					noFlat++;
					continue;
				}

				array<vector> pts = new array<vector>;
				pts.Insert(f + side * (FP_W * 0.5));
				pts.Insert(f - side * (FP_W * 0.5));
				vector backC = f + fwd * FP_D;
				pts.Insert(backC + side * (FP_W * 0.5));
				pts.Insert(backC - side * (FP_W * 0.5));
				vector mouth = f - fwd * 1.6;
				array<vector> slots = new array<vector>;
				slots.Insert(mouth);
				slots.Insert(mouth - side * 1.2);
				slots.Insert(mouth + side * 1.2);
				vector m2 = mouth - fwd * 1.2;
				slots.Insert(m2);
				slots.Insert(m2 - side * 1.2);
				slots.Insert(m2 + side * 1.2);
				float holyMin = 9999;
				foreach (vector hp : pts)
				{
					vector hd = hp - c;
					hd[1] = 0;
					if (hd.Length() - churchR < holyMin)
						holyMin = hd.Length() - churchR;
				}
				foreach (vector sp : slots)
				{
					vector sd = sp - c;
					sd[1] = 0;
					if (sd.Length() - churchR < holyMin)
						holyMin = sd.Length() - churchR;
				}
				if (holyMin < HOLY_MARGIN)
				{
					holy++;
					continue;
				}

				vector fpC = f + fwd * (FP_D * 0.5);
				string block = FlatBlocked(fpC, 2.0 + FP_W * 0.5);
				if (block == "")
					block = FlatBlocked(mouth - fwd * 1.0, 4.0);
				if (block == "")
				{
					foreach (vector bs : slots)
					{
						block = FlatBlocked(bs, 0.8);
						if (block != "")
							break;
					}
				}
				if (block != "")
				{
					blocked++;
					continue;
				}
				ok++;
				vector origin = f + fwd * AZ_VampCrypt.DOOR_FRONT;
				float originY = lo - 0.25;
				string line = "DOOR_X " + origin[0] + " DOOR_Z " + origin[2] + " DOOR_YAW " + yaw + " | face " + f + " fall " + fall + " m, origin y " + originY + ", holy margin " + holyMin + " m, church " + toC.Length() + " m";
				if (ok <= 40)
					Print("[AZ_Vamp] door survey FLAT CANDIDATE " + line);
				if (fall < s_FlatBest)
				{
					s_FlatBest = fall;
					s_FlatBestLine = line;
				}
			}
		}
		Print("[AZ_Vamp] door survey FLAT sweep done: " + ok + " of " + tried + "; too steep " + noFlat + ", holy ground " + holy + ", blocked " + blocked);
	}

	static string FlatBlocked(vector at, float r)
	{
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition(at, r, objs, null);
		foreach (Object o : objs)
		{
			if (!o || o.IsInherited(Man) || o.IsInherited(ItemBase))
				continue;
			string t = o.GetType();
			t.ToLower();
			if (o.IsTree() || o.IsBuilding() || t.Contains("wreck") || t.Contains("cross") || t.Contains("grave") || t.Contains("tomb"))
				return t;
		}
		return "";
	}

	static float Ground(vector at)
	{
		return GetGame().SurfaceY(at[0], at[2]);
	}

	static void Run()
	{
		Object church = null;
		float churchR = 12.0;
		vector mm[2];
		vector c = Vector(CHURCH_X, 0, CHURCH_Z);
		c[1] = GetGame().SurfaceY(CHURCH_X, CHURCH_Z);
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition(c, 30, objs, null);
		foreach (Object o : objs)
		{
			if (o && o.GetType().Contains("Church"))
			{
				church = o;
				churchR = o.ClippingInfo(mm);
				c = o.GetPosition();
				break;
			}
		}
		Print("[AZ_Vamp] door survey: church " + church + " at " + c + " radius " + churchR);
		int found = 0;
		int checkedN = 0;
		for (float dx = -HALF; dx <= HALF; dx += STEP)
		{
			for (float dz = -HALF; dz <= HALF; dz += STEP)
			{
				vector f = Vector(c[0] + dx, 0, c[2] + dz);
				float fy = Ground(f);
				f[1] = fy;
				vector toC = c - f;
				toC[1] = 0;
				float dist = toC.Length();
				if (dist < churchR + 5 || dist > 85)
					continue;
				float baseYaw = Math.Atan2(-toC[0], -toC[2]) * Math.RAD2DEG;
				for (int k = -2; k <= 2; k++)
				{
					checkedN++;
					float yaw = baseYaw + k * 15.0;
					string line = Try(f, yaw, c, churchR);
					if (line != "")
					{
						found++;
						if (found <= 120)
							Print("[AZ_Vamp] door survey CANDIDATE " + line);
					}
				}
			}
		}
		Print("[AZ_Vamp] door survey done: " + found + " candidates of " + checkedN + " tries; no bank " + s_NoRise + ", no apron " + s_NoApron + ", holy ground " + s_Holy + ", blocked " + s_Blocked + "; the best bank with an open apron: " + s_BestRise + " m at " + s_BestAt);
	}

	static string Try(vector f, float yaw, vector c, float churchR)
	{
		float a = yaw * Math.DEG2RAD;
		vector fwd = Vector(Math.Sin(a), 0, Math.Cos(a));
		float foot = f[1];
		float rise = -99;
		for (int b = 2; b <= 8; b += 2)
		{
			float bm = b;
			vector back = f + fwd * bm;
			float r = Ground(back) - foot;
			if (r > rise)
				rise = r;
		}
		bool apron = true;
		for (int fa = 2; fa <= 8; fa += 2)
		{
			float fam = fa;
			vector aheadA = f - fwd * fam;
			if (Math.AbsFloat(Ground(aheadA) - foot) > FALL_M)
				apron = false;
		}
		if (apron && rise > s_BestRise)
		{
			s_BestRise = rise;
			s_BestAt = "face " + f + " yaw " + yaw;
		}
		if (rise < RISE_M)
		{
			s_NoRise++;
			return "";
		}
		if (!apron)
		{
			s_NoApron++;
			return "";
		}
		for (int fr = 2; fr <= 8; fr += 2)
		{
			float fm = fr;
			vector ahead = f - fwd * fm;
			if (Math.AbsFloat(Ground(ahead) - foot) > FALL_M)
				return "";
		}
		vector mouth = f - fwd * 1.6;
		float my = Ground(mouth);
		mouth[1] = my;
		vector mc = mouth - c;
		mc[1] = 0;
		if (mc.Length() < churchR + 5)
		{
			s_Holy++;
			return "";
		}
		vector fp = f + fwd * 1.0;
		vector ap = mouth - fwd * 2.0;
		string block = Blocked(fp, 6.0);
		if (block == "")
			block = Blocked(ap, 4.0);
		if (block != "")
		{
			s_Blocked++;
			return "";
		}
		vector eye = mouth + Vector(0, 1.7, 0);
		vector top = c + Vector(0, 4, 0);
		bool seen = Sees(eye, top);
		vector origin = f + fwd * FRONT;
		vector fc = f - c;
		fc[1] = 0;
		return "DOOR_X " + origin[0] + " DOOR_Z " + origin[2] + " DOOR_YAW " + yaw + " | face " + f + " rise " + rise + " church " + fc.Length() + " m, visible " + seen;
	}

	static string Blocked(vector at, float r)
	{
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition(at, r, objs, null);
		foreach (Object o : objs)
		{
			if (!o || o.IsInherited(Man) || o.IsInherited(ItemBase))
				continue;
			string t = o.GetType();
			if (o.IsTree() || o.IsBuilding() || t.Contains("Wreck") || t.Contains("wreck"))
				return t;
		}
		return "";
	}

	static bool Sees(vector from, vector to)
	{
		vector pos, dir;
		int comp;
		set<Object> hits = new set<Object>;
		if (!DayZPhysics.RaycastRV(from, to, pos, dir, comp, hits, null, null, false, false, ObjIntersectView))
			return true;
		foreach (Object o : hits)
		{
			if (o && o.GetType().Contains("Church"))
				return true;
		}
		return vector.Distance(pos, to) < 8.0;
	}
}
