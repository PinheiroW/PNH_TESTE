



modded class IngameHud
{
	static const string AZ_MOON_LAYOUT = "azo/az_vampire_mask/gui/layouts/az_vamp_moon.layout";
	Widget m_AZMoon;
	bool m_AZMoonShown;
	string m_AZMoonLogState;

	override void Update(float timeslice)
	{
		super.Update(timeslice);
		AZ_MoonUpdate();
	}

	void AZ_MoonUpdate()
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());


		bool day = AZ_VampSun.SunUp();
		bool show = p && p.IsAlive() && (p.m_AZVampire || (p.m_AZBloodMoon && !day));
		if (!m_AZMoon)
		{
			if (!show || !m_HudPanelWidget)
			{
				AZ_MoonLog(day, false, "unavailable");
				return;
			}
			Widget row = m_HudPanelWidget.FindAnyWidget("BadgesPanel");
			if (!row)
			{
				AZ_MoonLog(day, false, "unavailable");
				return;
			}
			m_AZMoon = GetGame().GetWorkspace().CreateWidgets(AZ_MOON_LAYOUT, row);
			if (!m_AZMoon)
			{
				AZ_MoonLog(day, false, "unavailable");
				return;
			}
			ImageWidget made = ImageWidget.Cast(m_AZMoon.FindAnyWidget("AZ_VampMoonIcon"));
			if (made)
			{
				made.LoadImageFile(0, "azo/az_vampire_mask/gui/textures/az_vamp_fullmoon_ca.paa");
				made.LoadImageFile(1, "azo/az_vampire_mask/gui/textures/az_vamp_sun_ca.paa");
			}
			ImageWidget fillImage = ImageWidget.Cast(m_AZMoon.FindAnyWidget("AZ_VampMoonFill"));
			if (fillImage)
				fillImage.LoadImageFile(0, "azo/az_vampire_mask/gui/textures/az_vamp_fullmoon_ca.paa");
			Print("[AZ_Vamp] hud blood moon badge created");
		}
		if (show != m_AZMoonShown)
		{
			m_AZMoonShown = show;
			m_AZMoon.Show(show);
			WrapSpacerWidget spacer = WrapSpacerWidget.Cast(m_AZMoon.GetParent());
			if (spacer)
				spacer.Update();
		}
		if (!show)
		{
			AZ_MoonLog(day, false, "hidden");
			return;
		}
		ImageWidget icon = ImageWidget.Cast(m_AZMoon.FindAnyWidget("AZ_VampMoonIcon"));
		if (icon)
		{
			if (day)
			{
				icon.SetImage(1);
				icon.SetColor(ARGB(255, 255, 255, 255));
			}
			else
			{
				icon.SetImage(0);
				if (p.m_AZBloodMoon)
					icon.SetColor(ARGB(255, 204, 41, 41));
				else
					icon.SetColor(ARGB(255, 225, 225, 235));
			}
		}
		AZ_MoonFill(p, day);
		string iconState = "missing";
		if (icon)
		{
			iconState = "moon";
			if (day)
				iconState = "sun";
			else if (p.m_AZBloodMoon)
				iconState = "bloodmoon";
		}
		AZ_MoonLog(day, m_AZMoon.IsVisibleHierarchy(), iconState);
		float a = 1;
		if (p.m_AZVampire)
		{
			int now = GetGame().GetTime();
			float left = p.m_AZJReadyAt - now;
			if (p.m_AZJCoolMs > 0 && left > 0)
				a = 0.2 + 0.5 * (1 - left / p.m_AZJCoolMs);
			else
			{
				float since = now - p.m_AZJReadyAt;
				if (p.m_AZJCoolMs > 0 && since < 1500)
					a = 0.75 + 0.25 * Math.AbsFloat(Math.Sin(since / 1500.0 * Math.PI * 3));
			}
		}

		int pulseAge = GetGame().GetTime() - AZ_VampMoonCue.s_PulseAt;
		if (!day && p.m_AZBloodMoon && pulseAge >= 0 && pulseAge < 1200)
			a *= 1 - 0.35 * Math.Sin(pulseAge / 1200.0 * Math.PI);
		m_AZMoon.SetAlpha(a);
	}
	void AZ_MoonFill(PlayerBase p, bool day)
	{
		Widget clip = m_AZMoon.FindAnyWidget("AZ_VampMoonFillClip");
		Widget fill = m_AZMoon.FindAnyWidget("AZ_VampMoonFill");
		Widget icon = m_AZMoon.FindAnyWidget("AZ_VampMoonIcon");
		if (!clip || !fill || !icon)
			return;
		if (day || p.m_AZBloodMoon)
		{
			clip.Show(false);
			return;
		}
		float fraction = 0;
		if (p.m_AZMoonEligible)
			fraction = Math.Clamp(p.m_AZMoonProgress / 1000.0, 0, 1);


		float iconX, iconY, iconW, iconH;
		float rootX, rootY;
		icon.GetScreenPos(iconX, iconY);
		icon.GetScreenSize(iconW, iconH);
		m_AZMoon.GetScreenPos(rootX, rootY);
		float top = iconH * (1 - fraction);
		clip.SetFlags(WidgetFlags.CLIPCHILDREN);
		clip.SetPos(iconX - rootX, iconY - rootY + top);
		clip.SetSize(iconW, iconH * fraction);
		fill.SetSize(iconW, iconH);
		fill.SetPos(0, -top);
		fill.SetColor(ARGB(255, 204, 41, 41));
		clip.Show(fraction > 0);
	}

	void AZ_MoonLog(bool day, bool visible, string iconState)
	{
		string state = "day=" + day + " visible=" + visible + " icon=" + iconState;
		if (state == m_AZMoonLogState)
			return;
		m_AZMoonLogState = state;
		Print("[AZ_VampMoonClient] HUD " + state + " upMs=" + GetGame().GetTime());
	}

}
