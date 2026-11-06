


class AZ_VampRedLanternLight extends PointLightBase
{
	void AZ_VampRedLanternLight()
	{
		SetVisibleDuringDaylight(false);
		SetRadiusTo(5);
		SetBrightnessTo(2.6);
		SetCastShadow(false);
		SetDiffuseColor(1.0, 0.07, 0.03);
		SetAmbientColor(1.0, 0.07, 0.03);
		SetFlickerAmplitude(0.3);
		SetFlickerSpeed(0.75);
		FadeIn(1.0);
	}
}

class AZ_VampEyeLight extends PointLightBase
{
	static float s_Radius = 0.18;
	static float s_Bright = 0.9;

	void AZ_VampEyeLight()
	{
		SetVisibleDuringDaylight(false);
		SetRadiusTo(s_Radius);
		SetBrightnessTo(s_Bright);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetAmbientColor(0.9, 0.02, 0.0);
		SetDiffuseColor(1.0, 0.04, 0.02);
	}
}
