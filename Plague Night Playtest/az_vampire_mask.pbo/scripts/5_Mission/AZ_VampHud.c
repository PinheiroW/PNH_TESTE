


modded class IngameHud
{
	bool m_AZVampHudHidden;

	override void Update(float timeslice)
	{
		super.Update(timeslice);
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		bool hide = p && p.m_AZVampire;
		if (hide == m_AZVampHudHidden || !m_Notifiers)
			return;
		m_AZVampHudHidden = hide;
		AZ_VampHudShow("IconThirsty", !hide);
		AZ_VampHudShow("IconHungry", !hide);
	}

	void AZ_VampHudShow(string name, bool show)
	{
		Widget w = m_Notifiers.FindAnyWidget(name);


		while (w && w.GetParent() && w.GetParent() != m_Notifiers)
		{
			Widget up = w.GetParent();
			if (up.FindAnyWidget("IconBlood") || up.FindAnyWidget("IconHealth"))
				break;
			w = up;
		}
		if (w)
		{
			w.Show(show);
			Print("[AZ_Vamp] hud " + name + " slot " + w.GetName() + " show " + show);
		}
	}
}
