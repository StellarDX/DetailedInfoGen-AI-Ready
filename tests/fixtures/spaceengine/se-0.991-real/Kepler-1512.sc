Star	"Kepler-1512/KOI-3497/KIC 8424002"
{
	ParentBody     "Kepler-1512"
	Class          "K4 V"
	Luminosity      0.0985893533
	LumBol          0.105155021
	MassSol         0.730000019
	RadSol          0.670000017
	Teff            4372

	Age             3.78

	InertiaMoment   0.0455791168
	RotationPeriod  562.13205
	RotationEpoch   0
	Obliquity       100.284081
	EqAscendNode    84.9844666

	AlbedoBond      0.3
	AlbedoGeom      0.36
	Brightness      1
	BrightnessReal  1
	Color          (1.000 0.708 0.469)

	Surface
	{
		SurfStyle       0.236
		Randomize      (-0.443, 0.639, 0.336)
		colorDistMagn   0
		colorDistFreq   0
		detailScale     1.64e+04
		tropicLatitude  0.95
		icecapLatitude  1.05
		climatePole     0.415
		climateTropic   0.112
		mareFreq        41.9
		mareDensity     0.0143
		erosion         1
		montesMagn      0.03
		montesFreq      537
		dunesMagn       0.5
		hillsMagn       0.03
		hillsFreq       2.86e+03
		canyonsMagn     0.288
		cracksOctaves   5
		BumpHeight      471
		BumpOffset      471
		SpecBrightWater 0
		SpecBrightIce   0
		RoughnessWater  0.35
		RoughnessIce    0.45
		SpecularScale   1
		RoughnessBias   0.5
		Hapke           0.5
		SpotBright      1.92
		SpotWidth       0.05
		DayAmbient      0.07
	}

	NoOcean         true

	NoClouds        true

	NoLava          true

	Atmosphere
	{
		Model          "Sun"
		Height          4661.18994
		Density         0
		Pressure        1
		Bright          10
		Opacity         0
		SkyLight        0
		Hue             0
		Saturation      1
	}

	NoAurora        true

	NoRings         true

	NoAccretionDisk true

	Corona
	{
		Radius      2.69e+06
		Period      0.0299
		Brightness  0.5
		RayDensity  3.2
		RayCurv     11.3
	}

	NoCometTail     true

	Orbit
	{
		RefPlane        "Static"
		Period          0
		SemiMajorAxis   0
		Eccentricity    0
		Inclination     0
		AscendingNode   0
		ArgOfPericenter 0
		MeanAnomaly     0
	}
}

