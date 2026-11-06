




class AZ_VampSurvey
{
	static const float STEP = 40;
	static const int ROWS_PER_TICK = 6;

	static int s_N;
	static int s_Row;
	static FileHandle s_H;
	static FileHandle s_S;

	static void Start()
	{
		if (!GetGame().IsServer() || !FileExist(AZ_VampState.DIR + "/survey.txt"))
			return;
		MakeDirectory(AZ_VampState.DIR + "/survey");
		s_N = Math.Round(GetGame().GetWorld().GetWorldSize() / STEP);
		s_Row = 0;
		s_H = OpenFile(AZ_VampState.DIR + "/survey/height.txt", FileMode.WRITE);
		s_S = OpenFile(AZ_VampState.DIR + "/survey/sea.txt", FileMode.WRITE);
		if (!s_H || !s_S)
		{
			Print("[AZ_Vamp] survey: cannot open the output files");
			return;
		}
		FPrintln(s_H, "" + s_N + " " + s_N + " " + GetGame().GetWorld().GetWorldSize() + " " + STEP);
		FPrintln(s_S, "" + s_N + " " + s_N + " " + GetGame().GetWorld().GetWorldSize() + " " + STEP);
		Print("[AZ_Vamp] survey: " + s_N + " x " + s_N + " cells of " + STEP + " m");
		Tick();
	}

	static void Anchors()
	{
		FileHandle a = OpenFile(AZ_VampState.DIR + "/survey/anchors.txt", FileMode.WRITE);
		if (!a)
			return;
		array<ref AZ_VampMapMark> list = new array<ref AZ_VampMapMark>;
		list.Insert(AZ_VampOldMap.Mark("a1_crack", "a1_crack", "x", "always"));
		list.Insert(AZ_VampOldMap.Mark("tomb_mouth", "tomb_mouth", "x", "always"));
		list.Insert(AZ_VampOldMap.Mark("a1_orb", "a1_orb", "x", "always"));
		list.Insert(AZ_VampOldMap.Mark("area_a2", "area:Area_A2", "x", "always"));
		list.Insert(AZ_VampOldMap.Mark("area_ari", "area:Area_ARI", "x", "always"));
		list.Insert(AZ_VampOldMap.Mark("church_0", "church:0", "x", "always"));
		foreach (AZ_VampMapMark m : list)
		{
			vector p;
			if (AZ_VampOldMap.Resolve(m, p))
				FPrintln(a, m.key + " " + Math.Round(p[0]) + " " + Math.Round(p[2]));
			else
				FPrintln(a, m.key + " missing");
		}
		CloseFile(a);
	}

	static void Tick()
	{
		for (int r = 0; r < ROWS_PER_TICK && s_Row < s_N; r++)
		{
			float z = (s_Row + 0.5) * STEP;
			string hl = "";
			string sl = "";
			for (int c = 0; c < s_N; c++)
			{
				float x = (c + 0.5) * STEP;
				int h = Math.Round(GetGame().SurfaceY(x, z) * 10);
				hl = hl + h + " ";
				if (GetGame().SurfaceIsSea(x, z))
					sl = sl + "1";
				else
					sl = sl + "0";
			}
			FPrintln(s_H, hl);
			FPrintln(s_S, sl);
			s_Row++;
		}
		if (s_Row >= s_N)
		{
			CloseFile(s_H);
			CloseFile(s_S);
			Print("[AZ_Vamp] survey: done, " + s_N + " rows written");
			Anchors();
			return;
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Tick, 20, false);
	}
}
