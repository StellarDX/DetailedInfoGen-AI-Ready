from InfoGen_Data import Generate
import os

if __name__ == '__main__':
    print(os.getpid())
    input("Press Enter to attach...")
    Generate.Generate()