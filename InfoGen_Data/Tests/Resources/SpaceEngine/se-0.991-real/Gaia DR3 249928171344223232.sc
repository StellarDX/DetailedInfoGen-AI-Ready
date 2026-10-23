Planet	"6"
{
	ParentBody     "Gaia DR3 249928171344223232"
	Class          "Terra"
	Mass            1.24588871
	Radius          7294.96631
	InertiaMoment   0.331838697
	Oblateness      0.00932314433
	RotationPeriod  15.7425047
	RotationEpoch   0
	Obliquity       -36.3621255
	EqAscendNode    95.4095808

	AlbedoBond      0.594
	AlbedoGeom      0.36
	Brightness      1
	BrightnessReal  1

	Life
	{
		Class   "Organic"
		Type    "Unicellular"
		Biome   "Marine"
	}

	Surface
	{
		Preset         "terra_wet_earth_green.cfg"
		SurfStyle       0.337
		Randomize      (-0.196, -0.098, 0.693)
		colorDistMagn   0.095
		colorDistFreq   839
		detailScale     3.28e+04
		drivenDarkening 0
		seaLevel        0.0623
		snowLevel       0.85
		tropicLatitude  0.608
		icecapLatitude  2
		icecapHeight    0.453
		climatePole     0.562
		climateTropic   0.562
		climateEquator  0.562
		climateSteppeMin -1
		climateSteppeMax -1
		climateForestMin -1
		climateForestMax -1
		climateGrassMin  -1
		climateGrassMax  -1
		humidity        0.296
		heightTempGrad  0.375
		beachWidth      0.00047
		tropicWidth     0.5
		mainFreq        1.11
		venusFreq       1.48
		venusMagn       0
		mareFreq        5.73
		mareDensity     0
		terraceProb     0.338
		erosion         0.111
		montesMagn      0.178
		montesFreq      300
		montesSpiky     0.917
		montesFraction  0.789
		dunesMagn       0.0247
		dunesFreq       53.9
		dunesFraction   0.343
		hillsMagn       0.112
		hillsFreq       125
		hillsFraction   0.421
		hills2Fraction  0.495
		riversMagn      59.3
		riversFreq      2.42
		riversSin       6
		riftsMagn       0
		riftsFreq       2.6
		riftsSin        6.14
		canyonsMagn     0.0343
		canyonsFreq     216
		canyonsFraction 0.434
		cracksMagn      0.121
		cracksFreq      0.536
		cracksOctaves   0
		craterMagn      0.584
		craterFreq      19.5
		craterDensity   0
		craterOctaves   0
		volcanoMagn     0.739
		volcanoFreq     0.552
		volcanoDensity  0.4
		volcanoOctaves  3
		volcanoActivity 2
		volcanoFlows    0.711
		volcanoRadius   0.468
		volcanoTemp     293
		lavaCoverTidal  0
		lavaCoverSun    0
		lavaCoverYoung  0
		stripeZones     2.12
		stripeTwist     0.123
		cycloneMagn     3.16
		cycloneFreq     0.569
		cycloneDensity  0.313
		cycloneOctaves  2
		BumpHeight      20
		BumpOffset      1.25
		DiffMapAlpha   "Water"
		SpecBrightWater 0.65
		SpecBrightIce   0.85
		RoughnessWater  0.35
		RoughnessIce    0.45
		SpecularScale   1
		RoughnessBias   0.5
		Hapke           0.5
		SpotBright      1.5
		SpotWidth       0.05
		DayAmbient      0.07
	}

	Ocean
	{
		Depth           1.25
		Hapke           0
		SpotBright      2
		SpotWidth       0.05
		DayAmbient      0.07
		ModulateBright  1

		Composition
		{
			CO2       	96.9
			SO2       	3.08
		}
	}

	Clouds
	{
		Height          34.2
		Velocity        67.1
		BumpHeight      7.37
		Hapke           0
		SpotBright      2
		SpotWidth       0.03
		DayAmbient      0.07
		ModulateBright  1
		mainFreq        0.592
		mainOctaves     10
		Coverage        0.1
		stripeZones     2.12
		stripeTwist     0.123
	}

	NoLava          true

	Atmosphere
	{
		Model          "Thick"
		Height          92.560791
		Density         129.577225
		Pressure        90.1595764
		Greenhouse      170.916092
		Bright          10
		Opacity         1
		SkyLight        3.33
		Hue             -0.48
		Saturation      0.829

		Composition
		{
			N2        	67.6
			CO2       	28.7
			O2        	2.82
			H2O       	0.525
			SO2       	0.395
			CO        	0.00781
			Ar        	0.00106
			Ne        	6.26e-05
		}
	}

	Climate
	{
		GlobalWindSpeed  1.98
	}

	Aurora
	{
		Height         40.3
		NorthLat       74.8
		NorthLon       13.1
		NorthRadius    1.98e+03
		NorthWidth     697
		NorthRings     4
		NorthBright    0.398
		NorthFlashFreq 69
		NorthMoveSpeed 1.15
		NorthParticles 50000
		SouthLat       -66.6
		SouthLon       173
		SouthRadius    2.06e+03
		SouthWidth     513
		SouthRings     3
		SouthBright    0.398
		SouthFlashFreq 65.2
		SouthMoveSpeed 1.16
		SouthParticles 50000
		TopColor    (1.000 1.000 1.000)
		BottomColor (0.000 1.000 0.000)
	}

	NoRings         true

	NoAccretionDisk true

	NoCometTail     true

	Orbit
	{
		RefPlane        "Equator"
		Period          18.2643325
		SemiMajorAxis   7.66268158
		Eccentricity    0.0156478819
		Inclination     1.07197981
		AscendingNode   93.7362255
		ArgOfPericenter 334.672888
		MeanAnomaly     168.51071
	}
}

