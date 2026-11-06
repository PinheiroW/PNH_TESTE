class JTSkinSurvivorInput:ContinuousInteractActionInput
{
 void JTSkinSurvivorInput(PlayerBase player)
 {
  SetInput("UAJoinThemSkin");
  m_Priority=70;
 }
}
class ActionJTSkinSurvivor:ActionContinuousBase
{
 void ActionJTSkinSurvivor()
 {
  m_CallbackClass=ActionSkinningCB;
  m_CommandUID=DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING;
  m_StanceMask=DayZPlayerConstants.STANCEMASK_CROUCH;
  m_FullBody=true;
  m_Text="Skin survivor ( Hold Mouse 2 )";
 }
 PlayerBase GetBody(ActionTarget target)
 {
  if(!target)return null;
  PlayerBase body=PlayerBase.Cast(target.GetObject());
  if(!body)body=PlayerBase.Cast(target.GetParent());
  return body;
 }
 override typename GetInputType(){return JTSkinSurvivorInput;}
 override void CreateConditionComponents()
 {
  m_ConditionItem=new CCINone;
  m_ConditionTarget=new CCTCursorNoRuinCheck(2.0);
 }
 override bool ActionCondition(PlayerBase player,ActionTarget target,ItemBase item)
 {
  if(!player || player.m_JTState!=2 || !player.IsAlive() || player.IsUnconscious() || !target || player.GetHumanInventory().GetEntityInHands())return false;
  PlayerBase body=GetBody(target);
  return body && body!=player && !body.IsAlive() && !body.IsSkinned() && vector.DistanceSq(player.GetPosition(),body.GetPosition())<=4;
 }
 override void OnStartServer(ActionData action_data)
 {
  super.OnStartServer(action_data);
  action_data.m_Player.JTSkinSound(true);
 }
 override void OnEndServer(ActionData action_data)
 {
  action_data.m_Player.JTSkinSound(false);
  super.OnEndServer(action_data);
 }
 override void OnFinishProgressServer(ActionData action_data)
 {
  if(!ActionCondition(action_data.m_Player,action_data.m_Target,action_data.m_MainItem))return;
  PlayerBase body=GetBody(action_data.m_Target);
  body.SetAsSkinned(); // Atomically prevents a second reward from this corpse.
  int count=GetGame().ConfigGetInt("CfgVehicles "+body.GetType()+" Skinning Steaks count");
  if(count<=0)count=10;
  count=Math.Min(count,20);
  for(int i=0;i<count;i++)
  {
   vector p=body.GetPosition()+Vector(Math.RandomFloat(-0.5,0.5),0.1,Math.RandomFloat(-0.5,0.5));
   ItemBase meat=ItemBase.Cast(GetGame().CreateObjectEx("HumanSteakMeat",p,ECE_PLACE_ON_SURFACE));
   if(meat)meat.SetQuantity(meat.GetQuantityMax()*Math.RandomFloatInclusive(0.5,1));
  }
  // Keep the original corpse and its equipment on the native cleanup schedule.
 }
}
modded class ActionConstructor
{
 override void RegisterActions(TTypenameArray actions)
 {
  super.RegisterActions(actions);
  actions.Insert(ActionJTSkinSurvivor);
 }
}

