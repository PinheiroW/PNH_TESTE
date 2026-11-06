


class AZ_VampLoginSkin
{
	static const string LAYOUT = "azo/az_vampire_mask/gui/layouts/az_vamp_login.layout";

	static void Apply(Widget layoutRoot)
	{
		AZ_VampClientLife.Mark("login_skin_apply_enter", "rootPresent=" + (layoutRoot != null));
		if (!layoutRoot || !g_Game)
		{
			AZ_VampClientLife.Mark("login_skin_apply_skipped");
			return;
		}


		Widget vanillaBg = layoutRoot.FindAnyWidget("Background");
		if (vanillaBg)
		{
			Widget skin = g_Game.GetWorkspace().CreateWidgets(LAYOUT, vanillaBg);
			if (skin)
			{
				ImageWidget art = ImageWidget.Cast(skin.FindAnyWidget("AZ_VampLoginBg"));
				bool artOk = false;
				if (art)
					artOk = AZ_VampArt.LoadRandom(art);
				if (art && !artOk)
					art.Show(false);
				Widget note = skin.FindAnyWidget("AZ_VampLoginNote");
				if (note)
					note.Show(artOk);
			}
		}


		Widget dialog = layoutRoot.FindAnyWidget("TextInputDialog");
		if (dialog)
			dialog.SetColor(ARGB(235, 22, 6, 8));
		Widget leave = layoutRoot.FindAnyWidget("btnLeave");
		if (leave)
			leave.SetColor(ARGB(255, 140, 10, 12));
		AZ_VampClientLife.Mark("login_skin_apply_exit");
	}
}

modded class LoginTimeBase
{
	override Widget Init()
	{
		Widget root = super.Init();
		AZ_VampLoginSkin.Apply(root);
		return root;
	}
}

modded class LoginQueueBase
{
	override Widget Init()
	{
		Widget root = super.Init();
		AZ_VampLoginSkin.Apply(root);
		return root;
	}
}


modded class UiHintPanelLoading
{
	static const string AZ_VAMP_HINTS = "azo/az_vampire_mask/data/az_vamp_hints.json";

	override protected void LoadContentList()
	{
		array<ref HintPage> ours;
		string errorMessage;
		if (JsonFileLoader<array<ref HintPage>>.LoadFile(AZ_VAMP_HINTS, ours, errorMessage) && ours && ours.Count() > 1)
		{
			m_ContentList = ours;
			return;
		}
		super.LoadContentList();
	}
}
