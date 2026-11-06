modded class InGameMenu
{
	override void OnShow()
	{
		super.OnShow();
		AZ_VampShowDiscordLink();
	}

	override void OnHide()
	{
		AZ_VampHideDiscordLink();
		super.OnHide();
	}

	// Widget-only overlay; never access the menu scene or retain widget references.
	protected void AZ_VampShowDiscordLink()
	{
		if (!layoutRoot || !GetGame() || !GetGame().GetWorkspace())
			return;
		if (layoutRoot.FindAnyWidget("AZ_VampDiscordPanel"))
			return;
		Widget panel = GetGame().GetWorkspace().CreateWidgets("azo/az_vampire_mask/gui/layouts/az_vamp_discord_link.layout", layoutRoot);
		if (!panel)
			return;
	}

	protected void AZ_VampHideDiscordLink()
	{
		if (!layoutRoot)
			return;
		Widget panel = layoutRoot.FindAnyWidget("AZ_VampDiscordPanel");
		if (panel)
			panel.Unlink();
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w && layoutRoot && button == MouseState.LEFT)
		{
			Widget discordButton = layoutRoot.FindAnyWidget("AZ_VampDiscordButton");
			if (discordButton && w == discordButton && GetGame())
			{
				GetGame().OpenURL("https://azaleamap.com/discord");
				return true;
			}
		}
		return super.OnClick(w, x, y, button);
	}
}
