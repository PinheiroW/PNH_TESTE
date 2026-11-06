class AZ_VampMessageKey
{
	static string Resolve(string text)
	{
		if (text.IndexOf("{AZ_FANGS}") >= 0)
			text.Replace("{AZ_FANGS}", Binding("UAAZVampFangs"));
		if (text.IndexOf("{AZ_TIE}") >= 0)
			text.Replace("{AZ_TIE}", Binding("UAAZVampTie"));
		return text;
	}

	static string Binding(string inputName)
	{
		string binding = "unbound";
		if (GetUApi() && GetUApi().GetInputByName(inputName))
		{
			map<int,ref TStringArray> names = InputUtils.GetComboButtonNamesFromInput(inputName, EInputDeviceType.MOUSE_AND_KEYBOARD);
			if (names && names.Count() > 0)
			{
				TStringArray buttons = names.GetElement(0);
				if (buttons && buttons.Count() > 0)
				{
					binding = buttons[0];
					for (int i = 1; i < buttons.Count(); i++)
						binding += " + " + buttons[i];
				}
			}
		}
		return binding;
	}
}

modded class NotificationRuntimeData
{
	override string GetDetailText()
	{
		return AZ_VampMessageKey.Resolve(super.GetDetailText());
	}
}
