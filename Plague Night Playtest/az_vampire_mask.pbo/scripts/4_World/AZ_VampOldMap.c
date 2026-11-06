






class AZ_VampOldMap extends ChernarusMap
{
	static const string TEX_CLOSED = "azo\\az_vampire_mask\\data\\oldmap\\az_vamp_oldmap_closed_co.paa";
	static const string TEX_NORMAL = "azo\\az_vampire_mask\\data\\oldmap\\az_vamp_oldmap_ca.paa";
	static const string TEX_BLOOD = "azo\\az_vampire_mask\\data\\oldmap\\az_vamp_oldmap_blood_ca.paa";
	static const string TEX_LEGEND = "azo\\az_vampire_mask\\data\\oldmap\\az_vamp_oldmap_legend_co.paa";

	protected int m_AZFace = -1;



	override void InitMapState()
	{
		SetObjectTexture(0, TEX_CLOSED);
		SetObjectTexture(1, TEX_NORMAL);
		SetObjectTexture(2, TEX_LEGEND);
		m_MapMarkerArray = new array<ref MapMarker>;
		if (g_Game.IsMultiplayer() && g_Game.IsServer())
			SyncMapMarkers();
	}


	override void EEInit()
	{
		super.EEInit();
		if (!GetGame().IsDedicatedServer())
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AZRefreshFace, 2000, true);
	}

	override void EEDelete(EntityAI parent)
	{
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(AZRefreshFace);
		super.EEDelete(parent);
	}

	void AZRefreshFace()
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p)
			return;
		int want = 0;
		if (p.m_AZBloodMoon)
			want = 1;
		if (want == m_AZFace)
			return;
		m_AZFace = want;
		if (want == 1)
			SetObjectTexture(1, TEX_BLOOD);
		else
			SetObjectTexture(1, TEX_NORMAL);
	}

	static const int RPC_MARKS = 4702321;


	static ref array<string> s_Lines;

	static AZ_VampMapMark Mark(string key, string anchor, string text, string show)
	{
		AZ_VampMapMark m = new AZ_VampMapMark;
		m.key = key;
		m.anchor = anchor;
		m.text = text;
		m.show = show;
		return m;
	}

	static array<ref AZ_VampMapMark> Defaults()
	{
		array<ref AZ_VampMapMark> d = new array<ref AZ_VampMapMark>;

		d.Insert(Mark("orb", "a1_orb", "She waits below. Come on a blood moon.", "always"));
		d.Insert(Mark("tomb", "tomb_mouth", "The dead leave by the chapel.", "always"));
		return d;
	}


	static bool Resolve(AZ_VampMapMark m, out vector pos)
	{
		string a = m.anchor;
		if (a == "")
		{
			pos = Vector(m.x, 0, m.z);
			return true;
		}
		if (a == "a1_crack")
			pos = AZ_VampCryptA1.Crack();
		else if (a == "a1_orb")
			pos = AZ_VampCryptA1.ORB;
		else if (a == "tomb_mouth")
			pos = AZ_VampCrypt.Mouth();
		else if (a.IndexOf("area:") == 0)
		{
			string world;
			GetGame().GetWorldName(world);
			world.ToLower();
			TFloatArray fp = new TFloatArray;
			GetGame().ConfigGetFloatArray("CfgWorlds " + world + " Names " + a.Substring(5, a.Length() - 5) + " position", fp);
			if (fp.Count() < 2)
				return false;
			pos = Vector(fp[0], 0, fp[1]);
		}
		else if (a.IndexOf("church:") == 0)
		{
			int n = a.Substring(7, a.Length() - 7).ToInt();
			if (!AZ_VampLandmarks.s_Churches || n < 0 || n >= AZ_VampLandmarks.s_Churches.Count() || !AZ_VampLandmarks.s_Churches[n])
				return false;
			pos = AZ_VampLandmarks.s_Churches[n].GetPosition();
		}
		else
			return false;
		return true;
	}


	static array<string> Lines()
	{
		array<string> res = new array<string>;
		array<ref AZ_VampMapMark> src = null;
		if (AZ_VampState.Cfg())
			src = AZ_VampState.Cfg().oldMapMarks;
		if (!src || src.Count() == 0)
			src = Defaults();
		foreach (AZ_VampMapMark m : src)
		{
			vector p;
			if (!m || m.text == "" || !Resolve(m, p))
				continue;
			string txt = m.text;
			txt.Replace("|", " ");
			txt.Replace("\n", " ");
			res.Insert(Math.Round(p[0]).ToString() + "|0|" + Math.Round(p[2]).ToString() + "|" + m.show + "|" + txt);
		}
		return res;
	}


	static array<string> ClientLines()
	{
		if (s_Lines && s_Lines.Count() > 0)
			return s_Lines;
		return Lines();
	}


	static bool Shown(string show, bool bloodMoon)
	{
		if (show == "bloodmoon")
			return bloodMoon;
		if (show == "night")
			return GetGame().GetWorld().IsNight() || bloodMoon;
		return true;
	}


	static vector CrackAt()
	{
		return AZ_VampCryptA1.Crack();
	}

	static vector DoorAt()
	{
		return AZ_VampCrypt.Mouth();
	}
}


modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (rpc_type != AZ_VampOldMap.RPC_MARKS || !GetGame().IsClient())
			return;
		Param1<string> d = new Param1<string>("");
		if (!ctx.Read(d))
			return;
		AZ_VampOldMap.s_Lines = new array<string>;
		d.param1.Split("\n", AZ_VampOldMap.s_Lines);
	}
}


class AZ_VampOldMaps
{
	static const string TYPE = "AZ_VampOldMap";
	static const int TOWN_COPIES = 3;
	static const float LIFETIME = 43200;

	static ref array<vector> s_At;
	static ref array<EntityAI> s_Map;

	static vector Ground(vector p)
	{
		p[1] = GetGame().SurfaceY(p[0], p[2]) + 0.03;
		return p;
	}



	static void Plan()
	{
		s_At = new array<vector>;
		s_Map = new array<EntityAI>;
		float minZ = AZ_VampState.Cfg().spawnZoneMinZ;
		if (AZ_VampLandmarks.s_Churches)
		{
			for (int ci = 0; ci < AZ_VampLandmarks.s_Churches.Count() && s_At.Count() < TOWN_COPIES; ci++)
			{
				Object ch = AZ_VampLandmarks.s_Churches[ci];
				if (ch && ch.GetPosition()[2] >= minZ)
					s_At.Insert(Ground(ch.GetPosition() + Vector(2.5, 0, 2.5)));
			}
		}
		if (AZ_VampDecor.s_Lanterns)
		{
			for (int li = 0; li < AZ_VampDecor.s_Lanterns.Count() && s_At.Count() < 2; li += 7)
			{
				Object lan = AZ_VampDecor.s_Lanterns[li];
				if (lan && lan.GetPosition()[2] >= minZ)
					s_At.Insert(Ground(lan.GetPosition() + Vector(0.9, 0, 0.9)));
			}
		}
		s_At.Insert(Ground(AZ_VampCryptA1.Crack() + Vector(-1.3, 0, -0.9)));
		for (int i = 0; i < s_At.Count(); i++)
			s_Map.Insert(null);
	}

	static bool Lies(vector at)
	{
		array<Object> near = new array<Object>;
		GetGame().GetObjectsAtPosition(at, 2.5, near, null);
		foreach (Object o : near)
		{
			if (o && o.GetType() == TYPE)
				return true;
		}
		return false;
	}

	static void Restock()
	{
		if (!s_At)
			return;
		int made = 0;
		for (int i = 0; i < s_At.Count(); i++)
		{
			if (s_Map[i])
				continue;
			if (Lies(s_At[i]))
				continue;
			EntityAI m = EntityAI.Cast(GetGame().CreateObjectEx(TYPE, s_At[i], ECE_KEEPHEIGHT));
			if (!m)
			{
				Print("[AZ_Vamp] old map: " + TYPE + " NULL at " + s_At[i] + " (class not packed?)");
				continue;
			}
			m.SetPosition(s_At[i]);
			m.SetOrientation(Vector(Math.RandomFloat(0, 360), 0, 0));
			m.SetLifetimeMax(LIFETIME);
			s_Map[i] = m;
			made++;
		}
		if (made > 0)
			Print("[AZ_Vamp] old map: " + made + " laid (of " + s_At.Count() + ")");
	}


	static void Tick()
	{
		Restock();
	}
}




