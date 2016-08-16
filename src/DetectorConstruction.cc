#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "G4RotationMatrix.hh"
#include "G4Element.hh"
#include "G4SDManager.hh"
#include "CrystalSensitiveDetector.hh"



#include <cstdlib>
#include <cmath>


using namespace std;

DetectorConstruction::DetectorConstruction()
:G4VUserDetectorConstruction(),floorWidthX(2210*mm),floorWidthZ(3810*mm),floorThickness(0.5*m),wallsHeight(2.5*m),fADRheight(34*cm),
fADRheightaboveground(100*cm),fvec_roomoffset(-750*mm,-1525*mm,-(floorThickness/2.)-1.25*fADRheight)

{
	// materials
	DefineMaterials();
  fDetectorMessenger = new DetectorMessenger(this);
  // construct the geometry
  Construct();
  //ConstructDetectors();

	// create commands for interactive definition of the calorimeter

}

DetectorConstruction::~DetectorConstruction()
{ delete fDetectorMessenger;}

G4VPhysicalVolume*  DetectorConstruction::Construct()
{
  InitializeWorld();
  ConstructPit();
  ConstructOuterDetectors();
  //AddConcreteWalls();
  phystest = ConstructNuDetector();
  
  return phystest;
}

void DetectorConstruction::DefineMaterials()
{
	// use G4-NIST materials data base
	G4NistManager* man = G4NistManager::Instance();
	fMaterialGalactic = man->FindOrBuildMaterial("G4_Galactic");
	fMaterialGe = man->FindOrBuildMaterial("G4_Ge");
	fMaterialSi = man->FindOrBuildMaterial("G4_Si");
  fMaterialAir = man->FindOrBuildMaterial("G4_AIR");
  fMaterialPoly = man->FindOrBuildMaterial("G4_POLYETHYLENE");
  fMaterialConcrete = man->FindOrBuildMaterial("G4_CONCRETE");
  fMaterialLead = man->FindOrBuildMaterial("G4_Pb");
  fMaterialOs = man->FindOrBuildMaterial("G4_Os");
  fMaterialAl = man->FindOrBuildMaterial("G4_Al");
  fMaterialW = man->FindOrBuildMaterial("G4_W");
  fMaterialCu = man->FindOrBuildMaterial("G4_Cu");
  fMaterialPb = man->FindOrBuildMaterial("G4_Pb");
  fMaterialZn = man->FindOrBuildMaterial("G4_Zn");
  fMaterialZr = man->FindOrBuildMaterial("G4_Zr");  
  fMaterialGd = man->FindOrBuildMaterial("G4_Gd"); 

  elH = man->FindOrBuildElement("H");
  elO = man->FindOrBuildElement("O");
  elSi = man->FindOrBuildElement("Si");
  elMg = man->FindOrBuildElement("Mg");
  elAl = man->FindOrBuildElement("Al");
  elFe = man->FindOrBuildElement("Fe");
  elK = man->FindOrBuildElement("K");
  elB = man->FindOrBuildElement("B");
  elC = man->FindOrBuildElement("C");
  

  fMaterialScintillator = new G4Material("Scintillator",1.032*g/cm3,2);
  fMaterialScintillator->AddElement(elC,9);
  fMaterialScintillator->AddElement(elH,10);
  
  Chrysotile = new G4Material("Chrysotile",2.53*g/cm3,4);
  Chrysotile->AddElement(elMg,3);
  Chrysotile->AddElement(elSi,2);
  Chrysotile->AddElement(elO,9);
  Chrysotile->AddElement(elH,4);  
  fMaterialPoly_Borated = new G4Material("BoratedPoly",1.07*g/cm3,2);
  fMaterialPoly_Borated->AddMaterial(fMaterialPoly,95.0*perCent);
  fMaterialPoly_Borated->AddElement(elB,5.0*perCent);
  
  fMaterialQuartz = new G4Material("Quartz",2.81*g/cm3,2);
  fMaterialQuartz->AddElement(elSi,1);
  fMaterialQuartz->AddElement(elO,2);
  
  fMaterialCorundum = new G4Material("Corundum",2.81*g/cm3,2);
  fMaterialCorundum->AddElement(elAl,2);
  fMaterialCorundum->AddElement(elO,3);
  
  fMaterialIronOxide = new G4Material("IronOxide",2.81*g/cm3,2);
  fMaterialIronOxide->AddElement(elFe,1);
  fMaterialIronOxide->AddElement(elO,1);
  
  fMaterialMgOxide = new G4Material("MgOxide",2.81*g/cm3,2);
  fMaterialMgOxide->AddElement(elMg,1);
  fMaterialMgOxide->AddElement(elO,1);
  
  fMaterialKOxide = new G4Material("KOxide",2.81*g/cm3,2);
  fMaterialKOxide->AddElement(elK,2);
  fMaterialKOxide->AddElement(elO,1);

  fMaterialChoozRock = new G4Material("ChoozRock",2.81*g/cm3,5);
  fMaterialChoozRock->AddMaterial(fMaterialQuartz,58.0*perCent);
  fMaterialChoozRock->AddMaterial(fMaterialCorundum,19.0*perCent);
  fMaterialChoozRock->AddMaterial(fMaterialIronOxide,17.0*perCent);
  fMaterialChoozRock->AddMaterial(fMaterialMgOxide,4.0*perCent);
  fMaterialChoozRock->AddMaterial(fMaterialKOxide,2.0*perCent);
  
  mineraloil = new G4Material("MineralOil",0.86*g/cm3,2);
  mineraloil->AddElement(elC,25);
  mineraloil->AddElement(elH,43);  

  fMaterialSteel = new G4Material("Steel",7.9*g/cm3,2);
  fMaterialSteel->AddElement(elFe,98.0*perCent);
  fMaterialSteel->AddElement(elC,2.0*perCent);  

  fMaterialDodecane = new G4Material("Dodecane",0.75*g/cm3,2);
  fMaterialDodecane->AddElement(elC,12);
  fMaterialDodecane->AddElement(elH,24);  

  fMaterialPXE = new G4Material("PXE",0.988*g/cm3,2);
  fMaterialPXE->AddElement(elC,16);
  fMaterialPXE->AddElement(elH,18);

  fMaterialPPO = new G4Material("PPO",1.06*g/cm3,3);
  fMaterialPPO->AddElement(elC,3);
  fMaterialPPO->AddElement(elH,6);
  fMaterialPPO->AddElement(elO,1);
  
  fMaterialNuTarget = new G4Material("NuTargetLiquid",804.59*kg/m3,4);
  fMaterialNuTarget->AddMaterial(fMaterialDodecane,74.5*perCent);
  fMaterialNuTarget->AddMaterial(fMaterialPXE,24.6*perCent);
  fMaterialNuTarget->AddMaterial(fMaterialPPO,0.8*perCent);
  fMaterialNuTarget->AddMaterial(fMaterialGd,0.1*perCent);

  fMaterialGammaCatcher = new G4Material("GammaCatcherLiquid",800.6*kg/m3,3); 
  fMaterialGammaCatcher->AddMaterial(fMaterialDodecane,74.94*perCent);
  fMaterialGammaCatcher->AddMaterial(fMaterialPXE,24.68*perCent);
  fMaterialGammaCatcher->AddMaterial(fMaterialPPO,0.38*perCent);
  
}





void DetectorConstruction::InitializeWorld()
{
  fWorldSize  = 15.*m;
  fSolidWorld = new G4Box("fSolidWorld", fWorldSize/2., fWorldSize/2., fWorldSize/2.);
  fLogicWorld = new G4LogicalVolume(fSolidWorld, fMaterialGalactic, "world");
  fLogicWorld->SetVisAttributes(G4VisAttributes::GetInvisible());
  fPhysiWorld = new G4PVPlacement(0, G4ThreeVector(), fLogicWorld, "world", 0, false, 0);
}  


G4VPhysicalVolume*  DetectorConstruction::ConstructDetectors()
{
  DetectorSize = 1.5*cm;
  
  std::vector<std::string> CyrstalLabels = {"Crystal_1", "Crystal_2","Crystal_3", "Crystal_4","Crystal_5"};
  std::vector<G4double> Cyrstalpos_x = {0.*cm, -6.*cm, 6.*cm, 6.*cm, -6.*cm};
  std::vector<G4double> Cyrstalpos_y = {0.*cm, 6.*cm, 6.*cm, -6.*cm, -6.*cm};
  std::vector<G4double> Cyrstalpos_z = {0.*cm, 0.*cm, 0.*cm, 0.*cm, 0.*cm};
  G4VisAttributes visDetector(G4Colour(0.0,1.0,0.0));
  visDetector.SetForceWireframe(true);
  visDetector.SetForceAuxEdgeVisible(true);
  G4ThreeVector vec_zero(0*mm,0*mm,0*mm);
  G4ThreeVector vec_offset(0*mm,0*mm,0.5*cm+DetectorSize/2.0);
  G4SDManager* SDmanager = G4SDManager::GetSDMpointer();
  for(size_t jCrys = 0; jCrys < Cyrstalpos_x.size(); jCrys++)
  {
    v_CrystalBoxes.push_back(new G4Box(CyrstalLabels[jCrys],DetectorSize/2.0,DetectorSize/2.0,DetectorSize/2.0));
    v_CrystalBoxesLog.push_back(new G4LogicalVolume(v_CrystalBoxes[jCrys], fMaterialOs,CyrstalLabels[jCrys]));
    G4ThreeVector crys_pos(Cyrstalpos_x[jCrys],Cyrstalpos_y[jCrys],Cyrstalpos_z[jCrys]);
    v_CrystalBoxesPhys.push_back(new G4PVPlacement(0,crys_pos+vec_offset,v_CrystalBoxesLog[jCrys],CyrstalLabels[jCrys],fLogicWorld,false,0));
    v_CrystalBoxesLog[jCrys]->SetVisAttributes(visDetector);
    if (jCrys==0)
    {  
      CrystalSensitiveDetector *Crystal1_SD = new CrystalSensitiveDetector("Crystal_1SD");
      SDmanager->AddNewDetector(Crystal1_SD);
      v_CrystalBoxesLog[jCrys]->SetSensitiveDetector(Crystal1_SD);
    }  
  
  }
  return fPhysiWorld; 
}

G4VPhysicalVolume*  DetectorConstruction::ConstructSingleDetector()
{
  // arbitrarily assuming 4-inch iZIP here [AJA]
  // Taking out 4-inch iZIP for now [AFL - March 2016]
  //fSolidDetector = new G4Tubs("fSolidDetector", 0.*cm, 5.08*cm, 2.54*cm, 0*deg, 360*deg);
  //fLogicDetector = new G4LogicalVolume(fSolidDetector, fMaterialGe, "detector"); // in the future make detector material configurable between Si/Ge
  //fVisAttDetector = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0));
  //fVisAttDetector->SetForceSolid(true);
  //fLogicDetector->SetVisAttributes(fVisAttDetector);
  //fPhysiDetector = new G4PVPlacement(noRotation, G4ThreeVector(), fLogicDetector, "detector", fLogicWorld, false, 0);
  G4ThreeVector vec_zero(0*mm,0*mm,0*mm);
  this->crystalBox_single = new G4Box("Single Crystal",DetectorSize/2.0,DetectorSize/2.0,DetectorSize/2.0);
  this->crystalLog_single = new G4LogicalVolume(crystalBox_single,fMaterialZn, "Single Crystal");
  this->crystalPhys_single = new G4PVPlacement(0, vec_zero, crystalLog_single, "Single Crystal",fLogicWorld, false,0);
  G4VisAttributes visDetector(G4Colour(0.0,1.0,0.0));
  visDetector.SetForceWireframe(true);
  visDetector.SetForceAuxEdgeVisible(true);
  crystalLog_single->SetVisAttributes(visDetector); 
  return fPhysiWorld;  
}

void DetectorConstruction::ConstructOuterDetectors()
{
  // Gamma Catcher
  zeroradius = 0.*cm;
  startAngle = 0.*deg;
  spanningAngleFull = 360.*deg;
  G4ThreeVector vec_zero(0*mm,0*mm,0*mm);
  this->TempTube_inner = new G4Tubs("Temp_inner", zeroradius, 1158.0*mm, 1229.0*mm, startAngle, spanningAngleFull);
  this->TempTube_outer = new G4Tubs("Temp_outer", zeroradius, 1711.0*mm, 1787.0-15.0*mm,startAngle, spanningAngleFull);
  this->GCTubeSolid = new G4SubtractionSolid("Gammacatcher",TempTube_outer,TempTube_inner);
  this->GCTubeLog = new G4LogicalVolume(GCTubeSolid, fMaterialGammaCatcher, "GammaCatcher");
  this->GCTubePhys = new G4PVPlacement(0,vec_zero, GCTubeLog, "GammaCatcher",fLogicWorld, false,0);
  G4VisAttributes visGCTube(G4Colour(1.0,1.0,0.0));
  visGCTube.SetForceWireframe(true);
  visGCTube.SetForceAuxEdgeVisible(true);
  GCTubeLog->SetVisAttributes(visGCTube);
  TempTube_inner=0;
  TempTube_outer=0;
  this->TempTube_inner = new G4Tubs("Temp_inner", zeroradius, 1711.0*mm, 1787.0*mm, startAngle, spanningAngleFull);
  this->TempTube_outer = new G4Tubs("Temp_outer", zeroradius, 2761.0*mm, 2837.0-3.0*mm,startAngle, spanningAngleFull);
  this->BufferTubeSolid = new G4SubtractionSolid("Buffer",TempTube_outer,TempTube_inner);
  this->BufferTubeLog = new G4LogicalVolume(BufferTubeSolid, mineraloil, "Buffer");
  this->BufferTubePhys = new G4PVPlacement(0,vec_zero, BufferTubeLog, "Buffer",fLogicWorld, false,0);
  G4VisAttributes visBufferTube(G4Colour(1.0,0.269,0.0));
  visBufferTube.SetForceWireframe(true);
  visBufferTube.SetForceAuxEdgeVisible(true);
  BufferTubeLog->SetVisAttributes(visBufferTube);
  TempTube_inner=0;
  TempTube_outer=0;
  TempTube_inner = new G4Tubs("Temp_inner", zeroradius, 2761.0*mm, 2837.0*mm, startAngle, spanningAngleFull);
  TempTube_outer = new G4Tubs("Temp_outer", zeroradius, 3305.0*mm, 3320.0-10*mm,startAngle, spanningAngleFull);
  VetoTubeSolid = new G4SubtractionSolid("Veto",TempTube_outer,TempTube_inner);
  VetoTubeLog = new G4LogicalVolume(VetoTubeSolid, mineraloil, "Veto");
  VetoTubePhys = new G4PVPlacement(0,vec_zero, VetoTubeLog, "Veto",fLogicWorld, false,0);
  G4VisAttributes visVetoTube(G4Colour(0.0,0.0,1.0));
  visVetoTube.SetForceWireframe(true);
  visVetoTube.SetForceAuxEdgeVisible(true);
  VetoTubeLog->SetVisAttributes(visVetoTube);
  TempTube_inner=0;
  TempTube_outer=0;
  this->TempTube_inner = new G4Tubs("Temp_inner", zeroradius, 3305.0*mm, 3500.0-170.0*mm, startAngle, spanningAngleFull);
  this->TempTube_outer = new G4Tubs("Temp_outer", zeroradius, 3475.0*mm, 3500.0*mm,startAngle, spanningAngleFull);
  this->SteelTubeSolid = new G4SubtractionSolid("Shielding",TempTube_outer,TempTube_inner);
  this->SteelTubeLog = new G4LogicalVolume(SteelTubeSolid, fMaterialSteel, "Shielding");
  this->SteelTubePhys = new G4PVPlacement(0,vec_zero, SteelTubeLog, "Shielding",fLogicWorld, false,0);
  G4VisAttributes visSteelTube(G4Colour(0.375,0.375,0.375));
  visSteelTube.SetForceWireframe(true);
  visSteelTube.SetForceAuxEdgeVisible(true);
  SteelTubeLog->SetVisAttributes(visSteelTube);



}

G4VPhysicalVolume*  DetectorConstruction::ConstructNuDetector()
{
  startAngle = 0.*deg;
  spanningAngleFull = 360.*deg; 

  G4VisAttributes visDetector(G4Colour(0.0,1.0,0.0));
  visDetector.SetForceWireframe(true);
  visDetector.SetForceAuxEdgeVisible(true);
  G4ThreeVector vec_zero(0*mm,0*mm,0*mm);

  G4SDManager* SDmanager = G4SDManager::GetSDMpointer();

  TargetDetectorBox = new G4Tubs("NuDetector",0*m,1150*mm,1221*mm,startAngle, spanningAngleFull);
  TargetDetectorLog = new G4LogicalVolume(TargetDetectorBox, fMaterialNuTarget,"NuDetector");
  //G4ThreeVector crys_pos(Cyrstalpos_x[0],Cyrstalpos_y[0],Cyrstalpos_z[0]);
  TargetDetectorPhys = new G4PVPlacement(0,vec_zero,TargetDetectorLog,"NuDetector",fLogicWorld,false,0);
  TargetDetectorLog->SetVisAttributes(visDetector);
  CrystalSensitiveDetector *NuDetector = new CrystalSensitiveDetector("NuDetector");
  SDmanager->AddNewDetector(NuDetector);
  TargetDetectorLog->SetSensitiveDetector(NuDetector);
  return fPhysiWorld;  
}

void DetectorConstruction::ConstructPolySheilding(G4double innerR, G4double outerR,G4double topthickness)
{  
  // detector shielding is arbitrarily assuemd to be two concentric cylinders of
  // polyethylene and lead; poly is 10cm thick and lead is 4cm thick (again arbitrarily)
  // Taking out this version of shielding while I work on the ATR geometry [AFL]

  /*for(int jLayer = 0; jLayer < PbInnerRad.size(); jLayer++)
  {
    fSolidLeadShielding.push_back(new G4Tubs(PbLabels[jLayer], PbInnerRad[jLayer], PbOuterRad[jLayer], PbInnerShieldHeight, 0*deg, 360*deg));
    fLogicLeadShielding.push_back(new G4LogicalVolume(fSolidLeadShielding[jLayer], fMaterialLead, ""));
  }*/
  // Lead Tube
  zeroradius = 0.*cm;
  startAngle = 0.*deg;
  spanningAngleFull = 360.*deg;
  G4ThreeVector vec_zero(0*mm,0*mm,0*mm);
  outerRadius_shield = 1.0*outerR*cm;

  outerRadius_shield1 = 1.0*innerR*cm;
  fADRheight1 = fADRheight-2*(topthickness*cm);
  this->shieldTube = new G4Tubs("Polyshield", zeroradius, outerRadius_shield, fADRheight/2.0, startAngle, spanningAngleFull);
  this->shield1Tube = new G4Tubs("Polyshield", zeroradius, outerRadius_shield1, fADRheight1/2.0,startAngle, spanningAngleFull);
  G4SubtractionSolid* fullshieldTube = new G4SubtractionSolid("Shielding Shell (Poly)",shieldTube,shield1Tube);
  this->fullshieldTubeLog = new G4LogicalVolume(fullshieldTube, fMaterialPoly_Borated, "Shielding Shell (Borated Poly)");
  this->fullshieldTubePhys = new G4PVPlacement(0,vec_zero, fullshieldTubeLog, "Shielding shell (Borated Poly)",fLogicWorld, false,0);
  G4VisAttributes visshieldTube(G4Colour(1.0,0.586,0.0));
  visshieldTube.SetForceWireframe(true);
  visshieldTube.SetForceAuxEdgeVisible(true);
  fullshieldTubeLog->SetVisAttributes(visshieldTube);
}


void DetectorConstruction::ConstructPbSheilding(G4double innerR, G4double outerR,G4double topthickness)
{  
  // detector shielding is arbitrarily assuemd to be two concentric cylinders of
  // polyethylene and lead; poly is 10cm thick and lead is 4cm thick (again arbitrarily)
  // Taking out this version of shielding while I work on the ATR geometry [AFL]

  /*for(int jLayer = 0; jLayer < PbInnerRad.size(); jLayer++)
  {
    fSolidLeadShielding.push_back(new G4Tubs(PbLabels[jLayer], PbInnerRad[jLayer], PbOuterRad[jLayer], PbInnerShieldHeight, 0*deg, 360*deg));
    fLogicLeadShielding.push_back(new G4LogicalVolume(fSolidLeadShielding[jLayer], fMaterialLead, ""));
  }*/
  // Lead Tube
  zeroradius = 0.*cm;
  startAngle = 0.*deg;
  spanningAngleFull = 360.*deg;
  G4ThreeVector vec_zero(0*mm,0*mm,0*mm);
  outerRadius_shield = 1.0*outerR*cm;
  //fADRheight = 34*cm;
  outerRadius_shield1 = 1.0*innerR*cm;
  fADRheight1 = fADRheight-2*(topthickness*cm);
  this->shieldTube = new G4Tubs("Polyshield", zeroradius, outerRadius_shield, fADRheight/2.0, startAngle, spanningAngleFull);
  this->shield1Tube = new G4Tubs("Polyshield", zeroradius, outerRadius_shield1, fADRheight1/2.0,startAngle, spanningAngleFull);
  G4SubtractionSolid* fullshieldTube = new G4SubtractionSolid("Shielding Shell (Pb)",shieldTube,shield1Tube);
  this->fullshieldTubeLog = new G4LogicalVolume(fullshieldTube, fMaterialPb, "Shielding Shell (Pb)");
  this->fullshieldTubePhys = new G4PVPlacement(0,vec_zero, fullshieldTubeLog, "Shielding shell (Pb)",fLogicWorld, false,0);
  G4VisAttributes visshieldTube(G4Colour(1.0,0.586,0.0));
  visshieldTube.SetForceWireframe(true);
  visshieldTube.SetForceAuxEdgeVisible(true);
  fullshieldTubeLog->SetVisAttributes(visshieldTube);
}


void DetectorConstruction::ConstructADR()
{
  // copper plate
  outerRadius5 = 12.*cm;
  hz5 = 1.*cm;
  zeroradius = 0.*cm;
  startAngle = 0.*deg;
  startAngle90 = 90.*deg;
  spanningAngleFull = 360.*deg;
  G4ThreeVector vec_zero(0*mm,0*mm,0*mm);


  this->plateTube = new G4Tubs("Plate", zeroradius, outerRadius5, hz5/2., startAngle90, spanningAngleFull);
  this->plateLog = new G4LogicalVolume(plateTube, fMaterialCu, "Copper Plate");
  this->platePhys = new G4PVPlacement(0, vec_zero, plateLog, "Copper Plate", fLogicWorld, false,0);
  G4VisAttributes visplate(G4Colour(1.,0.,0.));
  visplate.SetForceWireframe(true);
  visplate.SetForceSolid(true);
  plateLog->SetVisAttributes(visplate);

  // inner Aluminum tube
  outerRadius1 = 13.97*cm;
  hz1 = 28.4*cm;
  outerRadius11 = 12.94*cm;
  hz11 = 27.4*cm;
  this->shell1Tube = new G4Tubs("Shell1", zeroradius, outerRadius1, hz1/2.0, startAngle, spanningAngleFull);
  this->shell11Tube = new G4Tubs("Shell11", zeroradius, outerRadius11, hz11/2.0,startAngle, spanningAngleFull);
  G4SubtractionSolid* shellinnerAlTube = new G4SubtractionSolid("Inner Shell",shell1Tube,shell11Tube);
  this->shellinnerAlTubeLog = new G4LogicalVolume(shellinnerAlTube, fMaterialAl, "Inner Shell");
  this->shellinnerAlTubePhys = new G4PVPlacement(0,vec_zero, shellinnerAlTubeLog, "Inner shell",fLogicWorld, false,0);
  G4VisAttributes visinnerAlTube(G4Colour(0.41,0.41,0.41));
  visinnerAlTube.SetForceWireframe(true);
  visinnerAlTube.SetForceAuxEdgeVisible(true);
  shellinnerAlTubeLog->SetVisAttributes(visinnerAlTube);

  // outer Aluminum tube/can
  outerRadius2 = 16.94*cm;
  hz2 = 29.4*cm;
  outerRadius21 = 15.94*cm;
  hz21 = 29.3*cm;
  this->shell2Tube = new G4Tubs("Shell2", zeroradius, outerRadius2, hz2/2.0, startAngle, spanningAngleFull);
  this->shell21Tube = new G4Tubs("Shell21", zeroradius, outerRadius21, hz21/2.0,startAngle, spanningAngleFull);
  G4SubtractionSolid* shellouterAlTube = new G4SubtractionSolid("Inner Shell",shell2Tube,shell21Tube);
  this->shellouterAlTubeLog = new G4LogicalVolume(shellouterAlTube, fMaterialAl, "Outer Shell");
  this->shellouterAlTubePhys = new G4PVPlacement(0,vec_zero, shellouterAlTubeLog, "Outer shell",fLogicWorld, false,0);
  G4VisAttributes visouterAlTube(G4Colour(0.41,0.41,0.41));
  visouterAlTube.SetForceWireframe(true);
  visouterAlTube.SetForceAuxEdgeVisible(true);
  shellouterAlTubeLog->SetVisAttributes(visouterAlTube);

    // Tungsten tube/can/sheild
  outerRadius3 = 12.1*cm;
  hz3 = 20.0*cm;
  outerRadius31 = 12.0*cm;
  hz31 = 19.9*cm;
  this->tungTube = new G4Tubs("Tungshell", zeroradius, outerRadius3, hz3/2.0, startAngle, spanningAngleFull);
  this->tung1Tube = new G4Tubs("Tungshell", zeroradius, outerRadius31, hz31/2.0,startAngle, spanningAngleFull);
  G4SubtractionSolid* fulltungTube = new G4SubtractionSolid("Tungsten Shell",tungTube,tung1Tube);
  this->fulltungTubeLog = new G4LogicalVolume(fulltungTube, fMaterialW, "Tungsten Shell");
  this->fulltungTubePhys = new G4PVPlacement(0,vec_zero, fulltungTubeLog, "Tungsten shell",fLogicWorld, false,0);
  G4VisAttributes vistungTube(G4Colour(0.18,0.31,0.31));
  vistungTube.SetForceWireframe(true);
  vistungTube.SetForceAuxEdgeVisible(true);
  fulltungTubeLog->SetVisAttributes(vistungTube);

  // Scintillator Tube
  outerRadius4 = 17.5*cm;
  hz4 = 31.4*cm;
  outerRadius41 = 16.5*cm;
  hz41 = 30.4*cm;
  this->scinTube = new G4Tubs("Scinshell", zeroradius, outerRadius4, hz4/2.0, startAngle, spanningAngleFull);
  this->scin1Tube = new G4Tubs("Scinshell", zeroradius, outerRadius41, hz41/2.0,startAngle, spanningAngleFull);
  G4SubtractionSolid* fullscinTube = new G4SubtractionSolid("Scintillator Shell",scinTube,scin1Tube);
  this->fullscinTubeLog = new G4LogicalVolume(fullscinTube, fMaterialScintillator, "Scintillator Shell");
  this->fullscinTubePhys = new G4PVPlacement(0,vec_zero, fullscinTubeLog, "Scintillator shell",fLogicWorld, false,0);
  G4VisAttributes visscinTube(G4Colour(0.0,0.0,1.0));
  visscinTube.SetForceWireframe(true);
  visscinTube.SetForceAuxEdgeVisible(true);
  fullscinTubeLog->SetVisAttributes(visscinTube);
}

void DetectorConstruction::ConstructPit()
{
  if(!fPhysiWorld)
  {
    G4cout << "ERROR in DetectorConstruction::ConstructPit" << G4endl;
    G4cout << "World volume does not exist!!" << G4endl;
    return;
  }  
  G4ThreeVector vec_zero(0*mm,0*mm,0*mm);
  zeroradius = 0.*cm;
  startAngle = 0.*deg;
  spanningAngleFull = 360.*deg;
  G4Box *solidRock = new G4Box("Rock",5.*m,5.*m,5.*m);
  G4Tubs *PitTube = new G4Tubs("Pittube", zeroradius, 3475*mm, 7000*mm/2.0, startAngle, spanningAngleFull);
  this->PitSolid = new G4SubtractionSolid("ChoozPit",solidRock,PitTube);
  this->PitLog = new G4LogicalVolume(PitSolid, fMaterialChoozRock, "logicalRock");
  this->PitPhys =  new G4PVPlacement(0, vec_zero, PitLog, "PVRock", fLogicWorld, false, 0);

  G4VisAttributes visRock(G4Colour(0.398,0.199,0.));
  visRock.SetForceWireframe(true);
  PitLog->SetVisAttributes(visRock);
}

void DetectorConstruction::AddConcreteFloor()
{
  if(!fPhysiWorld)
  {
    G4cout << "ERROR in DetectorConstruction::AddConcreteFloor" << G4endl;
    G4cout << "World volume does not exist!!" << G4endl;
    return;
  }
  //G4ThreeVector fvec_roomoffset(-354*mm,-1525*mm,-(floorThickness/2.)-1.25*fADRheight);
  G4ThreeVector vec_heightaboveground(0.,0.,-fADRheightaboveground);
  solidFloor = new G4Box("solidFloor", floorWidthX, floorWidthZ,floorThickness);
  logFloor = new G4LogicalVolume(solidFloor, fMaterialConcrete, "logicalFloor");
  physFloor = new G4PVPlacement(0, fvec_roomoffset+vec_heightaboveground, logFloor, "PVFloor", fLogicWorld, false, 0);

  // Set the visualizer properties
  G4VisAttributes visFloor(G4Colour(1.,0.,0.));
  visFloor.SetForceWireframe(true);
  logFloor->SetVisAttributes(visFloor);
}


void DetectorConstruction::AddCoffinWalls()
{
   G4double d_widthz = 5.839/2.0; // in meters
   G4double d_widthx = 8.277/2.0;
   G4double d_wallheight = 5.639/2.0;
   G4double d_wallthickness = .1;
   G4ThreeVector posWalls(0.,0.,0.);
   G4ThreeVector posfloor(0.,0.,0.);
   G4ThreeVector vec_heightaboveground(0.,0.,-fADRheightaboveground);
   G4RotationMatrix* nullRotation = new G4RotationMatrix();
   G4ThreeVector voidTranslation(0.0, -floorThickness, 0.0);
   G4ThreeVector vec_zero(0*mm,0*mm,0*mm);
   solidWallsOuterBox = new G4Box("solidWallsOuterBox", d_widthx*m, d_wallheight*m, d_widthz*m);
   coffinfloor = new G4Box("coffinfloor", d_widthx*m, floorThickness, d_widthz*m);
   solidWallsInnerVoid = new G4Box("solidWallsInnerVoid", d_widthx*m-2.0*d_wallthickness*m,d_wallheight*m-2.0*d_wallthickness*m, d_widthz*m-2.0*d_wallthickness*m);
   solidWalls = new G4SubtractionSolid("solidWalls", solidWallsOuterBox, solidWallsInnerVoid, nullRotation, voidTranslation);
   logWalls = new G4LogicalVolume(solidWalls, fMaterialConcrete, "logicalWalls");
   logFloor = new G4LogicalVolume(coffinfloor, fMaterialConcrete, "coffinfloor");
   physWalls = new G4PVPlacement(0, posWalls+fvec_roomoffset, logWalls, "PVWalls", fLogicWorld, false, 0);
   physFloor = new G4PVPlacement(0, posfloor+fvec_roomoffset, logFloor, "PVfloor", fLogicWorld, false, 0);
   G4VisAttributes viscoffin(G4Colour(1.,0.,0.));
   viscoffin.SetForceWireframe(true);
   logWalls->SetVisAttributes(viscoffin);
   G4VisAttributes visfloor(G4Colour(1.,0.,0.));
   visfloor.SetForceWireframe(false);
   logFloor->SetVisAttributes(visfloor);
}


void DetectorConstruction::AddConcreteWalls()
{
   //G4double floorWidthX = 2210*mm;
   //G4double floorWidthZ = 3810*mm;
   //G4double floorThickness = 0.5*m;
   //G4double wallsHeight = 2.5*m;


  if(!fPhysiWorld)
  {
    G4cout << "ERROR in DetectorConstruction::AddConcreteWalls" << G4endl;
    G4cout << "World volume does not exist!!" << G4endl;
    return;
    }

    // always build the floor by default when the walls are built
    if(!physFloor)
      AddConcreteFloor();

    // position vector of the walls
  G4double offset = wallsHeight/2.0-fADRheightaboveground-0.5*fADRheight;
  G4ThreeVector posWalls(0.,0.0,offset);
  G4ThreeVector posCeiling(0.,0.0,floorThickness/2.0+1.38*wallsHeight-fADRheightaboveground-0.5*fADRheight);
  G4ThreeVector vec_zero(0*mm,0*mm,0*mm);
  G4ThreeVector vec_wallA(-2561*mm,2057*mm,offset);
  G4ThreeVector vec_wallB(+209.7*mm,2057*mm,offset);
  G4ThreeVector vec_wallC(+551*mm,1372*mm,offset);
  G4ThreeVector vec_wallD(+1236*mm,1371*mm,offset);
  G4ThreeVector vec_wallE(+1236*mm,-2210*mm,offset);
  G4ThreeVector vec_testbox(-1649*mm,-1400*mm,offset-wallsHeight/2.0+38*cm);
  // transformation variables to help build the walls using a subtraction solid
  G4ThreeVector voidTranslation(0.0, 0.0,-floorThickness);

  //solidWallsOuterBox = new G4Box("solidWallsOuterBox", floorWidthX, wallsHeight, floorWidthZ);
  //solidWallsInnerVoid = new G4Box("solidWallsInnerVoid", floorWidthX-2.0*floorThickness,
  //                wallsHeight-floorThickness, floorWidthZ-2.0*floorThickness);
  //solidWalls = new G4SubtractionSolid("solidWalls", solidWallsOuterBox, solidWallsInnerVoid, nullRotation, voidTranslation);
  //logWalls = new G4LogicalVolume(solidWalls, concrete, "logicalWalls");
  //physFloor = new G4PVPlacement(0, posWalls+fvec_roomoffset, logWalls, "PVWalls", fLogicWorld, false, 0);


  //testbox = new G4Box("Test Box",42*cm,38*cm,79*cm);
    //testbox_logvolume = new G4LogicalVolume(testbox,Steel,"Test Box");


  inbetweenwall = new G4Box("Inbetweenwall",381*mm,229*mm,wallsHeight/2.0);
    inbetweenwall_logvolume = new G4LogicalVolume(inbetweenwall,fMaterialConcrete,"inbetweenwall_logvolume");
  inbetweenwall_B = new G4Box("Inbetweenwall_B",1123*mm,229*mm,wallsHeight/2.0);
    inbetweenwall_logvolume_B = new G4LogicalVolume(inbetweenwall_B,fMaterialConcrete,"inbetweenwall_logvolume_B");
    inbetweenwall_C = new G4Box("Inbetweenwall_C",457*mm,457*mm,wallsHeight/2.0);
    inbetweenwall_logvolume_C = new G4LogicalVolume(inbetweenwall_C,fMaterialConcrete,"inbetweenwall_logvolume_C");
  inbetweenwall_D = new G4Box("Inbetweenwall_D",229*mm,457*mm,wallsHeight/2.0);
    inbetweenwall_logvolume_D = new G4LogicalVolume(inbetweenwall_D,fMaterialConcrete,"inbetweenwall_logvolume_D");
  inbetweenwall_E = new G4Box("Inbetweenwall_E",267*mm,3125*mm,wallsHeight/2.0);
    inbetweenwall_logvolume_E = new G4LogicalVolume(inbetweenwall_E,Chrysotile,"inbetweenwall_logvolume_E");
    solidceiling = new G4Box("solidFloor", floorWidthX, floorWidthZ,floorThickness);
  logceiling = new G4LogicalVolume(solidceiling, fMaterialConcrete, "logicalCeiling");

    //pwinbetweenwall_logvolume_E = new G4LogicalVolume(inbetweenwall_E,fMaterialConcrete,"inbetweenwall_logvolume_E");

  //G4ThreeVector posinbetweenwall(-150.0*cm-(floorWidthX/4.0),offset,(1.0/6.0)*floorWidthZ);
    //inbetweenwall2 = new G4Box("Inbetweenwall2",182.88*cm,wallsHeight,floorThickness);
    //inbetweenwall_logvolume2 = new G4LogicalVolume(inbetweenwall,fMaterialConcrete,"inbetweenwall_logvolume2");
  //G4ThreeVector posinbetweenwall2(+150.0*cm+(floorWidthX/4.0),offset,(1.0/6.0)*floorWidthZ);

  //physinbetweenwall = new G4PVPlacement(0,posinbetweenwall+fvec_roomoffset,inbetweenwall_logvolume,"PVinbetweenWall",fLogicWorld,false,0);
  physceiling = new G4PVPlacement(0, posCeiling+fvec_roomoffset, logceiling,"PVCeiling", fLogicWorld, false, 0);
  physinbetweenwall = new G4PVPlacement(0,vec_wallA,inbetweenwall_logvolume,"PVinbetweenWall",fLogicWorld,false,0);
  physinbetweenwall_B = new G4PVPlacement(0,vec_wallB,inbetweenwall_logvolume_B,"PVinbetweenWall",fLogicWorld,false,0);
  physinbetweenwall_C = new G4PVPlacement(0,vec_wallC,inbetweenwall_logvolume_C,"PVinbetweenWall",fLogicWorld,false,0);
  physinbetweenwall_D = new G4PVPlacement(0,vec_wallD,inbetweenwall_logvolume_D,"PVinbetweenWall",fLogicWorld,false,0);
  physinbetweenwall_E = new G4PVPlacement(0,vec_wallE,inbetweenwall_logvolume_E,"PVinbetweenWall",fLogicWorld,false,0);
  //phystestbox = new G4PVPlacement(0,vec_testbox,testbox_logvolume,"PVinbetweenWall",fLogicWorld,false,0);
  //physinbetweenwall = new G4PVPlacement(0,posinbetweenwall2+fvec_roomoffset,inbetweenwall_logvolume2,"PVinbetweenWall",fLogicWorld,false,0);
  // Set the visualizer properties

  //G4VisAttributes visWalls(G4Colour(1.,0.,0.));
  //visWalls.SetForceWireframe(true);
  //logWalls->SetVisAttributes(visWalls);

  G4VisAttributes visbetweenwall(G4Colour(0.,1.,0.));
  visbetweenwall.SetForceWireframe(true);
  inbetweenwall_logvolume->SetVisAttributes(visbetweenwall);
  G4VisAttributes visbetweenwall_B(G4Colour(0.,1.,0.));
  visbetweenwall_B.SetForceWireframe(true);
  inbetweenwall_logvolume_B->SetVisAttributes(visbetweenwall_B);
  G4VisAttributes visbetweenwall_C(G4Colour(0.,1.,0.));
  visbetweenwall_C.SetForceWireframe(true);
  inbetweenwall_logvolume_C->SetVisAttributes(visbetweenwall_C);
  G4VisAttributes visbetweenwall_D(G4Colour(0.,1.,0.));
  visbetweenwall_D.SetForceWireframe(true);
  inbetweenwall_logvolume_D->SetVisAttributes(visbetweenwall_D);
  G4VisAttributes visbetweenwall_E(G4Colour(0.,0.,1.));
  visbetweenwall_E.SetForceWireframe(true);
  inbetweenwall_logvolume_E->SetVisAttributes(visbetweenwall_E);

    G4VisAttributes visceiling(G4Colour(1.,0.,0.));
    visceiling.SetForceWireframe(true);
    logceiling->SetVisAttributes(visceiling);


}


void DetectorConstruction::SetCrystalMaterial(G4String Material)
{
  if (Material == "Os" || Material == "Zn" || Material == "Zr")
  {
    crystalmaterial = Material;
    if (Material == "Os")
    {
      for (int i=0;i<5;i++)
      {  
        v_CrystalBoxesLog[i]->SetMaterial(fMaterialOs);
      }  
    }
    if (Material == "Zn")
    {
      for (int i=0;i<5;i++)
      {  
        v_CrystalBoxesLog[i]->SetMaterial(fMaterialZn);
      }
    }  
    if (Material == "Zr")
    {
      for (int i=0;i<5;i++)
      {  
        v_CrystalBoxesLog[i]->SetMaterial(fMaterialZr);
      }
    }   
  }
  else
  {
    std::cerr << "ERROR in SetCrystalMaterial source type must be Os or Zn setting to Os!" << std::endl;
    crystalmaterial = "Os";
    for (int i=0;i<5;i++)
    {  
      v_CrystalBoxesLog[i]->SetMaterial(fMaterialOs);
    }     
  }
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
  
  

}