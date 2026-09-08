#include "gbuffers_basic.h"
#include "AdvMath/Integral.h"
#include "CONSTANTS.INC"
#include "Mini-NeoCSE/ISCStream.h"
#include "composite.h"
#include "composite1.h"

#include <limits>

#include <pybind11/pytypes.h>

PhysicalTableType PhysicalTable;

void LoadRadiuses(const SETable& Data, OIDType CurrentID, PhysicalCharacteristics* Table)
{
    Table->Dimensions.setConstant(GetObjectS(Data, "Radius", 0, 
        std::numeric_limits<double>::quiet_NaN()) * Km * 2.);
    if (isnan(Table->Dimensions.x()))
    {
        Table->Dimensions.setConstant(GetObjectS(Data, "RadSol", 0, 
            std::numeric_limits<double>::quiet_NaN()) * SolarRadius * 2.);
    }

    Table->Flattening.setZero();
    Table->Flattening.y() = GetObjectS(Data, "Oblateness", 0, 
        std::numeric_limits<double>::quiet_NaN());
    if (isnan(Table->Flattening.y()))
    {
        Table->Flattening = GetObjectS(Data, "Oblateness", 0, SEVec3::Zero());
    }

    Table->Dimensions = Table->Dimensions - Table->Dimensions * Table->Flattening;

    if (isnan(Table->Dimensions.x()))[[unlikely]]
    {
        Table->Dimensions = GetObjectS(Data, "Dimensions", 0, 
            SEVec3::Constant(std::numeric_limits<double>::quiet_NaN())) * Km;
        if (isnan(Table->Dimensions.x()))
        {
            Table->Dimensions = GetObjectS(Data, "DimensionsSol", 0, 
                SEVec3::Constant(std::numeric_limits<double>::quiet_NaN())) * SolarRadius;
        }

        Table->Flattening = 1. - (Table->Dimensions / Table->Dimensions.maxCoeff()).array();
    }
}

double EquatorialCircumference(SEVec3 Dimensions)
{
    double a = std::max(Dimensions.x(), Dimensions.z()) / 2.;
    double b = std::min(Dimensions.x(), Dimensions.z()) / 2.;
    if (a == b) {return 2. * M_PI * a;}

    double e2 = 1. - pow(b / a, 2);
    auto E = [e2](double tet)
    {
        return sqrt(1. - e2 * pow(sin(tet), 2));
    };
    return 4. * a * GaussKronrodQuadrature()(E, 0, M_PI_2);
}

double MeridionalCircumference(SEVec3 Dimensions)
{
    double a = std::max(Dimensions.x(), Dimensions.z()) / 2.;
    double b = Dimensions.y();
    if (a == b) {return 2. * M_PI * a;}

    double e2 = 1. - pow(b / a, 2);
    auto E = [e2](double tet)
    {
        return sqrt(1. - e2 * pow(sin(tet), 2));
    };
    return 4. * a * GaussKronrodQuadrature()(E, 0, M_PI_2);
}

double SurfaceArea(SEVec3 Dimensions)
{
    SEVec3 Radius = Dimensions / 2.;
    std::sort(Radius.begin(), Radius.end());
    double a = Radius[2], b = Radius[1], c = Radius[0];
    if (a == b && b == c) {return 4. * M_PI * a * a;}

    auto F = [](double phi, double k2)
    {
        auto EllipticalIntegral1st = [k2](double tet)
        {
            return 1. / sqrt(1. - k2 * pow(sin(tet), 2));
        };
        return GaussKronrodQuadrature()(EllipticalIntegral1st, 0, phi);
    };

    auto E = [](double phi, double k2)
    {
        auto EllipticalIntegral2nd = [k2](double tet)
        {
            return sqrt(1. - k2 * pow(sin(tet), 2));
        };
        return GaussKronrodQuadrature()(EllipticalIntegral2nd, 0, phi);
    };

    double phi = acos(c / a);
    double k2 = (a * a * (b * b - c * c)) / (b * b * (a * a - c * c));
    return 2. * M_PI * c * c + (2. * M_PI * a * b) / sin(phi) *
        (E(phi, k2) * pow(sin(phi), 2) + F(phi, k2) * pow(cos(phi), 2));
}

void ComputeRadiusesRelativeParams(PhysicalCharacteristics* Table)
{
    Table->MeanRadius = cbrt(Table->Dimensions.x() * Table->Dimensions.y() * Table->Dimensions.z());
    Table->Circumference = 
    {
        EquatorialCircumference(Table->Dimensions),
        MeridionalCircumference(Table->Dimensions)
    };
    Table->SurfaceArea = SurfaceArea(Table->Dimensions);
    Table->Volume = (4. / 3.) * M_PI * Table->Dimensions.x() * Table->Dimensions.y() * Table->Dimensions.z();
}

void LoadStar(const BasicTableType& BasicTable, const SystemType& System, OIDType CurrentID, PhysicalCharacteristics* Table)
{
    auto RawData = BasicTable.at(CurrentID).second[1].As<SETable>();
    Table->Class = GetObjectS(RawData, "Class", 0, "None");
    LoadRadiuses(RawData, CurrentID, Table);
    ComputeRadiusesRelativeParams(Table);
}

void LoadPhysicalParamsFromRawData(const BasicTableType& BasicTable, const SystemType& System, OIDType Barycenter, PhysicalTableType* Output)
{
    for (auto [OID, Data] : BasicTable)
    {
        PhysicalCharacteristics Table;
        if (Data.first == "Star")
        {
            LoadStar(BasicTable, System, OID, &Table);
        }
        // TODO...
    }
}

PhysicalCharTableType gbuffer_basic()
{
    LoadPhysicalParamsFromRawData(BASIC, SystemTable, BarycenterID, &PhysicalTable);

    return PhysicalCharTableType(); // TODO
}