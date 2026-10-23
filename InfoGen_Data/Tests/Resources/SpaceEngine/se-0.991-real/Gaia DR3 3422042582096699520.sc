Star	"Gaia DR3 3422042582096699520"
{
	ParentBody     "Gaia DR3 3422042582096699520"
	Class          "F8 V"
	Luminosity      2.2654407
	LumBol          2.18349743
	MassSol         1.21559298
	RadSol          1.19348013
	Teff            6200

	Age             3.39

	InertiaMoment   0.0469008386
	Oblateness      0.0199380293
	RotationPeriod  14.5502286
	RotationEpoch   0
	Obliquity       322.625854
	EqAscendNode    194.519852

	AlbedoBond      0.3
	AlbedoGeom      0.36
	Brightness      1
	BrightnessReal  1
	Color          (1.000 0.916 0.922)

	Surface
	{
		SurfStyle       0.54
		Randomize      (0.792, -0.930, -0.573)
		colorDistMagn   0
		colorDistFreq   0
		detailScale     3.28e+04
		tropicLatitude  0.95
		icecapLatitude  1.05
		climatePole     0.301
		climateTropic   0.0833
		mareFreq        43.1
		mareDensity     0.0087
		erosion         1
		montesMagn      0.03
		montesFreq      537
		dunesMagn       0.5
		hillsMagn       0.03
		hillsFreq       4.74e+03
		canyonsMagn     0.307
		cracksOctaves   5
		BumpHeight      650
		BumpOffset      650
		SpecBrightWater 0
		SpecBrightIce   0
		RoughnessWater  0.35
		RoughnessIce    0.45
		SpecularScale   1
		RoughnessBias   0.5
		Hapke           0.5
		SpotBright      2.49
		SpotWidth       0.05
		DayAmbient      0.07
	}

	NoOcean         true

	NoClouds        true

	NoLava          true

	Atmosphere
	{
		Model          "Sun"
		Height          8303.04102
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
		Radius      4.48e+06
		Period      0.0212
		Brightness  0.5
		RayDensity  3.15
		RayCurv     17.2
	}

	NoCometTail     true

	Orbit
	{
		RefPlane        "Static"
		Period          0
		SemiMajorAxis   0
		Eccentricity    0
		Inclination     322.625854
		AscendingNode   194.519852
		ArgOfPericenter 0
		MeanAnomaly     0
	}
}

