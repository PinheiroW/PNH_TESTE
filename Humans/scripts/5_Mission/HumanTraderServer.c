modded class MissionServer
{
 override void OnInit()
 {
  super.OnInit();HumanTraderSpawner.Instance=new HumanTraderSpawner;
  GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HumanTraderSpawner.Instance.Start,2000,false);
 }
 override void OnMissionFinish()
 {
  if(HumanTraderSpawner.Instance){GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(HumanTraderSpawner.Instance.Start);HumanTraderSpawner.Instance.Stop();}
  super.OnMissionFinish();
 }
}
