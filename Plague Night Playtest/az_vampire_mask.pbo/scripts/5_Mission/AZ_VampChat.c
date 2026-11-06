

modded class ChatInputMenu
{
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (finished && m_edit_box)
		{
			string text = m_edit_box.GetText();
			if (text.Trim() == "/vamp sundebug")
			{
				AZ_VampSunDebug.Toggle();
				m_edit_box.SetText("");
			}
			else if (text == "/vamp" || text.IndexOf("/vamp ") == 0)
			{
				PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
				if (text.Trim() == "/vamp dimtest")
					AZ_VampVictimView.s_TestUntil = GetGame().GetTime() + 30000;	// the server knocks an admin out; this side only changes the view
				if (p)
					GetGame().RPCSingleParam(p, AZ_VampEvent.RPC_CMD, new Param1<string>(text), true);
				m_edit_box.SetText("");
			}
		}
		return super.OnChange(w, x, y, finished);
	}
}

// AZ-2597: ChatMP arrives with the recipient's Fangs binding still unresolved.
modded class Chat
{
	override void Add(ChatMessageEventParams params)
	{
		if (params && params.param3.IndexOf("{AZ_FANGS}") != -1)
		{
			ref ChatMessageEventParams resolved = new ChatMessageEventParams(params.param1, params.param2, AZ_VampMessageKey.Resolve(params.param3), params.param4);
			super.Add(resolved);
			return;
		}
		super.Add(params);
	}
}
