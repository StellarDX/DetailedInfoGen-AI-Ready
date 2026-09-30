import os
from importlib.util import spec_from_file_location, module_from_spec
from pathlib import Path

def LoadMod(ModFile:Path):
    Spec = spec_from_file_location(ModFile.stem, ModFile)
    if Spec is None or Spec.loader is None:
        return
    Module = module_from_spec(Spec)
    Spec.loader.exec_module(Module)
    if hasattr(Module, "DLCMain") and callable(Module.DLCMain):
        Module.DLCMain()

def SearchMod(Mods:Path):
    if not Mods.exists():
        return
    for i in sorted(Mods.glob("*.py")):
        if i.name.startswith("_"):
            continue
        LoadMod(i)

SearchMod(Path("./InfoGen_Data/Plug-ins"))