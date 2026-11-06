


class AZ_VampArt
{
	static const string DIR      = "azo/az_vampire_mask/gui/loading/";
	static const string ORIGINAL = "az_pn_ig_02_co.paa";
	static string s_LastPick;

	// Who took each player screenshot (shown on the loading screen).
	static string Credit(string file)
	{
		if (file == "az_pn_ss_02_co.paa" || file == "az_pn_ss_07_co.paa") return "Pensey";
		if (file == "az_pn_ss_11_co.paa") return "Sir Chaka";
		if (file == "az_pn_ss_16_co.paa") return "Ali";
		if (file == "az_pn_ss_17_co.paa") return "its_emma";
		return "";
	}



	static void List(out array<string> files)
	{
		files = new array<string>;
		files.Insert(ORIGINAL);
		files.Insert("az_pn_ig_04_co.paa");
		files.Insert("az_pn_ig_07_co.paa");
		files.Insert("az_pn_ig_08_co.paa");
		files.Insert("az_pn_ss_02_co.paa");
		files.Insert("az_pn_ss_07_co.paa");
		files.Insert("az_pn_ss_11_co.paa");
		files.Insert("az_pn_ss_16_co.paa");
		files.Insert("az_pn_ss_17_co.paa");
	}


	static bool LoadRandom(ImageWidget w)
	{
		AZ_VampClientLife.Mark("art_load_enter", "widgetPresent=" + (w != null));
		if (!w)
		{
			AZ_VampClientLife.Mark("art_load_skipped");
			return false;
		}
		array<string> files;
		List(files);
		string pick = files.GetRandomElement();
		bool loaded = w.LoadImageFile(0, DIR + pick);
		if (!loaded && pick != ORIGINAL)
		{
			pick = ORIGINAL;
			loaded = w.LoadImageFile(0, DIR + ORIGINAL);
		}
		s_LastPick = pick;
		if (loaded)
		{
			w.SetImage(0);
			AZ_VampClientLife.Mark("art_load_exit", "loaded=true");
			return true;
		}
		AZ_VampClientLife.Mark("art_load_exit", "loaded=false");
		return false;
	}
}
