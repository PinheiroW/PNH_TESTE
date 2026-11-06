

class AZ_VampEarWidget extends Managed
{
	protected ImageWidget m_icon;
	protected float m_elapsed;
	protected bool m_active;
	static const float HOLD_TIME = 1.5;
	static const float FADE_TIME = 0.8;
	static const float TICK_INTERVAL = 0.02;

	void AZ_VampEarWidget()
	{
		m_icon = ImageWidget.Cast(GetGame().GetWorkspace().CreateWidgets("azo/az_vampire_mask/gui/layouts/az_vamp_ear.layout"));
		if (m_icon)
			m_icon.Show(false);
	}

	void ~AZ_VampEarWidget()
	{
		if (m_active && GetGame() && GetGame().GetCallQueue(CALL_CATEGORY_GUI))
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.Tick);
		if (m_icon)
			m_icon.Unlink();
	}

	void SetIcon(string imagePath)
	{
		if (!m_icon)
			return;
		m_icon.LoadImageFile(0, imagePath);
		m_icon.Show(true);
		m_icon.SetAlpha(1.0);
		m_elapsed = 0;
		if (!m_active)
		{
			m_active = true;
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.Tick, 20, true);
		}
	}

	void Tick()
	{
		m_elapsed += TICK_INTERVAL;
		if (m_elapsed < HOLD_TIME)
			return;
		float alpha = 1.0 - ((m_elapsed - HOLD_TIME) / FADE_TIME);
		if (alpha <= 0)
		{
			m_icon.Show(false);
			m_icon.SetAlpha(0);
			m_active = false;
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.Tick);
			return;
		}
		m_icon.SetAlpha(alpha);
	}
};

class AZ_VampReportMenu extends UIScriptedMenu
{
	protected static const string LAYOUT = "azo/az_vampire_mask/gui/layouts/az_vamp_report.layout";
	protected static const int COOLDOWN_MS = 20000;
	protected static int s_LastSent = -20000;

	protected ref array<ButtonWidget> m_KindButtons = new array<ButtonWidget>;
	protected EditBoxWidget m_Text;
	protected TextWidget m_Hint;
	protected TextWidget m_Status;
	protected ButtonWidget m_Send;
	protected ButtonWidget m_Cancel;
	protected int m_Kind = AZ_VampQoL.KIND_BUG;

	override Widget Init()
	{
		AZ_VampClientLife.Mark("report_init_enter");
		layoutRoot = GetGame().GetWorkspace().CreateWidgets(LAYOUT);
		AZ_VampClientLife.Mark("report_layout_created", "rootPresent=" + (layoutRoot != null));
		for (int i = 0; i < AZ_VampQoL.KIND_COUNT; i++)
			m_KindButtons.Insert(ButtonWidget.Cast(layoutRoot.FindAnyWidget("Kind" + i)));
		m_Text = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("ReportText"));
		m_Hint = TextWidget.Cast(layoutRoot.FindAnyWidget("Hint"));
		m_Status = TextWidget.Cast(layoutRoot.FindAnyWidget("Status"));
		m_Send = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonSend"));
		m_Cancel = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonCancel"));
		SelectKind(AZ_VampQoL.KIND_BUG);
		AZ_VampClientLife.Mark("report_init_exit");
		return layoutRoot;
	}

	override void OnShow()
	{
		AZ_VampClientLife.Mark("report_show_enter");
		super.OnShow();
		if (m_Text)
		{
			AZ_VampClientLife.Mark("report_text_focus_request");
			SetFocus(m_Text);
		}
		AZ_VampClientLife.Mark("report_show_exit");
	}

	override void OnHide()
	{
		AZ_VampClientLife.Mark("report_hide_enter");
		super.OnHide();
		AZ_VampClientLife.Mark("report_hide_exit");
	}

	override bool OnFocus(Widget w, int x, int y)
	{
		AZ_VampClientLife.Mark("report_widget_focus", "textField=" + (w == m_Text));
		return super.OnFocus(w, x, y);
	}

	override bool OnFocusLost(Widget w, int x, int y)
	{
		AZ_VampClientLife.Mark("report_widget_focus_lost", "textField=" + (w == m_Text));
		return super.OnFocusLost(w, x, y);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		int kind = m_KindButtons.Find(ButtonWidget.Cast(w));
		if (kind >= 0)
		{
			SelectKind(kind);
			return true;
		}
		if (w == m_Send)
		{
			SendReport();
			return true;
		}
		if (w == m_Cancel)
		{
			Close();
			return true;
		}
		return false;
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);
		if (GetUApi().GetInputByID(UAUIBack).LocalPress())
			Close();
	}

	protected void SelectKind(int kind)
	{
		m_Kind = kind;
		for (int i = 0; i < m_KindButtons.Count(); i++)
		{
			ButtonWidget b = m_KindButtons[i];
			if (!b)
				continue;
			if (i == kind)
				b.SetText("[ " + AZ_VampQoL.KindLabel(i) + " ]");
			else
				b.SetText(AZ_VampQoL.KindLabel(i));
		}
		if (!m_Hint)
			return;
		switch (kind)
		{
			case AZ_VampQoL.KIND_BUG: m_Hint.SetText("What broke? Your position is added for you."); break;
			case AZ_VampQoL.KIND_IDEA: m_Hint.SetText("What would make Plague Night better?"); break;
			case AZ_VampQoL.KIND_FEEDBACK: m_Hint.SetText("How is the night going? The team reads this."); break;
		}
	}

	protected void SendReport()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player || !m_Text)
			return;
		string text = m_Text.GetText().Trim();
		if (text.LengthUtf8() < AZ_VampQoL.TEXT_MIN)
		{
			SetStatus("Write a few words first.");
			return;
		}
		if (text.LengthUtf8() > AZ_VampQoL.TEXT_MAX)
		{
			SetStatus("Keep it under " + AZ_VampQoL.TEXT_MAX + " characters.");
			return;
		}
		int now = GetGame().GetTime();
		if (now - s_LastSent < COOLDOWN_MS)
		{
			SetStatus("Wait a few seconds before sending another.");
			return;
		}
		s_LastSent = now;
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(m_Kind);
		rpc.Write(text);
		rpc.Send(player, AZ_VampQoL.RPC_REPORT, true, null);
		Close();
	}

	protected void SetStatus(string text)
	{
		if (m_Status)
			m_Status.SetText(text);
	}
};

modded class MissionGameplay
{

	protected float m_AZVampEarLevels[4] = {1.0, 0.5, 0.15, 0.0};
	protected string m_AZVampEarIcons[4] = {
		"azo/az_vampire_mask/gui/qol/az_vamp_ear_full.paa",
		"azo/az_vampire_mask/gui/qol/az_vamp_ear_mid.paa",
		"azo/az_vampire_mask/gui/qol/az_vamp_ear_low.paa",
		"azo/az_vampire_mask/gui/qol/az_vamp_ear_off.paa"
	};
	protected int m_AZVampEarTier;
	protected ref AZ_VampEarWidget m_AZVampEarWidget;

	protected void AZVampEarCycle()
	{
		if (!m_AZVampEarWidget)
			m_AZVampEarWidget = new AZ_VampEarWidget();
		m_AZVampEarTier = (m_AZVampEarTier + 1) % 4;
		GetGame().GetSoundScene().SetSoundVolume(m_AZVampEarLevels[m_AZVampEarTier], 1);
		m_AZVampEarWidget.SetIcon(m_AZVampEarIcons[m_AZVampEarTier]);
	}

	protected void AZVampOpenReport()
	{
		AZ_VampReportMenu menu = new AZ_VampReportMenu();
		menu.SetID(AZ_VampQoL.MENU_REPORT);
		GetGame().GetUIManager().ShowScriptedMenu(menu, null);
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);

		if (GetGame().GetUIManager().GetMenu() == NULL)
		{
			UAInput ear = GetUApi().GetInputByName("UAAZVampEarCycle");
			if (ear && ear.LocalPress())
				AZVampEarCycle();
			UAInput rep = GetUApi().GetInputByName("UAAZVampReport");
			if (rep && rep.LocalPress())
				AZVampOpenReport();
		}
	}
};
