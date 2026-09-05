#include "composite.h"
#include "final.h"
#include "gbuffers_orbit.h"

#include <queue>
#include <stdexcept>
#include <string>
#include <vector>

#include <spdlog/spdlog.h>

BasicTableType      BASIC;
IdentTableType      IDENT;
OIDType             BarycenterID;
SystemType          SystemTable;

OrbitCharTableType  OrbitCharacteristicsTable;

void AllocateObjectID(const SETable& RootTable, BasicTableType* Output)
{
    spdlog::info("创建索引...");
    size_t i = 0;
    for (const auto& Obj : RootTable)
    {
        Output->insert({i, Obj});
        ++i;
    }
    spdlog::info("完成");
}

void InitObjectNames(const BasicTableType& BasicTable, IdentTableType* Output)
{
    spdlog::info("整理物体名称...");
    for (auto [OID, Table] : BasicTable)
    {
        std::vector<std::string> NameList;
        std::string NameText = Table.second.front().As<std::string>();
        auto NameView = NameText | std::views::split('/');
        for (auto i : NameView)
        {
            NameList.push_back(std::string(i.begin(), i.end()));
        }
        Output->insert({OID, NameList});
    }
    spdlog::info("完成");
}

bool NameContains(std::string Name, std::vector<std::string> NameList)
{
    return std::find(NameList.begin(), NameList.end(), Name) != NameList.end();
}

void __Find_Barycenter(const BasicTableType& BasicTable, const IdentTableType& IdentTable, OIDType* Barycenter)
{
    spdlog::info("查找系统根节点...");
    auto BarycenIt = std::find_if(IdentTable.begin(), IdentTable.end(), 
        [&BasicTable](const IdentTableType::value_type& x)
    {
        auto Table = BasicTable.at(x.first).second[1].As<SETable>();
        SEString ParentBody = Table.find("ParentBody")->second[0].As<SEString>();
        return NameContains(ParentBody, x.second);
    });
    if (BarycenIt == IdentTable.end()) {throw std::invalid_argument("行星系统没有根节点");}
    *Barycenter = BarycenIt->first;
    spdlog::info("行星系统根节点为：{}:{}", BarycenIt->second[0], BarycenIt->first);
}

void __BFS_Build(const BasicTableType& BasicTable, const IdentTableType& IdentTable, OIDType Barycenter, SystemType* Output)
{
    spdlog::info("重建系统（此步骤预计耗时较长，请耐心等待）...");

    auto __BFS_Build_Start = std::chrono::high_resolution_clock::now();

    int WaitSecond = 3;
    std::jthread __BFS_Build_Timer([WaitSecond, &BasicTable, Output](std::stop_token Stoken) 
    {
        while (!Stoken.stop_requested()) 
        {
            // 使用 stop_token 实现可中断的睡眠
            std::this_thread::sleep_for(std::chrono::seconds(WaitSecond));
            if (Stoken.stop_requested()) break;
            spdlog::info("重建系统（{}%）...", (100 * Output->size()) / BasicTable.size());
        }
    });

    // 预构建反向索引：查找所有带有相同ParentBody的OID并预挂载（只扫描一次表）
    std::unordered_map<std::string, std::vector<OIDType>> ParentIndex;
    ParentIndex.reserve(BasicTable.size());
    for (const auto& [OID, Table] : BasicTable)
    {
        const auto& SubTbl = Table.second[1].As<SETable>();
        SEString ParentBody = SubTbl.find("ParentBody")->second[0].As<SEString>();
        ParentIndex[ParentBody].push_back(OID);
    }

    std::queue<OIDType> OIDQueue;
    OIDQueue.push(Barycenter);
    while (!OIDQueue.empty())
    {
        OIDType CurrentOID = OIDQueue.front();
        OIDQueue.pop();
        auto IdentList = IdentTable.at(CurrentOID);
        std::vector<OIDType> SubSystems;
        for (const auto& Name : IdentList)
        {
            auto it = ParentIndex.find(Name);
            if (it == ParentIndex.end()){continue;}
            for (OIDType i : it->second)
            {
                if (i == Barycenter){continue;}
                SubSystems.push_back(i);
                OIDQueue.push(i);
            }
        }
        Output->insert({CurrentOID, SubSystems});
    }

    auto __BFS_Build_End = std::chrono::high_resolution_clock::now();
    auto __BFS_Build_Delay = std::chrono::duration_cast<std::chrono::seconds>(__BFS_Build_End - __BFS_Build_Start);

    spdlog::info("完成（已处理 {} 个物体，延迟：{} s）", Output->size(), __BFS_Build_Delay.count());
}

void RebuildSystem(const BasicTableType& BasicTable, const IdentTableType& IdentTable, OIDType* Barycenter, SystemType* Output)
{
    __Find_Barycenter(BasicTable, IdentTable, Barycenter);
    __BFS_Build(BasicTable, IdentTable, *Barycenter, Output);
}

void Composite()
{
    AllocateObjectID(RawData, &BASIC);
    InitObjectNames(BASIC, &IDENT);
    RebuildSystem(BASIC, IDENT, &BarycenterID, &SystemTable);

    OrbitCharacteristicsTable = gbuffer_orbit();
}