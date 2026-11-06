



modded class NasdaraData
{
	override bool WeatherOnBeforeChange(EWeatherPhenomenon type, float actual, float change, float time)
	{
		bool r = super.WeatherOnBeforeChange(type, actual, change, time);
		if (!AZ_VampEvent.Active())
			return r;
		m_Weather.GetFog().SetLimits(0.04, 0.22);
		m_Weather.GetOvercast().SetLimits(0.10, 1);
		if (type == EWeatherPhenomenon.OVERCAST)
		{

			m_Weather.GetFog().Set(Math.RandomFloatInclusive(0.06, 0.2), 600, 1500);
		}
		return r;
	}

	protected override void CalculateVolFog(float lerpValue, float windMagnitude, float changeTime)
	{
		super.CalculateVolFog(lerpValue, windMagnitude, changeTime);
		if (AZ_VampEvent.Active())
			m_Weather.SetDynVolFogDistanceDensity(0.012, changeTime);
	}
}
