

class AZ_VampAdminTp
{
	static bool Handles(string where)
	{
		return where == "a1" || where == "exit";
	}

	static string Go(PlayerBase p, string where)
	{
		if (!p)
			return "no player";
		vector at;
		vector look;
		if (where == "a1")
		{

			at = AZ_VampCryptA1.Crack() + Vector(-3, 0, 0);
			at[1] = GetGame().SurfaceY(at[0], at[2]) + 0.05;
			look = AZ_VampCryptA1.ORB;
		}
		else if (where == "exit")
		{

			array<vector> slots = AZ_VampCryptExit.VorkutaSlots();
			at = slots[1];
			look = at + AZ_VampCryptExit.VorkutaFwd() * 10;
		}
		else
			return "usage: /vamp tp ledge|altar|gate|a1|exit";
		vector d = look - at;
		d[1] = 0;
		AZ_VampSnap.Far(p, at, d.Normalized());
		Print("[AZ_Vamp] admin tp " + where + " " + AZ_VampState.Uid(p) + " to " + at);
		return "moved to " + where;
	}
}
