


modded class LoadingScreen
{
	static const string AZ_VAMP_LOAD_LAYOUT = "azo/az_vampire_mask/gui/layouts/az_vamp_loading.layout";
	static const float  AZ_VAMP_TIP_SEC     = 7.0;

	protected Widget            m_AZVampRoot;
	protected ImageWidget       m_AZVampBg;
	protected Widget            m_AZVampShade;
	protected Widget            m_AZVampNote;
	protected TextWidget        m_AZVampTip;
	protected ProgressBarWidget m_AZVampBar;
	protected ref array<string> m_AZVampTips;
	protected int               m_AZVampTipIdx;
	protected float             m_AZVampTipNext;

	protected bool              m_AZVampBuilt;


	protected void AZVamp_Build()
	{
		AZ_VampClientLife.Mark("loading_skin_build_enter");
		m_AZVampBuilt = true;

		m_AZVampTips = new array<string>;
		m_AZVampTips.Insert("Vampires burn in the sun. Hide before dawn.");
		m_AZVampTips.Insert("A blood transfusion cures the bitten.");
		m_AZVampTips.Insert("Holy water from a graveyard washes out a bite.");
		m_AZVampTips.Insert("Fists and blades do nothing to a vampire. Carve a stake from a long stick or firewood.");
		m_AZVampTips.Insert("Hungry vampires hear your heartbeat.");
		m_AZVampTips.Insert("Press your Fangs key ({AZ_FANGS}) to bare or hide your fangs.");
		m_AZVampTips.Insert("Red eyes and fangs: that one is hungry.");
		m_AZVampTips.Insert("Bitten? Find a blood bag before the fever takes you.");
		m_AZVampTips.Insert("Vampires cannot keep human food down.");
		m_AZVampTips.Insert("Headshots can leave a vampire down at night. Finish them with a stake. Village fires burn vampires.");
		m_AZVampTips.Insert("Questions, bugs or a hunt to brag about? azaleamap.com/discord");
		m_AZVampTipIdx = Math.RandomInt(0, m_AZVampTips.Count());

		if (!m_DayZGame || !m_WidgetRoot)
		{
			AZ_VampClientLife.Mark("loading_skin_build_skipped");
			return;
		}
		m_AZVampRoot = m_DayZGame.GetLoadingWorkspace().CreateWidgets(AZ_VAMP_LOAD_LAYOUT, m_WidgetRoot);
		if (!m_AZVampRoot)
		{
			AZ_VampClientLife.Mark("loading_skin_layout_absent");
			return;
		}

		m_AZVampBg    = ImageWidget.Cast(m_AZVampRoot.FindAnyWidget("AZ_VampLoadBg"));
		m_AZVampShade = m_AZVampRoot.FindAnyWidget("AZ_VampLoadShade");
		m_AZVampNote  = m_AZVampRoot.FindAnyWidget("AZ_VampLoadNote");
		m_AZVampTip   = TextWidget.Cast(m_AZVampRoot.FindAnyWidget("AZ_VampLoadTip"));
		m_AZVampBar   = ProgressBarWidget.Cast(m_AZVampRoot.FindAnyWidget("AZ_VampLoadBar"));
		AZ_VampClientLife.Mark("loading_skin_build_exit", "barPresent=" + (m_AZVampBar != null));

	}



	protected void AZVamp_SetArt()
	{
		bool artOk = AZ_VampArt.LoadRandom(m_AZVampBg);
		if (m_AZVampBg)
			m_AZVampBg.Show(artOk);
		if (m_AZVampShade)
			m_AZVampShade.Show(artOk);
		if (m_AZVampNote)
			m_AZVampNote.Show(false);		TextWidget credit = TextWidget.Cast(m_AZVampRoot.FindAnyWidget("AZ_VampLoadCredit"));
		if (credit)
		{
			string who = AZ_VampArt.Credit(AZ_VampArt.s_LastPick);
			if (who != "")
				credit.SetText("Plague Night by OlipollyZ   |   Screenshot by " + who);
			else
				credit.SetText("Plague Night by OlipollyZ");
		}
	}

	override void Show()
	{
		AZ_VampClientLife.Mark("loading_skin_show_enter");
		super.Show();
		if (!m_AZVampBuilt)
			AZVamp_Build();
		AZVamp_Apply();
		AZ_VampClientLife.Mark("loading_skin_show_exit");
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		AZVamp_TickTip();
	}


	override void SetProgress(float val)
	{
		super.SetProgress(val);
		AZVamp_TickTip();
	}


	protected void AZVamp_Apply()
	{
		if (!m_AZVampRoot)
			return;

		if (m_ImageLogoMid)
			m_ImageLogoMid.Show(false);
		if (m_ImageLogoCorner)
			m_ImageLogoCorner.Show(false);
		if (m_ModdedWarning)
			m_ModdedWarning.Show(false);
		if (m_ProgressLoading)
			m_ProgressLoading.Show(false);

		Widget hintFrame = m_WidgetRoot.FindAnyWidget("hint_frame");
		if (hintFrame)
			hintFrame.Show(false);
		Widget hintIcon = m_WidgetRoot.FindAnyWidget("hintIcon");
		if (hintIcon)
			hintIcon.Show(false);

		m_AZVampRoot.Show(true);
		AZVamp_SetArt();
		if (m_AZVampBar)
		{
			m_AZVampBar.SetCurrent(0.0);
			ProgressAsync.SetProgressData(m_AZVampBar);
		}

		AZVamp_NextTip();
	}

	protected void AZVamp_TickTip()
	{
		if (!m_AZVampTip || !m_DayZGame)
			return;
		if (m_DayZGame.GetTickTime() >= m_AZVampTipNext)
			AZVamp_NextTip();
	}

	protected void AZVamp_NextTip()
	{
		if (!m_AZVampTip || !m_AZVampTips || m_AZVampTips.Count() == 0)
			return;

		m_AZVampTipIdx = (m_AZVampTipIdx + 1) % m_AZVampTips.Count();
		m_AZVampTip.SetText(AZ_VampMessageKey.Resolve(m_AZVampTips[m_AZVampTipIdx]));
		if (m_DayZGame)
			m_AZVampTipNext = m_DayZGame.GetTickTime() + AZ_VAMP_TIP_SEC;
	}
}
