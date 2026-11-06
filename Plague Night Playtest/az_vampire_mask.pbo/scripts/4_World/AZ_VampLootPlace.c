






class AZ_VampLootPlace
{
	static const float SETTLE_UP = 0.5;
	static const float SETTLE_DOWN = 1.5;
	static const float GAP_SUNK = -2.0;
	static const float GAP_FLOAT = 3.0;
	static ref array<EntityAI> s_Items = new array<EntityAI>;
	static ref array<string> s_Where = new array<string>;

	static ref array<EntityAI> s_AuditShow = new array<EntityAI>;

	static void ServerReset()
	{
		s_Items.Clear();
		s_Where.Clear();
		s_AuditShow.Clear();
	}


	static bool Floor(EntityAI e, vector at, out vector hit)
	{
		vector from = at + Vector(0, SETTLE_UP, 0);
		vector to = at - Vector(0, SETTLE_DOWN, 0);
		vector dir;
		int comp;
		set<Object> res = new set<Object>;
		return DayZPhysics.RaycastRV(from, to, hit, dir, comp, res, null, e, true, false, ObjIntersectGeom);
	}

	static float Bottom(EntityAI e)
	{
		vector mm[2];
		e.ClippingInfo(mm);
		return mm[0][1];
	}


	static string Boxed(EntityAI e, vector floor)
	{
		vector mm[2];
		e.ClippingInfo(mm);
		float h = Math.Max(0.1, mm[1][1] - mm[0][1]);
		vector from = floor + Vector(0, 0.03, 0);
		vector to = floor + Vector(0, h, 0);
		vector hit, dir;
		int comp;
		set<Object> res = new set<Object>;
		if (!DayZPhysics.RaycastRV(from, to, hit, dir, comp, res, null, e, true, false, ObjIntersectGeom))
			return "";
		string what = "something";
		if (res.Count() > 0 && res[0])
			what = res[0].GetType();
		return what + " " + Math.Round((hit[1] - floor[1]) * 100) + " cm up";
	}


	static void Track(EntityAI e, string where)
	{
		if (e && s_Items.Find(e) < 0)
		{
			s_Items.Insert(e);
			s_Where.Insert(where);
		}
	}


	static void Settle(EntityAI e, string where)
	{
		if (!e || !GetGame().IsServer() || e.GetHierarchyParent())
			return;
		if (s_Items.Find(e) < 0)
		{
			s_Items.Insert(e);
			s_Where.Insert(where);
		}
		vector at = e.GetPosition();
		vector floor;
		if (!Floor(e, at, floor))
		{
			Print("[AZ_Vamp] loot place: " + e.GetType() + " (" + where + ") no floor within " + SETTLE_DOWN + " m under " + at + ", left as placed");
			return;
		}
		vector put = Vector(at[0], floor[1] - Bottom(e) + 0.005, at[2]);
		e.SetPosition(put);
		string box = Boxed(e, floor);
		string note = "";
		if (box != "")
			note = ", BOXED IN by " + box;
		Print("[AZ_Vamp] loot place: " + e.GetType() + " (" + where + ") settled " + Math.Round((put[1] - at[1]) * 100) + " cm onto " + floor + note);
	}


	static string Audit()
	{
		int ok = 0;
		int fail = 0;
		array<EntityAI> fails = new array<EntityAI>;
		array<EntityAI> oks = new array<EntityAI>;
		for (int i = s_Items.Count() - 1; i >= 0; i--)
		{
			EntityAI e = s_Items[i];
			if (!e || e.GetHierarchyParent())
			{
				s_Items.Remove(i);
				s_Where.Remove(i);
				continue;
			}
			vector at = e.GetPosition();
			vector floor;
			bool hasFloor = Floor(e, at + Vector(0, Bottom(e), 0), floor);
			float gap = 0;
			string box = "";
			if (hasFloor)
			{
				gap = (at[1] + Bottom(e) - floor[1]) * 100;
				box = Boxed(e, floor);
			}
			bool bad = !hasFloor || gap < GAP_SUNK || gap > GAP_FLOAT || box != "";
			string verdict = "ok";
			if (bad)
			{
				verdict = "FAIL";
				fail++;
				fails.Insert(e);
			}
			else
			{
				ok++;
				oks.Insert(e);
			}
			string overlap = "no";
			if (box != "")
				overlap = "yes (" + box + ")";
			string gapS = "no floor";
			if (hasFloor)
				gapS = Math.Round(gap).ToString() + " cm";
			Print("[AZ_Vamp] loot audit: " + verdict + " " + e.GetType() + " (" + s_Where[i] + ") at " + at + ", gap " + gapS + ", overlap " + overlap);
		}
		s_AuditShow.Clear();
		foreach (EntityAI f : fails)
			s_AuditShow.Insert(f);
		for (int k = 0; k < 3 && oks.Count() > 0; k++)
		{
			int pick = Math.RandomInt(0, oks.Count());
			s_AuditShow.Insert(oks[pick]);
			oks.Remove(pick);
		}
		string sum = "[AZ_Vamp] loot audit: " + ok + " ok, " + fail + " fail";
		Print(sum);
		return sum;
	}
}
