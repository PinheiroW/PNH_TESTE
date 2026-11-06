




class AZ_TzFillLight extends PointLightBase
{
	void AZ_TzFillLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(9.0);
		SetBrightnessTo(1.6);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetAmbientColor(0.9, 0.55, 0.3);
		SetDiffuseColor(1.0, 0.6, 0.35);
	}
}



class AZ_TzKeyLight extends PointLightBase
{
	void AZ_TzKeyLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(4.0);
		SetBrightnessTo(2.4);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetAmbientColor(1.0, 0.6, 0.35);
		SetDiffuseColor(1.0, 0.66, 0.4);
	}
}



class AZ_TzTorchLight extends PointLightBase
{
	void AZ_TzTorchLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(11.0);
		SetBrightnessTo(3.2);
		SetCastShadow(true);
		SetFlareVisible(false);
		SetAmbientColor(1.0, 0.52, 0.22);
		SetDiffuseColor(1.0, 0.58, 0.26);
		SetFlickerSpeed(7.0);
		SetFlickerAmplitude(0.35);
	}
}




class AZ_TzMoonKeyLight extends PointLightBase
{
	void AZ_TzMoonKeyLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(4.5);
		SetBrightnessTo(2.8);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetAmbientColor(0.7, 0.8, 1.0);
		SetDiffuseColor(0.78, 0.86, 1.0);
	}
}


class AZ_TzCoolRimLight extends PointLightBase
{
	void AZ_TzCoolRimLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(2.2);
		SetBrightnessTo(5.5);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetAmbientColor(0.45, 0.6, 0.95);
		SetDiffuseColor(0.55, 0.7, 1.0);
	}
}

class AZ_TzRimLight extends PointLightBase
{
	void AZ_TzRimLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(1.6);
		SetBrightnessTo(6.0);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetAmbientColor(0.9, 0.05, 0.03);
		SetDiffuseColor(1.0, 0.08, 0.05);
	}
}


class AZ_TzFlareLight extends PointLightBase
{
	void AZ_TzFlareLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(10.0);
		SetBrightnessTo(3.5);
		SetCastShadow(true);
		SetFlareVisible(false);
		SetAmbientColor(1.0, 0.12, 0.06);
		SetDiffuseColor(1.0, 0.15, 0.08);
	}
}



class AZ_VampFaceLight extends PointLightBase
{
	void AZ_VampFaceLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(1.8);
		SetBrightnessTo(2.2);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetAmbientColor(1.0, 0.9, 0.8);
		SetDiffuseColor(1.0, 0.92, 0.82);
	}
}
