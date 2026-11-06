class HumansSettings
{
 float MeleeSpawnChancePercent = 75;
 int HumansReanimate=1;
 float ReanimationDelaySeconds=60;
 float ArmedAimAccuracy=5;
 float ArmedFireRange=40;
 float ArmedShotIntervalMin=1.1;
 float ArmedShotIntervalMax=1.8;
 float ArmedReloadSeconds=3.5;
 float ArmedBandageSeconds=7;
 float ArmedBandageHeal=25;
 static ref HumansSettings Instance;
 static HumansSettings Get()
 {
  if (Instance) return Instance;
  Instance = new HumansSettings;
  if (!GetGame().IsServer() && GetGame().IsMultiplayer()) return Instance;
  string directory = "$profile:Humans";
  string path = directory + "/Settings.json";
  string error;
  if (FileExist(path))
  {
   HumansSettings loaded;
   if (JsonFileLoader<HumansSettings>.LoadFile(path, loaded, error) && loaded) Instance = loaded;
   else Print("[Humans] Invalid Settings.json; using 75 percent default. " + error);
  }
  else
  {
   MakeDirectory(directory);
   if (!JsonFileLoader<HumansSettings>.SaveFile(path, Instance, error)) Print("[Humans] Cannot save Settings.json: " + error);
  }
  Instance.MeleeSpawnChancePercent = Math.Clamp(Instance.MeleeSpawnChancePercent, 0, 100);
  Instance.ReanimationDelaySeconds=Math.Clamp(Instance.ReanimationDelaySeconds,1,3600);
  Instance.ArmedAimAccuracy=Math.Clamp(Instance.ArmedAimAccuracy,0,10);
  Instance.ArmedFireRange=Math.Clamp(Instance.ArmedFireRange,5,80);
  Instance.ArmedShotIntervalMin=Math.Clamp(Instance.ArmedShotIntervalMin,0.6,10);
  Instance.ArmedShotIntervalMax=Math.Clamp(Instance.ArmedShotIntervalMax,Instance.ArmedShotIntervalMin,15);
  Instance.ArmedReloadSeconds=Math.Clamp(Instance.ArmedReloadSeconds,2,15);
  Instance.ArmedBandageSeconds=Math.Clamp(Instance.ArmedBandageSeconds,4,30);
  Instance.ArmedBandageHeal=Math.Clamp(Instance.ArmedBandageHeal,0,50);
  JsonFileLoader<HumansSettings>.SaveFile(path,Instance,error);
  Print("[Humans] Civilian melee spawn chance: " + Instance.MeleeSpawnChancePercent.ToString() + " percent; random weapon IDs 2-13. Settings: " + path);
  return Instance;
 }
}

