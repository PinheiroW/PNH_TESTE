







modded class DayZPlayerCameraBase
{
	protected int m_AZCamNullLogged;

	protected bool AZ_HasWeaponsModifier()
	{
		if (m_CommandWeapons)
			return true;
		if (m_pPlayer)
			m_CommandWeapons = m_pPlayer.GetCommandModifier_Weapons();
		if (m_CommandWeapons)
			return true;
		if (m_AZCamNullLogged < 3)
		{
			m_AZCamNullLogged++;
			Print("[AZ_Vamp] camera guard: " + GetCameraName() + " has no weapons modifier, pitch update skipped (AZ-2290)");
		}
		return false;
	}

	override float UpdateUDAngleUnlocked(out float pAngle, out float pAngleAdd, float pMin, float pMax, float pDt)
	{
		if (!AZ_HasWeaponsModifier())
			return pAngle + pAngleAdd;
		return super.UpdateUDAngleUnlocked(pAngle, pAngleAdd, pMin, pMax, pDt);
	}

	override float UpdateUDAngle(out float pAngle, out float pAngleAdd, float pMin, float pMax, float pDt)
	{
		if (!AZ_HasWeaponsModifier())
			return pAngle + pAngleAdd;
		return super.UpdateUDAngle(pAngle, pAngleAdd, pMin, pMax, pDt);
	}
}
