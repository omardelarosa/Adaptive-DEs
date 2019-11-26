import glob
import os

_BUILD_PATH = "build"
# _LIB_PATH = glob.glob(BUILD_PATH + "/**/DE.*.so", recursive=True)[0]

# # DLL path
_DE_SO_PATH = 'DE.*.so'
# # the absolute path to the C shared object library
_LIB_PATH = os.path.join(_BUILD_PATH, "**", _DE_SO_PATH)

print(glob.glob(_LIB_PATH, recursive=True))
