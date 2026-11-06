



modded class DynamicMusicPlayerRegistry
{
	static const string AZ_VAMP_MENU_SOUNDSET = "AZ_PlagueNight_Menu_SoundSet";

	override protected void PreloadData()
	{
		super.PreloadData();

		if (!m_TracksMenu || !g_Game || !g_Game.ConfigIsExisting("CfgSoundSets " + AZ_VAMP_MENU_SOUNDSET))
			return;

		m_TracksMenu.Clear();
		RegisterTrackMenu(AZ_VAMP_MENU_SOUNDSET, true);
	}
}
