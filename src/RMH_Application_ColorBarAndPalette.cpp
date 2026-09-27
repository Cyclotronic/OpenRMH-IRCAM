
/*
 *  RMH_Application_ColorBarAndPalette.cpp
 *
 *  Author: Rune Mark Hansen
 *  Date: November 2022
 *
 */

// Included libraries
#include "RMH_Application_ColorBarAndPalette.h"
#include "RMH_ThermalCameraSupport_Library.h"
#include "RMH_ImageProcessing_Library.h"
#include "RMH_Winforms_Library.h"
#include <iostream>

// Included resources
#include "GlobalObjectsAndVariables.h"
#include "RMH_CustomColorPalette_Resources.h"

// Global namespaces
using namespace System;
using namespace std;

// --------------------------- Color Palette Handling Routines --------------------------- //

void RMH_ColorPalette_LoadColorPalettesToCombiBox(System::Windows::Forms::ComboBox^ ColorPaletteComboBox) {

	// This routine loads the available color palette names, from resources, into the associated ComboBox

	// Insert the list of available color palettes into the "Color Palette" ComboBox
	RMH_Winforms_CombiBox_AddArrayOfItemStrings(ColorPaletteComboBox, ColorPaletteNames);

}

void RMH_ColorPalette_ChangeColorPalette(System::Windows::Forms::ComboBox^ ColorPaletteComboBox) {

	// This routine sets the selected color palette to the live view PictureBox

	// Read the selected color palette
	unsigned int ColorPaletteIndex = ColorPaletteComboBox->SelectedIndex;

	// Setting of the selected color palette
	switch (ColorPaletteIndex) {

		// Color Palette: Parula
		case _ColorPaletteIndex_Parula:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Parula;

		break;

		// Color Palette: Turbo
		case _ColorPaletteIndex_Turbo:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Turbo;

		break;

		// Color Palette: Jet
		case _ColorPaletteIndex_Jet:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Jet;

		break;

		// Color Palette: HSV
		case _ColorPaletteIndex_HSV:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_HSV;

		break;

		// Color Palette: Hot
		case _ColorPaletteIndex_Hot:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Hot;

		break;

		// Color Palette: Cool
		case _ColorPaletteIndex_Cool:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Cool;

		break;

		// Color Palette: Spring
		case _ColorPaletteIndex_Spring:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Spring;

		break;

		// Color Palette: Summer
		case _ColorPaletteIndex_Summer:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Summer;

		break;

		// Color Palette: Autumn
		case _ColorPaletteIndex_Autumn:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Autumn;

		break;

		// Color Palette: Winter
		case _ColorPaletteIndex_Winter:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Winter;

		break;

		// Color Palette: Gray
		case _ColorPaletteIndex_Gray:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Gray;

		break;

		// Color Palette: Bone
		case _ColorPaletteIndex_Bone:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Bone;

		break;

		// Color Palette: Copper
		case _ColorPaletteIndex_Copper:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Copper;

		break;

		// Color Palette: Pink
		case _ColorPaletteIndex_Pink:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Pink;

		break;

		// Color Palette: Custom DarkHot
		case _ColorPaletteIndex_DarkHot:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_DarkHot;

		break;

		// Color Palette: Custom ColdSpot
		case _ColorPaletteIndex_ColdSpot:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_ColdSpot;

		break;

		// Color Palette: Custom ColdHotSpot
		case _ColorPaletteIndex_ColdHotSpot:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_ColdHotSpot;

		break;

		// Color Palette: Custom BlackRed
		case _ColorPaletteIndex_BlackRed:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_BlackRed;

		break;

		// Color Palette: Custom Inferno
		case _ColorPaletteIndex_Inferno:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Inferno;

		break;

		// Color Palette: Custom Magma
		case _ColorPaletteIndex_Magma:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Magma;

		break;

		// Color Palette: Custom Plasma
		case _ColorPaletteIndex_Plasma:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Plasma;

		break;

		// Color Palette: Custom Lava
		case _ColorPaletteIndex_Lava:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Lava;

		break;

		// Color Palette: Custom LavaHT
		case _ColorPaletteIndex_LavaHT:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_LavaHT;

		break;

		// Color Palette: Custom InfiRay
		case _ColorPaletteIndex_InfiRay:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_InfiRay;

		break;

		// Color Palette: Custom BowHC
		case _ColorPaletteIndex_BowHC:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_BowHC;

		break;

		// Color Palette: Custom RainHC
		case _ColorPaletteIndex_RainHC:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_RainHC;

		break;

		// Color Palette: Custom RainHT
		case _ColorPaletteIndex_RainHT:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_RainHT;

		break;

		// Color Palette: Custom Iron
		case _ColorPaletteIndex_Iron:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Iron;

		break;

		// Color Palette: Custom Viridis
		case _ColorPaletteIndex_Viridis:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Viridis;

		break;

		// Color Palette: Custom Tesla
		case _ColorPaletteIndex_Tesla:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Tesla;

		break;

		// Color Palette: Custom Helix
		case _ColorPaletteIndex_Helix:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Helix;

		break;

		// Color Palette: Custom Grey10
		case _ColorPaletteIndex_Grey10:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Grey10;

		break;

		// Color Palette: Custom GreyRed
		case _ColorPaletteIndex_GreyRed:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_GreyRed;

		break;

		// Color Palette: Custom Iron10
		case _ColorPaletteIndex_Iron10:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Iron10;

		break;

		// Color Palette: Custom Medical
		case _ColorPaletteIndex_Medical1:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Medical1;

		break;

		// Color Palette: Custom Medical
		case _ColorPaletteIndex_Medical2:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Medical2;

		break;

		// Color Palette: Custom MIdGrey
		case _ColorPaletteIndex_MidGrey:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_MidGrey;

		break;

		// Color Palette: Custom Prism
		case _ColorPaletteIndex_Prism:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Prism;

		break;

		// Color Palette: Custom Rain
		case _ColorPaletteIndex_Rain:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Rain;

		break;

		// Color Palette: Custom Rain10
		case _ColorPaletteIndex_Rain10:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Rain10;

		break;

		// Color Palette: Custom DarkRed
		case _ColorPaletteIndex_DarkRed:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_DarkRed;

		break;

		// Color Palette: Custom Lambda
		case _ColorPaletteIndex_Lambda:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Lambda;

		break;

		// Color Palette: Custom AllWhite
		case _ColorPaletteIndex_AllWhite:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_AllWhite;

		break;

		// Color Palette: Custom AllBlack
		case _ColorPaletteIndex_AllBlack:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_AllBlack;

		break;

		// Color Palette: Custom InfernoEX
		case _ColorPaletteIndex_InfernoEX:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_InfernoEX;

		break;

		// Color Palette: Custom IsoRainBow1
		case _ColorPaletteIndex_IsoRainBow1:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_IsoRainBow1;

		break;

		// Color Palette: Custom IsoRainBow2
		case _ColorPaletteIndex_IsoRainBow2:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_IsoRainBow2;

		break;

		// Color Palette: Custom IsoTropic
		case _ColorPaletteIndex_IsoTropic:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_IsoTropic;

		break;

		// Color Palette: Custom UAVFlying
		case _ColorPaletteIndex_UAVFlying:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_UAVFlying;

		break;

		// Color Palette: Custom SpectrumHot
		case _ColorPaletteIndex_SpectrumHot:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_SpectrumHot;

		break;

		// Color Palette: Custom Epsilon
		case _ColorPaletteIndex_Epsilon:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_Epsilon;

		break;

		// Color Palette: Custom EmissivityMap
		case _ColorPaletteIndex_EmissivityMap:

			// Update the color palette pointer
			ColorPalettePtr = RMH_CustomPalette_EmissivityMap;

		break;

	}

}

void RMH_ColorPalette_ChangeDualColorPalette(System::Windows::Forms::ComboBox^ DualColorPaletteComboBox) {

	// This routine sets the selected dual color palette to the live view PictureBox

	// Read the selected dual color palette
	unsigned int ColorPaletteIndex = DualColorPaletteComboBox->SelectedIndex;

	// Setting of the selected color palette
	switch (ColorPaletteIndex) {

		// Color Palette: Parula
		case _ColorPaletteIndex_Parula:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Parula;

		break;

		// Color Palette: Turbo
		case _ColorPaletteIndex_Turbo:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Turbo;

		break;

		// Color Palette: Jet
		case _ColorPaletteIndex_Jet:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Jet;

		break;

		// Color Palette: HSV
		case _ColorPaletteIndex_HSV:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_HSV;

		break;

		// Color Palette: Hot
		case _ColorPaletteIndex_Hot:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Hot;

		break;

		// Color Palette: Cool
		case _ColorPaletteIndex_Cool:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Cool;

		break;

		// Color Palette: Spring
		case _ColorPaletteIndex_Spring:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Spring;

		break;

		// Color Palette: Summer
		case _ColorPaletteIndex_Summer:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Summer;

		break;

		// Color Palette: Autumn
		case _ColorPaletteIndex_Autumn:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Autumn;

		break;

		// Color Palette: Winter
		case _ColorPaletteIndex_Winter:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Winter;

		break;

		// Color Palette: Gray
		case _ColorPaletteIndex_Gray:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Gray;

		break;

		// Color Palette: Bone
		case _ColorPaletteIndex_Bone:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Bone;

		break;

		// Color Palette: Copper
		case _ColorPaletteIndex_Copper:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Copper;

		break;

		// Color Palette: Pink
		case _ColorPaletteIndex_Pink:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Pink;

		break;

		// Color Palette: Custom DarkHot
		case _ColorPaletteIndex_DarkHot:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_DarkHot;

		break;

		// Color Palette: Custom ColdSpot
		case _ColorPaletteIndex_ColdSpot:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_ColdSpot;

		break;

		// Color Palette: Custom ColdHotSpot
		case _ColorPaletteIndex_ColdHotSpot:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_ColdHotSpot;

		break;

		// Color Palette: Custom BlackRed
		case _ColorPaletteIndex_BlackRed:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_BlackRed;

		break;

		// Color Palette: Custom Inferno
		case _ColorPaletteIndex_Inferno:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Inferno;

		break;

		// Color Palette: Custom Magma
		case _ColorPaletteIndex_Magma:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Magma;

		break;

		// Color Palette: Custom Plasma
		case _ColorPaletteIndex_Plasma:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Plasma;

		break;

		// Color Palette: Custom Lava
		case _ColorPaletteIndex_Lava:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Lava;

		break;

		// Color Palette: Custom LavaHT
		case _ColorPaletteIndex_LavaHT:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_LavaHT;

		break;

		// Color Palette: Custom InfiRay
		case _ColorPaletteIndex_InfiRay:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_InfiRay;

		break;

		// Color Palette: Custom BowHC
		case _ColorPaletteIndex_BowHC:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_BowHC;

		break;

		// Color Palette: Custom RainHC
		case _ColorPaletteIndex_RainHC:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_RainHC;

		break;

		// Color Palette: Custom RainHT
		case _ColorPaletteIndex_RainHT:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_RainHT;

		break;

		// Color Palette: Custom Iron
		case _ColorPaletteIndex_Iron:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Iron;

		break;

		// Color Palette: Custom Viridis
		case _ColorPaletteIndex_Viridis:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Viridis;

		break;

		// Color Palette: Custom Tesla
		case _ColorPaletteIndex_Tesla:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Tesla;
			 
		break;

		// Color Palette: Custom Helix
		case _ColorPaletteIndex_Helix:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Helix;

		break;

		// Color Palette: Custom Grey10
		case _ColorPaletteIndex_Grey10:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Grey10;

		break;

		// Color Palette: Custom GreyRed
		case _ColorPaletteIndex_GreyRed:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_GreyRed;

		break;

		// Color Palette: Custom Iron10
		case _ColorPaletteIndex_Iron10:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Iron10;

		break;

		// Color Palette: Custom Medical 1
		case _ColorPaletteIndex_Medical1:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Medical1;

		break;

		// Color Palette: Custom Medical 2
		case _ColorPaletteIndex_Medical2:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Medical2;

		break;

		// Color Palette: Custom MIdGrey
		case _ColorPaletteIndex_MidGrey:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_MidGrey;

		break;

		// Color Palette: Custom Prism
		case _ColorPaletteIndex_Prism:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Prism;

		break;

		// Color Palette: Custom Rain
		case _ColorPaletteIndex_Rain:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Rain;

		break;

		// Color Palette: Custom Rain10
		case _ColorPaletteIndex_Rain10:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Rain10;

		break;

		// Color Palette: Custom DarkRed
		case _ColorPaletteIndex_DarkRed:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_DarkRed;

		break;

		// Color Palette: Custom Lambda
		case _ColorPaletteIndex_Lambda:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Lambda;

		break;

		// Color Palette: Custom AllWhite
		case _ColorPaletteIndex_AllWhite:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_AllWhite;

		break;

		// Color Palette: Custom AllBlack
		case _ColorPaletteIndex_AllBlack:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_AllBlack;

		break;

		// Color Palette: Custom InfernoEX
		case _ColorPaletteIndex_InfernoEX:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_InfernoEX;

		break;

		// Color Palette: Custom IsoRainBow1
		case _ColorPaletteIndex_IsoRainBow1:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_IsoRainBow1;

		break;

		// Color Palette: Custom IsoRainBow2
		case _ColorPaletteIndex_IsoRainBow2:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_IsoRainBow2;

		break;

		// Color Palette: Custom IsoTropic
		case _ColorPaletteIndex_IsoTropic:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_IsoTropic;

		break;

		// Color Palette: Custom UAVFlying
		case _ColorPaletteIndex_UAVFlying:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_UAVFlying;

		break;

		// Color Palette: Custom SpectrumHot
		case _ColorPaletteIndex_SpectrumHot:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_SpectrumHot;

		break;

		// Color Palette: Custom Epsilon
		case _ColorPaletteIndex_Epsilon:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_Epsilon;

		break;

		// Color Palette: Custom EmissivityMap
		case _ColorPaletteIndex_EmissivityMap:

			// Update the color palette pointer
			DualColorPalettePtr = RMH_CustomPalette_EmissivityMap;

		break;

	}

}

void RMH_ColorPalette_ChangeColorBarBackgroundColorPalette(System::Windows::Forms::ComboBox^ ColorBarBackPaletteComboBox) {

	// This routine sets the selected color palette as the background palette of the colorbar

	// Read the selected colorbar background color palette
	unsigned int ColorPaletteIndex = ColorBarBackPaletteComboBox->SelectedIndex;

	// Setting of the selected color palette
	switch (ColorPaletteIndex) {

		// Color Palette: Parula
		case _ColorPaletteIndex_Parula:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Parula;

		break;

		// Color Palette: Turbo
		case _ColorPaletteIndex_Turbo:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Turbo;

		break;

		// Color Palette: Jet
		case _ColorPaletteIndex_Jet:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Jet;

		break;

		// Color Palette: HSV
		case _ColorPaletteIndex_HSV:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_HSV;

		break;

		// Color Palette: Hot
		case _ColorPaletteIndex_Hot:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Hot;

		break;

		// Color Palette: Cool
		case _ColorPaletteIndex_Cool:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Cool;

		break;

		// Color Palette: Spring
		case _ColorPaletteIndex_Spring:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Spring;

		break;

		// Color Palette: Summer
		case _ColorPaletteIndex_Summer:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Summer;

		break;

		// Color Palette: Autumn
		case _ColorPaletteIndex_Autumn:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Autumn;

		break;

		// Color Palette: Winter
		case _ColorPaletteIndex_Winter:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Winter;

		break;

		// Color Palette: Gray
		case _ColorPaletteIndex_Gray:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Gray;

		break;

		// Color Palette: Bone
		case _ColorPaletteIndex_Bone:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Bone;

		break;

		// Color Palette: Copper
		case _ColorPaletteIndex_Copper:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Copper;

		break;

		// Color Palette: Pink
		case _ColorPaletteIndex_Pink:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Pink;

		break;

		// Color Palette: Custom DarkHot
		case _ColorPaletteIndex_DarkHot:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_DarkHot;

		break;

		// Color Palette: Custom ColdSpot
		case _ColorPaletteIndex_ColdSpot:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_ColdSpot;

		break;

		// Color Palette: Custom ColdHotSpot
		case _ColorPaletteIndex_ColdHotSpot:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_ColdHotSpot;

		break;

		// Color Palette: Custom BlackRed
		case _ColorPaletteIndex_BlackRed:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_BlackRed;

		break;

		// Color Palette: Custom Inferno
		case _ColorPaletteIndex_Inferno:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Inferno;

		break;

		// Color Palette: Custom Magma
		case _ColorPaletteIndex_Magma:
	
			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Magma;

		break;

		// Color Palette: Custom Plasma
		case _ColorPaletteIndex_Plasma:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Plasma;

		break;

		// Color Palette: Custom Lava
		case _ColorPaletteIndex_Lava:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Lava;

		break;

		// Color Palette: Custom LavaHT
		case _ColorPaletteIndex_LavaHT:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_LavaHT;

		break;

		// Color Palette: Custom InfiRay
		case _ColorPaletteIndex_InfiRay:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_InfiRay;

		break;

		// Color Palette: Custom BowHC
		case _ColorPaletteIndex_BowHC:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_BowHC;

		break;

		// Color Palette: Custom RainHC
		case _ColorPaletteIndex_RainHC:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_RainHC;

		break;

		// Color Palette: Custom RainHT
		case _ColorPaletteIndex_RainHT:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_RainHT;

		break;

		// Color Palette: Custom Iron
		case _ColorPaletteIndex_Iron:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Iron;

		break;

		// Color Palette: Custom Viridis
		case _ColorPaletteIndex_Viridis:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Viridis;

		break;

		// Color Palette: Custom Tesla
		case _ColorPaletteIndex_Tesla:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Tesla;

		break;

		// Color Palette: Custom Helix
		case _ColorPaletteIndex_Helix:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Helix;

		break;

		// Color Palette: Custom Grey10
		case _ColorPaletteIndex_Grey10:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Grey10;

		break;

		// Color Palette: Custom GreyRed
		case _ColorPaletteIndex_GreyRed:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_GreyRed;

		break;

		// Color Palette: Custom Iron10
		case _ColorPaletteIndex_Iron10:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Iron10;

		break;

		// Color Palette: Custom Medical 1
		case _ColorPaletteIndex_Medical1:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Medical1;

		break;

		// Color Palette: Custom Medical 2
		case _ColorPaletteIndex_Medical2:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Medical2;

			break;

		// Color Palette: Custom MIdGrey
		case _ColorPaletteIndex_MidGrey:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_MidGrey;

		break;

		// Color Palette: Custom Prism
		case _ColorPaletteIndex_Prism:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Prism;

		break;

		// Color Palette: Custom Rain
		case _ColorPaletteIndex_Rain:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Rain;

		break;

		// Color Palette: Custom Rain10
		case _ColorPaletteIndex_Rain10:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Rain10;

		break;

		// Color Palette: Custom DarkRed
		case _ColorPaletteIndex_DarkRed:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_DarkRed;

		break;

			// Color Palette: Custom Lambda
		case _ColorPaletteIndex_Lambda:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Lambda;

		break;

		// Color Palette: Custom AllWhite
		case _ColorPaletteIndex_AllWhite:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_AllWhite;

		break;

			// Color Palette: Custom AllBlack
		case _ColorPaletteIndex_AllBlack:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_AllBlack;

		break;

		// Color Palette: Custom InfernoEX
		case _ColorPaletteIndex_InfernoEX:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_InfernoEX;

		break;

		// Color Palette: Custom IsoRainBow1
		case _ColorPaletteIndex_IsoRainBow1:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_IsoRainBow1;

		break;

		// Color Palette: Custom IsoRainBow2
		case _ColorPaletteIndex_IsoRainBow2:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_IsoRainBow2;

		break;

		// Color Palette: Custom IsoTropic
		case _ColorPaletteIndex_IsoTropic:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_IsoTropic;

		break;

		// Color Palette: Custom UAVFlying
		case _ColorPaletteIndex_UAVFlying:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_UAVFlying;

		break;

		// Color Palette: Custom SpectrumHot
		case _ColorPaletteIndex_SpectrumHot:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_SpectrumHot;

		break;

		// Color Palette: Custom Epsilon
		case _ColorPaletteIndex_Epsilon:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_Epsilon;

		break;

		// Color Palette: Custom EmissivityMap
		case _ColorPaletteIndex_EmissivityMap:

			// Update the color palette pointer
			ColorBarBackPalettePtr = RMH_CustomPalette_EmissivityMap;

		break;

	}

}

void RMH_ColorPalette_EnableDualColorPalettes(System::Windows::Forms::Button^ DualColorPaletteButton) {

	// This routine handles the event when dual live view color palettes are enabled

	// Should the dual color palette be enabled or disabled
	if (DualColorPaletteEnableFlag == true) {

		// Update the border color of the dual color palette button
		DualColorPaletteButton->FlatAppearance->BorderColor = System::Drawing::Color::Lime;

	}
	else {

		// Reset the border color of the dual color palette button
		DualColorPaletteButton->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(255, 40, 40, 40);

	}

	// Update the graphic of the dual color palette button
	DualColorPaletteButton->Refresh();

}

void RMH_ColorPalette_UpdateHistogramColorPaletteInvertionState() {

	// This routine updates the inversion state of the histogram color palettes

	// Check which color palette the histogram uses
	// If the histogram color palette is set to the dual color palette
	if (HistogramDualOrLiveViewPaletteFlag == true) {

		// Should the dual color palette be inverted for the histogram
		if (InvertLiveViewDualPaletteFlag == true) {

			// Update the inversion flag of the histogram color palette 
			GlobalVariables::OpenGLHistogram->RMH_OpenGL_InvertHistogramColorPalette(true);

		}
		else {

			// Update the inversion flag of the histogram color palette 
			GlobalVariables::OpenGLHistogram->RMH_OpenGL_InvertHistogramColorPalette(false);

		}

	}
	else {

		// Should the live view color palette be inverted for the histogram
		if (InvertLiveViewPaletteFlag == true) {

			// Update the inversion flag of the histogram color palette 
			GlobalVariables::OpenGLHistogram->RMH_OpenGL_InvertHistogramColorPalette(true);

		}
		else {

			// Update the inversion flag of the histogram color palette 
			GlobalVariables::OpenGLHistogram->RMH_OpenGL_InvertHistogramColorPalette(false);

		}

	}

}

void RMH_ColorPalette_InvertColorPalettes(System::Object^ sender) {

	// This routine updates the inverted state of a selected color palette

	// Cast the sender object as a WinForms ToolStripMenuItem object
	System::Windows::Forms::ToolStripMenuItem^ InvertMenuItem = (System::Windows::Forms::ToolStripMenuItem^)sender;

	// Read the identification tag of the tool strip menu
	unsigned int InvertMenuItemTag = Convert::ToInt32(InvertMenuItem->Tag);

	// Which color palette should be inverted
	switch (InvertMenuItemTag) {

		// Invert the selected color palette
		case 0: InvertLiveViewPaletteFlag = !InvertLiveViewPaletteFlag;						break;
		case 1: InvertLiveViewDualPaletteFlag = !InvertLiveViewDualPaletteFlag;				break;
		case 2: InvertColorBarBackgroundPaletteFlag = !InvertColorBarBackgroundPaletteFlag; break;

	}

	// Update the inversion state of the histogram color palette
	RMH_ColorPalette_UpdateHistogramColorPaletteInvertionState();

	// Update the inversion states of the colorbar palettes 
	GlobalVariables::OpenGLColorBar->RMH_OpenGL_InvertColorBarPalettes(InvertLiveViewPaletteFlag, InvertLiveViewDualPaletteFlag);

}

// -------------------------- Color Bar Panel Handling Routines --------------------------- //

void RMH_ColorBar_ChangeManualRangeMouseWheelStepSize(System::Object^ sender) {

	// This routine sets the mouse wheel temperature step size of the colorbar

	// Read the temporary array data and sort the kernel array
	float MouseWheelStepSize = 0.0;

	// Cast the sender object as a WinForms ToolStrip object
	System::Windows::Forms::ToolStripMenuItem^ MenuStripIndex = (System::Windows::Forms::ToolStripMenuItem^)sender;

	// Read the sub context menu identification tag
	unsigned int IndexTag = Convert::ToInt32(MenuStripIndex->Tag);

	// Update to the selected step size
	switch (IndexTag) {

		// Configure the step size
		case 1: MouseWheelStepSize = 10.0; break;
		case 2: MouseWheelStepSize = 5.0;  break;
		case 3: MouseWheelStepSize = 1.0;  break;
		case 4: MouseWheelStepSize = 0.5;  break;
		case 5: MouseWheelStepSize = 0.2;  break;
		case 6: MouseWheelStepSize = 0.1;  break;

	}

	// Update the mouse wheel temperature step size of the colorbar
	GlobalVariables::OpenGLColorBar->RMH_OpenGL_SetMouseWheelTempOffsetStepSize(MouseWheelStepSize);

}

void RMH_ColorBar_ChangeColorBarAmountOfTemperatureTick(System::Object^ sender) {

	// This routine sets the number of colorbar temperature ticks to some fixed values

	// Read the temporary array data and sort the kernel array
	unsigned char ColorBarTempTicks = 0;

	// Cast the sender object as a WinForms ToolStrip object
	System::Windows::Forms::ToolStripMenuItem^ MenuStripIndex = (System::Windows::Forms::ToolStripMenuItem^)sender;

	// Read the sub context menu identification tag
	unsigned int IndexTag = Convert::ToInt32(MenuStripIndex->Tag);

	// Update to the selected number of ticks
	switch (IndexTag) {

		// Configure the step size
		case 1: ColorBarTempTicks = 5;  break;
		case 2: ColorBarTempTicks = 10; break;
		case 3: ColorBarTempTicks = 15; break;
		case 4: ColorBarTempTicks = 20; break;

	}

	// Update the number of temperature ticks of the colorbar
	NmbOfColorBarTempTicks = ColorBarTempTicks;

}

// ------------------------------------------------------------------------------------------ //
