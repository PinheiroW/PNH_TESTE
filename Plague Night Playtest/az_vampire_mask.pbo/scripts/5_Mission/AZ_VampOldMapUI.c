


modded class MapMenu
{
	static const string AZ_INK_ICON = "\\DZ\\gear\\navigation\\data\\map_monument_ca.paa";


	void AZ_FrameOldMarks()
	{
		if (!m_MapWidgetInstance)
			return;
		vector a = AZ_VampOldMap.CrackAt();
		vector b = AZ_VampOldMap.DoorAt();
		m_MapWidgetInstance.SetMapPos((a + b) * 0.5);
		m_MapWidgetInstance.SetScale(Math.Clamp(vector.Distance(a, b) / 9000, 0.15, 1.0));
	}

	override void LoadMapMarkers()
	{
		super.LoadMapMarkers();
		if (!m_Map || !m_MapWidgetInstance || !AZ_VampOldMap.Cast(m_Map))
			return;
		int ink = ARGB(255, 92, 22, 18);
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		bool bloodMoon = player && player.m_AZBloodMoon;
		array<string> lines = AZ_VampOldMap.ClientLines();
		foreach (string line : lines)
		{
			array<string> f = new array<string>;
			line.Split("|", f);
			if (f.Count() < 5 || !AZ_VampOldMap.Shown(f[3], bloodMoon))
				continue;
			vector pos = Vector(f[0].ToFloat(), 0, f[2].ToFloat());
			m_MapWidgetInstance.AddUserMark(pos, f[4], ink, AZ_INK_ICON);
		}
	}
}
