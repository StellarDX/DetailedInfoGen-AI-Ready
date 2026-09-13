import re

def RegexStellarClassification(OClass:str):
    if OClass[0] in "OBAFGKM" and OClass[0:1] != "MS":
        # 此处使用SE的标准，兼容性非常低，真实光谱字符串会比这复杂的多
        SpectralRegex = r"^(?P<Sp>O|B|A|F|G|K|M)(?P<Sub>([0-9](\.[0-9])?)|10)? ?(?P<Lum>0|Ia|Iab|Ib|II|III|IV|V|VI)?$"
        SMatch = re.search(SpectralRegex, OClass).groupdict()
        Color = ""
        Lum = ""
        match SMatch["Lum"]:
            case "VI":
                Lum = "次矮星"
            case "V":
                Lum = "主序星"
            case "IV":
                Lum = "亚巨星"
            case "III":
                Lum = "巨星"
            case "II":
                Lum = "亮巨星"
            case "Ib":
                Lum = "超巨星"
            case "Iab":
                Lum = "超巨星"
            case "Ia":
                Lum = "超巨星"
            case "0":
                Lum = "特超巨星"
        match SMatch["Sp"]:
            case "O":
                if SMatch["Lum"] == "VI" or SMatch["Lum"] == "V" or SMatch["Lum"] == "IV":
                    Color = "O型"
                else:
                    Color = "蓝"
            case "B":
                if SMatch["Lum"] == "VI" or SMatch["Lum"] == "V" or SMatch["Lum"] == "IV":
                    Color = "B型"
                else:
                    Color = "蓝"
            case "A":
                if SMatch["Lum"] == "VI" or SMatch["Lum"] == "V" or SMatch["Lum"] == "IV":
                    Color = "A型"
                else:
                    Color = "蓝"
            case "F":
                if SMatch["Lum"] == "VI" or SMatch["Lum"] == "V" or SMatch["Lum"] == "IV":
                    Color = "F型"
                else:
                    Color = "黄"
            case "G":
                if SMatch["Lum"] == "VI" or SMatch["Lum"] == "IV":
                    Color = "G型"
                else:
                    Color = "黄"
                if SMatch["Lum"] == "V":
                    Lum = "矮星"
            case "K":
                if SMatch["Lum"] == "V":
                    Color = "橙"
                elif SMatch["Lum"] == "VI" or SMatch["Lum"] == "IV":
                    Color = "K型"
                else:
                    Color = "红"
                if SMatch["Lum"] == "V":
                    Lum = "矮星"
            case "M":
                if SMatch["Lum"] == "VI" or SMatch["Lum"] == "IV":
                    Color = "M型"
                else:
                    Color = "红"
                if SMatch["Lum"] == "V":
                    Lum = "矮星"
        return Color + Lum
    if OClass[0] in "LTY":
        return "褐矮星"
    if OClass[0] == "W" and OClass[0:1] != "WD":
        return "沃尔夫-拉叶星"
    if OClass[0] == "C" or OClass[0:1] == "SC":
        return "碳星"
    if OClass[0] == "S" or OClass[0:1] == "MS":
        return "S型恒星"
    if OClass[0] == "D" or OClass[0:1] == "WD":
        return "白矮星"
    if OClass == "Q":
        return "中子星"
    if OClass == "X":
        return "黑洞"
    if OClass == "Z":
        return "虫洞"
    return "特殊的恒星"

def PlanetClassification(OClass:str, Physical:dict, Ocean:dict = None):
    EarthMass = 5.9721684E+24
    EarthRadius = 6378137
    Mass = Physical["Mass"]
    Radius = Physical["MeanRadius"]
    Temperature = Physical["Temperature"]
    match OClass:
        case "Ferria":
            return "铁质行星"
        case "Terra":
            if Mass > 10 * EarthMass: # 对应2.1~5地球半径
                return "Mega Earth" # 直接翻译有些别扭
            elif Mass > 1.9 * EarthMass:
                return "超级地球"
            elif Radius < 0.8 * EarthRadius:
                return "Sub Earth"
            else:
                return "岩质行星"
        case "Carbonia":
            return "碳质行星"
        case "Aquaria":
            if Ocean != None and len(Ocean) != 0:
                if Ocean["Height"] > 10:
                    return "海洋行星"
                else:
                    return "水质行星"
            else:
                return "冰质行星"
        case "Neptune" | "IceGiant":
            if Temperature > 600: # 目前没有一个界定的标准
                return "Hoptune"
            elif Radius < 4 * EarthRadius:
                return "迷你海王星"
            else:
                return "冰巨星"
        case "Jupiter" | "GasGiant": # 气态行星采用Sudarsky的气态行星分类标准
            if Temperature < 150:
                return "1类（氨云）气态行星"
            elif Temperature < 350: # 250
                return "2类（水云）气态行星"
            elif Temperature < 800:
                return "3类（无云）气态行星"
            elif Temperature < 1400:
                return "4类（碱云）气态行星"
            else:
                return "5类（硅云）气态行星"
        case "Chthonia" | "HeliumGiant":
            return "氦行星"
    return "未分类"