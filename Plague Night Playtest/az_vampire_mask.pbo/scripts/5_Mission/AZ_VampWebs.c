






class AZ_VampWebs
{
	static const float NEAR = 110;
	static const float FAR = 170;
	static const float SCAN = 4;
	static const string DIR = "DZ\\structures_takistan\\Props\\Cobwebs\\";
	static const string CEIL_WEB = "Cobweb_06_wwc.p3d";
	static const string FLOOR_WEB_A = "Cobweb_01_wwg.p3d";
	static const string FLOOR_WEB_B = "Cobweb_05_wwg.p3d";
	static const string WALL_WEB = "Cobweb_04_ww.p3d";

	protected float m_ScanIn;
	protected ref map<Building, ref array<Object>> m_Done = new map<Building, ref array<Object>>;
	protected int m_Count;
	protected bool m_Logged;

	void ~AZ_VampWebs()
	{
		foreach (Building b, array<Object> webs : m_Done)
			Clear(webs);
	}

	static void Clear(array<Object> webs)
	{
		if (!webs)
			return;
		foreach (Object w : webs)
		{
			if (w)
				GetGame().ObjectDelete(w);
		}
	}

	void Update(float dt)
	{
		m_ScanIn -= dt;
		if (m_ScanIn > 0)
			return;
		m_ScanIn = SCAN;
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return;
		if (!player.m_AZHalloween)
		{
			foreach (Building ob, array<Object> owebs : m_Done)
				Clear(owebs);
			m_Done.Clear();
			return;
		}
		vector me = player.GetPosition();

		array<Building> gone = new array<Building>;
		foreach (Building hb, array<Object> hw : m_Done)
		{
			if (!hb || vector.Distance(hb.GetPosition(), me) > FAR)
				gone.Insert(hb);
		}
		foreach (Building g : gone)
		{
			array<Object> gw = m_Done.Get(g);
			if (gw)
				m_Count -= gw.Count();
			Clear(gw);
			m_Done.Remove(g);
		}
		array<Object> objs = new array<Object>;
		array<CargoBase> proxies = new array<CargoBase>;
		GetGame().GetObjectsAtPosition(me, NEAR, objs, proxies);
		int fresh = 0;
		foreach (Object o : objs)
		{
			Building b = Building.Cast(o);
			if (!b || b.GetDoorCount() == 0 || m_Done.Contains(b) || !AZ_VampDecor.IsDecor(b.GetType()))
				continue;
			array<Object> webs = new array<Object>;
			m_Done.Insert(b, webs);
			Dress(b, webs);
			m_Count += webs.Count();
			if (++fresh >= 4)
				break;
		}
		if (fresh > 0 && !m_Logged)
		{
			m_Logged = true;
			Print("[AZ_Vamp] cobwebs: " + m_Count + " in " + m_Done.Count() + " houses near " + me);
		}
	}

	protected void Dress(Building b, array<Object> webs)
	{
		vector bp = b.GetPosition();
		int seed = bp[0] * 7 + bp[2] * 13;
		Math.Randomize(Math.AbsInt(seed));
		vector box[2];
		if (!b.GetCollisionBox(box) || box[1][0] - box[0][0] < 3 || box[1][2] - box[0][2] < 3)
		{
			Math.Randomize(-1);
			return;
		}
		float floorY = b.GetDoorSoundPos(0)[1] - 1.0;
		int want = Math.RandomInt(2, 5);
		for (int t = 0; t < 10 && webs.Count() < want; t++)
		{
			vector lp = Vector(Math.RandomFloat(box[0][0] + 1.0, box[1][0] - 1.0), 0, Math.RandomFloat(box[0][2] + 1.0, box[1][2] - 1.0));
			vector w = b.ModelToWorld(lp);
			vector inside = Vector(w[0], floorY + 1.4, w[2]);
			int kind = Math.RandomInt(0, 4);
			float y;
			if (kind <= 1)
			{
				float up = RayDist(inside, Vector(0, 1, 0), 3.0, b);
				if (up < 0)
					continue;
				y = inside[1] + up - 0.02;
			}
			else
			{
				float flo = RayDist(inside, Vector(0, -1, 0), 2.0, b);
				if (flo < 0)
					continue;
				y = inside[1] - flo + 0.02;
			}
			string mesh = CEIL_WEB;
			if (kind == 2)
			{
				mesh = FLOOR_WEB_A;
				if (Math.RandomInt(0, 2) == 0)
					mesh = FLOOR_WEB_B;
			}
			else if (kind == 3)
			{
				mesh = WALL_WEB;
				y = inside[1] - RayDist(inside, Vector(0, -1, 0), 2.0, b) + 0.02;
			}
			float probeY = y + 0.25;
			if (kind <= 1)
				probeY = y - 0.25;
			Object web = Corner(b, Vector(w[0], probeY, w[2]), y, kind <= 1, mesh);
			if (web)
				webs.Insert(web);
		}
		Math.Randomize(-1);
	}


	protected float RayDist(vector from, vector dir, float len, Building b)
	{
		vector hit, nrm;
		int comp;
		set<Object> res = new set<Object>;
		if (!DayZPhysics.RaycastRV(from, from + dir * len, hit, nrm, comp, res, null, GetGame().GetPlayer(), false, false, ObjIntersectView))
			return -1;
		if (res.Count() == 0)
			return -1;
		return vector.Distance(from, hit);
	}


	protected Object Corner(Building b, vector probe, float y, bool top, string mesh)
	{
		vector ax = b.VectorToParent("1 0 0");
		vector az = b.VectorToParent("0 0 1");
		ax[1] = 0;
		az[1] = 0;
		ax.Normalize();
		az.Normalize();
		float sx = 1;
		float dx = RayDist(probe, ax, 6, b);
		float dxn = RayDist(probe, ax * -1, 6, b);
		if (dx < 0 || (dxn >= 0 && dxn < dx))
		{
			sx = -1;
			dx = dxn;
		}
		float sz = 1;
		float dz = RayDist(probe, az, 6, b);
		float dzn = RayDist(probe, az * -1, 6, b);
		if (dz < 0 || (dzn >= 0 && dzn < dz))
		{
			sz = -1;
			dz = dzn;
		}
		if (dx < 0 || dz < 0 || dx > 5 || dz > 5)
			return null;
		vector corner = probe + ax * (sx * (dx - 0.02)) + az * (sz * (dz - 0.02));
		corner[1] = y;
		Object web = GetGame().CreateStaticObjectUsingP3D(DIR + mesh, corner, b.GetOrientation(), 1.0, true);
		if (!web)
			return null;
		vector mm[2];
		web.ClippingInfo(mm);
		float ex = 1;
		if (mm[0][0] + mm[1][0] < 0)
			ex = -1;
		float ez = 1;
		if (mm[0][2] + mm[1][2] < 0)
			ez = -1;

		float wx = mm[0][0];
		if (ex < 0)
			wx = mm[1][0];
		float wz = mm[0][2];
		if (ez < 0)
			wz = mm[1][2];
		float wy = mm[0][1];
		if (top)
			wy = mm[1][1];
		vector room = ax * -sx + az * -sz;
		vector ori = b.GetOrientation();
		for (int k = 0; k < 4; k++)
		{
			web.SetOrientation(Vector(ori[0] + 90 * k, 0, 0));
			if (vector.Dot(web.VectorToParent(Vector(ex, 0, ez)), room) > 1.5)
				break;
		}
		web.SetPosition(corner - web.VectorToParent(Vector(wx, wy, wz)));
		return web;
	}
}

modded class MissionGameplay
{
	protected ref AZ_VampWebs m_AZWebs;
	protected bool m_AZWebsChecked;

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		if (!m_AZWebs)
		{
			if (m_AZWebsChecked)
				return;
			m_AZWebsChecked = true;
			string world;
			GetGame().GetWorldName(world);
			world.ToLower();
			if (world != "namalsk")
				return;
			m_AZWebs = new AZ_VampWebs();
		}
		m_AZWebs.Update(timeslice);
	}
}
