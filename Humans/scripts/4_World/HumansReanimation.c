// Dead native infected cannot be revived safely. Keep the actual corpse and
// its loot during the delay, then perform one server-owned infected handover.
class HumanReanimation
{
 static ref array<ref HumanReanimationJob> Jobs={};
 static string ZombieType(string human)
 {
  human.Replace("HumanArmed","Human");
  if(human=="HumanM_HunterOld_Summer")return "HumanReanimatedM_HunterOld_Summer";
  if(human=="HumanM_Jacket_grey")return "HumanReanimatedM_Jacket_grey";
  if(human=="HumanM_JournalistSkinny")return "HumanReanimatedM_JournalistSkinny";
  if(human=="HumanF_SurvivorNormal_Blue")return "HumanReanimatedF_SurvivorNormal_Blue";
  if(human=="HumanM_SoldierNormal")return "HumanReanimatedM_SoldierNormal";
  if(human=="HumanM_SoldierNormal_Female")return "HumanReanimatedM_SoldierNormal_Female";
  return "";
 }
 static void Schedule(Human_Base body)
 {
  if(!body || body.IsAlive() || body.HumanIsTrader() || body.m_HRQueued || HumansSettings.Get().HumansReanimate!=1)return;
  string type=ZombieType(body.GetType());if(type=="")return;
  body.m_HRQueued=true;HumanReanimationJob job=new HumanReanimationJob;job.Body=body;job.Type=type;
  job.Due=GetGame().GetTime()*0.001+HumansSettings.Get().ReanimationDelaySeconds;
  body.SetLifetimeMax(Math.Max(body.GetLifetimeMax(),HumansSettings.Get().ReanimationDelaySeconds+60));
  body.SetLifetime(Math.Max(body.GetLifetime(),HumansSettings.Get().ReanimationDelaySeconds+60));
  Jobs.Insert(job);GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(job.Turn,HumansSettings.Get().ReanimationDelaySeconds*1000,false);
 }
}
modded class Human_Base
{
 bool m_HRQueued;
 override void EEKilled(Object killer)
 {
  super.EEKilled(killer);
  if(HumanAuthority())HumanReanimation.Schedule(this);
 }
}
class HumanReanimationJob
{
 Human_Base Body;
 HumanReanimated_Base InfectedActor;
 string Type;
 float Due;
 int Attempts;
 void Finish(){HumanReanimation.Jobs.RemoveItem(this);}
 void Turn()
 {
  if(!Body || Body.IsAlive()){Finish();return;}
  if(GetGame().GetTime()*0.001+0.01<Due)return;
  vector position=Body.GetPosition();vector orientation=Body.GetOrientation();orientation[1]=0;orientation[2]=0;
  InfectedActor=HumanReanimated_Base.Cast(GetGame().CreateObjectEx(Type,position,ECE_PLACE_ON_SURFACE|ECE_INITAI));
  if(!InfectedActor){Print("[Humans] Reanimation failed; original corpse and loot retained.");Finish();return;}
  InfectedActor.SetPosition(position);InfectedActor.SetOrientation(orientation);InfectedActor.UpdateSimulationPrecision(0);
  Body.SetInvisible(true);Transfer();
 }
 void Transfer()
 {
  if(!Body){if(InfectedActor)InfectedActor.BeginRise();Finish();return;}
  if(!InfectedActor){Body.SetInvisible(false);Finish();return;}
  array<EntityAI> items={};Body.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER,items);
  int remaining;
  foreach(EntityAI item:items)
  {
   if(!item || item==Body || item.GetHierarchyParent()!=Body)continue;
   remaining++;
   if(Attempts<10)
   {
    if(!Body.GetInventory().TakeEntityToTargetCargo(InventoryMode.SERVER,InfectedActor,item))Body.GetInventory().DropEntity(InventoryMode.SERVER,Body,item);
   }
   else Body.GetInventory().DropEntity(InventoryMode.SERVER,Body,item);
  }
  if(remaining==0)
  {
   GetGame().ObjectDelete(Body);Body=null;InfectedActor.BeginRise();Print("[Humans] Corpse reanimated with remaining loot preserved.");Finish();return;
  }
  Attempts++;
  if(Attempts>30)
  {
   // Never delete a body while it still owns items. Restore it rather than
   // cloning or silently destroying loot if another inventory mod blocks moves.
   Body.SetInvisible(false);InfectedActor.BeginRise();Print("[Humans] Inventory transfer blocked; original loot body retained alongside infected to prevent item loss.");Finish();return;
  }
  GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(Transfer,100,false);
 }
}
class HumanReanimated_Base extends ZombieMaleBase
{
 int HRPhase; // 0 settled lying pose, 1 getting up, 2 ordinary native infected
 float HRTime;
 float HRHeading;
 void HumanReanimated_Base(){RegisterNetSyncVariableInt("HRPhase",0,2);}
 override void EOnInit(IEntity other,int extra)
 {
  super.EOnInit(other,extra);HRHeading=GetOrientation()[0]*Math.DEG2RAD;
  if(GetGame().IsServer() || !GetGame().IsMultiplayer())
  {
   m_StanceVariation=0;DayZInfectedCommandMove move=GetCommand_Move();if(move){move.SetStanceVariation(0);move.SetIdleState(0);}
  }
 }
 void BeginRise()
 {
  if(!IsAlive())return;HRPhase=1;HRTime=0;HRHeading=GetOrientation()[0]*Math.DEG2RAD;SetSynchDirty();
  DayZInfectedCommandMove move=GetCommand_Move();if(move){move.SetStanceVariation(0);move.SetIdleState(1);}
  GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(FinishRise,2500,false);
 }
 void FinishRise()
 {
  if(!IsAlive() || HRPhase==2)return;
  HRPhase=2;SetSynchDirty();DayZInfectedInputController input=GetInputController();
  if(input){input.OverrideMovementSpeed(false,0);input.OverrideTurnSpeed(false,0);input.OverrideHeading(false,0);}
  DayZInfectedCommandMove move=GetCommand_Move();if(move)move.SetIdleState(2);
#ifdef VanillaSleepingZombies
  m_HZSleepPhase=3;m_HZSpawnedAwake=true;m_HZRestInitialized=true;
#endif
 }
 override void EEKilled(Object killer){GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(FinishRise);super.EEKilled(killer);}
 override void EEDelete(EntityAI parent){GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(FinishRise);super.EEDelete(parent);}
 override bool ModCommandHandlerBefore(float pDt,int pCurrentCommandID,bool pCurrentCommandFinished)
 {
  if(!IsAlive() || (!GetGame().IsServer() && GetGame().IsMultiplayer()))return super.ModCommandHandlerBefore(pDt,pCurrentCommandID,pCurrentCommandFinished);
  DayZInfectedInputController input=GetInputController();if(!input)return false;
  if(HRPhase<2)
  {
   HRTime+=pDt;input.OverrideMovementSpeed(true,0);input.OverrideTurnSpeed(true,0);input.OverrideHeading(true,HRHeading);
   DayZInfectedCommandMove move=GetCommand_Move();if(!move)move=StartCommand_Move();
   move.SetStanceVariation(0);int idle=0;if(HRPhase==1)idle=1;move.SetIdleState(idle);
   HandleMove(DayZInfectedConstants.COMMANDID_MOVE);HandleOrientation(pDt,DayZInfectedConstants.COMMANDID_MOVE);
   if(HRPhase==1 && HRTime>=2.5)FinishRise();
   return true;
  }
  return super.ModCommandHandlerBefore(pDt,pCurrentCommandID,pCurrentCommandFinished);
 }
 override bool HandleMindStateChange(int pCurrentCommandID,DayZInfectedInputController pInputController,float pDt)
 {
  bool result=super.HandleMindStateChange(pCurrentCommandID,pInputController,pDt);
  DayZInfectedCommandMove move=GetCommand_Move();if(HRPhase==2 && move)move.SetIdleState(2);
  return result;
 }
 override bool EvaluateCrawlTransitionAnimation(EntityAI pSource,string pComponent,string pAmmoType,out int pAnimType){pAnimType=-1;return false;}
}
class HumanReanimatedM_HunterOld_Summer extends HumanReanimated_Base {}
class HumanReanimatedM_Jacket_grey extends HumanReanimated_Base {}
class HumanReanimatedM_JournalistSkinny extends HumanReanimated_Base {}
class HumanReanimatedF_SurvivorNormal_Blue extends HumanReanimated_Base {}
class HumanReanimatedM_SoldierNormal extends HumanReanimated_Base {}
class HumanReanimatedM_SoldierNormal_Female extends HumanReanimated_Base {}
