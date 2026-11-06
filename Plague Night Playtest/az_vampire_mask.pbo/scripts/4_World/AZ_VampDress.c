








class AZ_VampDress
{
	static const string MALES = "SurvivorM_Mirek,SurvivorM_Boris,SurvivorM_Cyril,SurvivorM_Denis,SurvivorM_Elias,SurvivorM_Francis,SurvivorM_Guo,SurvivorM_Hassan,SurvivorM_Indar,SurvivorM_Jose,SurvivorM_Kaito,SurvivorM_Lewis,SurvivorM_Manua,SurvivorM_Niki,SurvivorM_Peter,SurvivorM_Quinn,SurvivorM_Rolf,SurvivorM_Seth,SurvivorM_Taiki";
	static const string FEMALES = "SurvivorF_Linda,SurvivorF_Baty,SurvivorF_Eva,SurvivorF_Frida,SurvivorF_Gabi,SurvivorF_Helga,SurvivorF_Irena,SurvivorF_Judy,SurvivorF_Keiko,SurvivorF_Maria,SurvivorF_Naomi";

	static const string TOPS = "QuiltedJacket_Grey,QuiltedJacket_Black,M65Jacket_Black,WoolCoat_Black,WoolCoat_BrownCheck,WoolCoat_GreyCheck,Sweater_Gray,Sweater_Green,Hoodie_Grey,Shirt_PlaneBlack,Shirt_GreenCheck,Shirt_BlueCheck,RidersJacket_Black,BomberJacket_Black";
	static const string PANTS = "Jeans_Black,Jeans_Grey,Jeans_Brown,CargoPants_Grey,CargoPants_Black,CanvasPants_Grey,HunterPants_Brown,SlacksPants_Black,Breeches_Browncheck,TrackSuitPants_Black";
	static const string SHOES = "WorkingBoots_Brown,HikingBootsLow_Black,CombatBoots_Black,MilitaryBoots_Black,Sneakers_Gray,JoggingShoes_Black";
	static const string HATS = "BeanieHat_Grey,FlatCap_Grey";

	static const string DARK_PANTS = "SlacksPants_Black,Jeans_Black,CargoPants_Black";
	static const string BOOTS = "MilitaryBoots_Black,CombatBoots_Black,WorkingBoots_Brown,HikingBootsLow_Black";


	static array<string> List(string csv)
	{
		array<string> parts = new array<string>;
		csv.Split(",", parts);
		return parts;
	}


	static string Pick(string csv, int seed, int salt)
	{
		array<string> list = List(csv);
		int i = (seed * (7 + salt * 6) + salt * 3) % list.Count();
		if (i < 0)
			i += list.Count();
		return list[i];
	}

	static string Male(int seed)
	{
		return Pick(MALES, seed, 1);
	}

	static string Female(int seed)
	{
		return Pick(FEMALES, seed, 1);
	}




	static ref array<Object> s_Faces;
	static bool FaceTaken(string cls)
	{
		if (!s_Faces)
			s_Faces = new array<Object>;
		for (int fi = s_Faces.Count() - 1; fi >= 0; fi--)
		{
			EntityAI worn = EntityAI.Cast(s_Faces[fi]);
			if (!worn || !worn.IsAlive())
			{
				s_Faces.Remove(fi);
				continue;
			}
			if (worn.GetType() == cls)
				return true;
		}
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		foreach (Man pl : players)
		{
			if (pl && pl.GetType() == cls)
				return true;
		}
		return false;
	}

	static string Unique(string cls)
	{
		string csv = MALES;
		if (cls.IndexOf("SurvivorF_") == 0)
			csv = FEMALES;
		array<string> faces = List(csv);
		int start = faces.Find(cls);
		if (start < 0)
			return cls;
		for (int fj = 0; fj < faces.Count(); fj++)
		{
			int at = start + fj;
			if (at >= faces.Count())
				at -= faces.Count();
			if (!FaceTaken(faces[at]))
				return faces[at];
		}
		return cls;
	}

	static void Note(Object o)
	{
		if (!s_Faces)
			s_Faces = new array<Object>;
		if (o)
			s_Faces.Insert(o);
	}

	static string Any(int seed)
	{
		if (seed % 2 == 0)
			return Male(seed / 2);
		return Female(seed / 2);
	}

	static bool Wear(PlayerBase p, string cls)
	{
		if (p.GetInventory().CreateAttachment(cls))
			return true;
		Print("[AZ_Vamp] dress: " + cls + " refused on " + p.GetType());
		return false;
	}


	static string Worn(PlayerBase p)
	{
		string w = "";
		array<string> slots = {"Body", "Legs", "Feet", "Mask", "Headgear"};
		foreach (string sl : slots)
		{
			EntityAI it = p.FindAttachmentBySlotName(sl);
			string t = "-";
			if (it)
				t = it.GetType();
			w += " " + sl + "=" + t;
		}
		return w;
	}

	static void Survivor(PlayerBase p, int seed)
	{
		if (!p || !GetGame().IsServer())
			return;
		Wear(p, Pick(TOPS, seed, 0));
		Wear(p, Pick(PANTS, seed, 1));
		Wear(p, Pick(SHOES, seed, 2));
		if (seed % 3 == 1)
			Wear(p, Pick(HATS, seed, 3));
	}

	static void Elder(PlayerBase p, int seed)
	{
		if (!p || !GetGame().IsServer())
			return;
		string coat;
		if (p.IsMale())
		{
			coat = AZ_VampCostumes.GREATCOAT;
		}
		else
		{



			string gowns = AZ_VampCostumes.GRAVE_GOWN + "," + AZ_VampCostumes.VELVET_GOWN;
			coat = Pick(gowns, seed / 2, 0);
		}


		if (!Wear(p, coat))
		{
			array<string> backups = new array<string>;
			if (!p.IsMale())
			{
				backups.Insert(AZ_VampCostumes.VELVET_GOWN);
				backups.Insert(AZ_VampCostumes.GRAVE_GOWN);
			}
			foreach (string b : backups)
			{
				if (b != coat && Wear(p, b))
					break;
			}
		}


		if (p.IsMale())
			Wear(p, Pick(DARK_PANTS, seed, 1));
		Wear(p, Pick(BOOTS, seed, 2));
		Print("[AZ_Vamp] elder " + seed + " " + p.GetType() + " wears:" + Worn(p));
	}
}





class AZ_VampSpawn
{
	static vector Floor(vector at, float refY)
	{
		vector from = Vector(at[0], refY + 1.5, at[2]);
		vector to = Vector(at[0], refY - 3.0, at[2]);
		vector hit, normal;
		int comp;
		set<Object> results = new set<Object>;
		if (DayZPhysics.RaycastRV(from, to, hit, normal, comp, results, null, null, false, false, ObjIntersectGeom))
			return Vector(at[0], hit[1], at[2]);
		return Vector(at[0], refY, at[2]);
	}
}
