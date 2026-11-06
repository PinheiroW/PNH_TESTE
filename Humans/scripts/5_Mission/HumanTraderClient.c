class HumanTraderPanel:ScriptedWidgetEventHandler
{
 Widget Root;
 TextWidget Title;
 TextWidget Money;
 TextWidget SellValue;
 TextWidget SellCaption;
 TextListboxWidget Stock;
 TextListboxWidget InventoryList;
 MultilineTextWidget Status;
 ButtonWidget Buy;
 ButtonWidget Sell;
 ButtonWidget Refresh;
 ButtonWidget CloseButton;
 ref HTReply Data;
 HumanTraderF_Journalist NPC;
 int Sequence;
 float Heartbeat;
 float Waiting;
 bool Pending;
 int LastSellSelection=-2;
 ButtonWidget HoverButton;
 ButtonWidget PressedButton;
 ButtonWidget TradeButton(Widget w)
 {
  while(w && w!=Root)
  {
   if(w==Buy || w==Sell || w==Refresh || w==CloseButton)return ButtonWidget.Cast(w);
   w=w.GetParent();
  }
  return null;
 }
 bool ButtonAvailable(ButtonWidget control)
 {
  if(control==Buy)return !Pending;
  if(control==Sell)
  {
   if(Pending || !Data || !InventoryList)return false;
   int row=InventoryList.GetSelectedRow();
   return row>=0 && row<Data.Inventory.Count() && Data.Inventory[row].Price>0;
  }
  return true;
 }
 void PaintButton(ButtonWidget control)
 {
  if(!control)return;
  Widget surface=control.FindAnyWidget(control.GetName()+"Surface");
  Widget caption=control.FindAnyWidget(control.GetName()+"Caption");
  int background=ARGB(245,36,31,22);int foreground=ARGB(255,232,209,158);
  if(!ButtonAvailable(control)){background=ARGB(245,29,29,29);foreground=ARGB(255,125,125,120);}
  else if(PressedButton==control){background=ARGB(255,116,83,33);foreground=ARGB(255,255,245,210);}
  else if(HoverButton==control){background=ARGB(255,78,64,37);foreground=ARGB(255,255,239,184);}
  if(surface)surface.SetColor(background);if(caption)caption.SetColor(foreground);
 }
 void PaintButtons(){PaintButton(Buy);PaintButton(Sell);PaintButton(Refresh);PaintButton(CloseButton);}
 override bool OnMouseEnter(Widget w,int x,int y)
 {
  HoverButton=TradeButton(w);PaintButtons();return false;
 }
 override bool OnMouseLeave(Widget w,Widget enterW,int x,int y)
 {
  HoverButton=TradeButton(enterW);PressedButton=null;PaintButtons();return false;
 }
 override bool OnMouseButtonDown(Widget w,int x,int y,int button)
 {
  if(button==MouseState.LEFT)PressedButton=TradeButton(w);
  PaintButtons();return false;
 }
 override bool OnMouseButtonUp(Widget w,int x,int y,int button)
 {
  if(button==MouseState.LEFT)PressedButton=null;
  PaintButtons();return false;
 }

 void Lock(bool locked)
 {
  PlayerBase player=PlayerBase.Cast(GetGame().GetPlayer());
  if(locked){GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);if(player && player.GetInputController())player.GetInputController().SetDisabled(true);GetGame().GetUIManager().ShowUICursor(true);}
  else {if(player && player.GetInputController())player.GetInputController().SetDisabled(false);GetGame().GetMission().PlayerControlEnable(true);GetGame().GetUIManager().ShowUICursor(false);}
  GetUApi().SupressNextFrame(true);
 }
 void Open(HTReply reply)
 {
  Data=reply;NPC=HumanTraderF_Journalist.Cast(GetGame().GetObjectByNetworkId(reply.TraderLow,reply.TraderHigh));
  Root=GetGame().GetWorkspace().CreateWidgets("Humans/gui/HumanTrader.layout");
  if(!Root)return;
  Root.SetHandler(this);Title=TextWidget.Cast(Root.FindAnyWidget("Title"));Money=TextWidget.Cast(Root.FindAnyWidget("Money"));
  SellValue=TextWidget.Cast(Root.FindAnyWidget("SellValue"));
  Stock=TextListboxWidget.Cast(Root.FindAnyWidget("Stock"));InventoryList=TextListboxWidget.Cast(Root.FindAnyWidget("Inventory"));Status=MultilineTextWidget.Cast(Root.FindAnyWidget("Status"));
  Buy=ButtonWidget.Cast(Root.FindAnyWidget("Buy"));Sell=ButtonWidget.Cast(Root.FindAnyWidget("Sell"));Refresh=ButtonWidget.Cast(Root.FindAnyWidget("Refresh"));CloseButton=ButtonWidget.Cast(Root.FindAnyWidget("Close"));
  SellCaption=TextWidget.Cast(Root.FindAnyWidget("SellCaption"));
  if(!Title || !Money || !SellValue || !Stock || !InventoryList || !Status || !Buy || !Sell || !Refresh || !CloseButton || !SellCaption)
  {
   Print("[HumanTraderUI] ERROR: layout controls missing; closing safely.");Send(4);Root.Unlink();Root=null;return;
  }
  Print("[HumanTraderUI] Panel loaded: Buy, Sell, Money, SellValue, Status and Goodbye present.");
  Update(reply);Lock(true);
 }
 void Update(HTReply reply)
 {
  int shopRow=Stock.GetSelectedRow();int ownedRow=InventoryList.GetSelectedRow();Data=reply;
  Title.SetText(reply.Name);Money.SetText("Money: $"+reply.Money.ToString());
  Stock.ClearItems();InventoryList.ClearItems();
  foreach(HTTradeRow row:reply.Stock)Stock.AddItem(Widget.TranslateString(row.Name)+" | "+row.Price.ToString()+" | "+row.Detail,null,0);
  foreach(HTTradeRow inventoryRow:reply.Inventory)
  {
   string value=inventoryRow.Price.ToString();if(inventoryRow.Price<0)value="Not accepted";
   InventoryList.AddItem(Widget.TranslateString(inventoryRow.Name)+" | "+value+" | "+inventoryRow.Detail,null,0);
  }
  if(shopRow>=0 && shopRow<reply.Stock.Count())Stock.SelectRow(shopRow);
  if(ownedRow>=0 && ownedRow<reply.Inventory.Count())InventoryList.SelectRow(ownedRow);
  if(reply.Message!="")Status.SetText(reply.Message);
  Pending=false;Waiting=0;Buy.Enable(true);Sell.Enable(true);UpdateSellValue();
 }
 void UpdateSellValue()
 {
  if(!SellValue || !Sell || !SellCaption || !InventoryList || !Data)return;
  int selected=InventoryList.GetSelectedRow();LastSellSelection=selected;
  if(selected<0 || selected>=Data.Inventory.Count()){SellValue.SetText("Sell value: --");SellCaption.SetText("SELECT AN ITEM TO SELL");Sell.Enable(false);return;}
  HTTradeRow row=Data.Inventory[selected];
  if(row.Price<=0){SellValue.SetText("Not accepted");SellCaption.SetText("ITEM NOT ACCEPTED");Sell.Enable(false);return;}
  SellValue.SetText("Sell value: $"+row.Price.ToString());
  SellCaption.SetText("SELL FOR $"+row.Price.ToString());Sell.Show(true);Sell.Enable(!Pending);
 }
 override bool OnItemSelected(Widget w,int x,int y,int row,int column,int oldRow,int oldColumn)
 {
  if(w==InventoryList){UpdateSellValue();return true;}
  return false;
 }
 void Send(int operation)
 {
  if(!Data)return;
  HTRequest request=new HTRequest;request.TraderLow=Data.TraderLow;request.TraderHigh=Data.TraderHigh;request.Token=Data.Token;
  request.Operation=operation;Sequence++;request.Sequence=Sequence;
  int row;
  if(operation==1){row=Stock.GetSelectedRow();if(row<0 || row>=Data.Stock.Count()){Status.SetText("Choose a trader item first.");return;}request.Index=Data.Stock[row].Index;}
  if(operation==2)
  {
   row=InventoryList.GetSelectedRow();if(row<0 || row>=Data.Inventory.Count()){Status.SetText("Choose one of your items first.");return;}
   if(Data.Inventory[row].Price<=0){Status.SetText("This item is not accepted. Empty its cargo, remove attachments and unload it first.");return;}
   request.ExpectedSalePrice=Data.Inventory[row].Price;
   request.ItemLow=Data.Inventory[row].NetLow;request.ItemHigh=Data.Inventory[row].NetHigh;
  }
  PlayerBase player=PlayerBase.Cast(GetGame().GetPlayer());if(!player)return;
  if(GetGame().IsMultiplayer())GetGame().RPCSingleParam(player,HTClient.REQUEST_RPC,new Param1<ref HTRequest>(request),true);
  else if(NPC)NPC.Request(player,request);
  if(operation==1 || operation==2){Pending=true;Waiting=0;Buy.Enable(false);Sell.Enable(false);Status.SetText("Processing trade...");PaintButtons();}
 }
 void Tick(float dt)
 {
  Lock(true);Heartbeat+=dt;PaintButtons();
  if(InventoryList.GetSelectedRow()!=LastSellSelection)UpdateSellValue();
  if(Pending){Waiting+=dt;if(Waiting>4){Pending=false;Buy.Enable(true);UpdateSellValue();Status.SetText("Refreshing trade status...");Send(3);}}
  if(Heartbeat>5){Heartbeat=0;Send(3);}
 }
 override bool OnClick(Widget w,int x,int y,int button)
 {
  if(button!=MouseState.LEFT)return false;
  w=TradeButton(w);if(!w)return false;
  if(w==CloseButton){Close();return true;}
  if(w==Refresh){Send(3);return true;}
  if(!Pending && w==Buy){Send(1);return true;}
  if(!Pending && w==Sell){Send(2);return true;}
  return false;
 }
 void Close()
 {
  if(Root){Send(4);Root.Unlink();Root=null;Lock(false);}NPC=null;
 }
}
modded class MissionGameplay
{
 ref HumanTraderPanel m_HTPanel;
 Widget m_HTPrompt;
 bool m_HTPending;
 float m_HTWait;
#ifdef DIAG_DEVELOPER
 bool m_HTLayoutChecked;
 void CheckTraderLayout()
 {
  m_HTLayoutChecked=true;
  Widget probe=GetGame().GetWorkspace().CreateWidgets("Humans/gui/HumanTrader.layout");
  if(!probe){Print("[HumanTraderUI] FAIL: layout construction");return;}
  array<string> names={"Title","Stock","Inventory","SellValue","Buy","BuyCaption","Sell","SellCaption","Money","Status","Refresh","Close"};
  bool valid=true;foreach(string name:names){if(!probe.FindAnyWidget(name)){valid=false;Print("[HumanTraderUI] MISSING: "+name);}}
  if(valid)Print("[HumanTraderUI] PASS: all 12 required controls constructed.");
  probe.Unlink();
 }
#endif

 HumanTraderF_Journalist FindHumanTrader(PlayerBase player)
 {
  HumanTraderF_Journalist selected;float best=9;
  vector camera=GetGame().GetCurrentCameraPosition();vector facing=GetGame().GetCurrentCameraDirection();
  foreach(HumanTraderF_Journalist trader:HumanTraderF_Journalist.Traders)
  {
   if(!trader || !trader.IsAlive())continue;
   float distance=vector.DistanceSq(player.GetPosition(),trader.GetPosition());if(distance>best)continue;
   vector direction=vector.Direction(camera,trader.GetPosition()+"0 1.25 0");direction.Normalize();
   if(vector.Dot(direction,facing)<0.65 || !trader.HumanSight(player))continue;
   selected=trader;best=distance;
  }
  return selected;
 }
 override void OnUpdate(float timeslice)
 {
  super.OnUpdate(timeslice);
#ifdef DIAG_DEVELOPER
  if(!m_HTLayoutChecked && GetGame().GetWorkspace())CheckTraderLayout();
#endif
  PlayerBase player=PlayerBase.Cast(GetGame().GetPlayer());
  if(!player || !player.IsAlive() || player.IsUnconscious() || GetGame().GetUIManager().GetMenu())
  {
   if(m_HTPanel && m_HTPanel.Root)m_HTPanel.Close();if(m_HTPrompt)m_HTPrompt.Show(false);m_HTPending=false;HTClient.Ready=false;return;
  }
  if(HTClient.Ready)
  {
   HTClient.Ready=false;HTReply reply=HTClient.Reply;
   if(reply && reply.Closed){if(m_HTPending && reply.Message!="")player.MessageStatus(reply.Message);if(m_HTPanel && m_HTPanel.Root && m_HTPanel.Data.TraderLow==reply.TraderLow && m_HTPanel.Data.TraderHigh==reply.TraderHigh)m_HTPanel.Close();m_HTPending=false;}
   else if(reply)
   {
    if(m_HTPanel && m_HTPanel.Root){if(m_HTPanel.Data.Token==reply.Token)m_HTPanel.Update(reply);}
    else if(m_HTPending){m_HTPanel=new HumanTraderPanel;m_HTPanel.Open(reply);m_HTPending=false;}
   }
  }
  if(m_HTPanel && m_HTPanel.Root)
  {
   if(m_HTPrompt)m_HTPrompt.Show(false);
   if(!m_HTPanel.NPC || !m_HTPanel.NPC.IsAlive() || vector.DistanceSq(player.GetPosition(),m_HTPanel.NPC.GetPosition())>25 || GetGame().GetInput().LocalPress("UAUIBack",false)){m_HTPanel.Close();return;}
   m_HTPanel.Tick(timeslice);return;
  }
  if(m_HTPending){m_HTWait+=timeslice;if(m_HTWait>4)m_HTPending=false;}
  HumanTraderF_Journalist target=FindHumanTrader(player);
  if(!m_HTPrompt)m_HTPrompt=GetGame().GetWorkspace().CreateWidgets("Humans/gui/HumanTraderPrompt.layout");
  if(m_HTPrompt)m_HTPrompt.Show(target!=null);
  if(target && !m_HTPending && GetGame().GetInput().LocalPress("UAHumanTraderTalk",false))
  {
   HTRequest request=new HTRequest;target.GetNetworkID(request.TraderLow,request.TraderHigh);request.Operation=0;
   HTClient.Ready=false;m_HTPending=true;m_HTWait=0;
   if(GetGame().IsMultiplayer())GetGame().RPCSingleParam(player,HTClient.REQUEST_RPC,new Param1<ref HTRequest>(request),true);else target.Request(player,request);
  }
 }
 override void OnMissionFinish()
 {
  if(m_HTPanel && m_HTPanel.Root)m_HTPanel.Close();if(m_HTPrompt)m_HTPrompt.Unlink();HTClient.Reply=null;HTClient.Ready=false;
  super.OnMissionFinish();
 }
}
