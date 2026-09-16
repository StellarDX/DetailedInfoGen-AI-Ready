import chromadb
from langchain_chroma import Chroma
from langchain_openai import OpenAIEmbeddings

import tomllib

from pathlib import Path

ChromaDBPersistDirectory = "./IGAppData/Data"
_ChromaCli = None

EmbedConfigFile = "./InfoGen_Data/Config/Embedding.toml"
_EmbedCli = None

def ChromaClient():
    global _ChromaCli
    if _ChromaCli == None:
        _ChromaCli = chromadb.PersistentClient(path = ChromaDBPersistDirectory)
    return _ChromaCli

def EmbeddingClient():
    global _EmbedCli
    if _EmbedCli == None:
        with Path(EmbedConfigFile).open(mode = "rb") as ConfigFile:
            Config = tomllib.load(ConfigFile)
        _EmbedCli = OpenAIEmbeddings(**Config)
    return _EmbedCli

def _GetNamespace(Name:str):
    pass

def _CreateNamespace(Name:str):
    Client = ChromaClient()
    Client.get_or_create_collection(Name)
    print(f"命名空间\"{Name}\"已创建")

def CreateNamespace(args):
    _CreateNamespace(args.value)
    pass