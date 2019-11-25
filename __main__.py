import ctypes


_devo = ctypes.CDLL("dist/devolib.so")
_devo.run_all.argtypes = (ctypes.c_uint,)


def run_all(seed):
    global _devo
    s = ctypes.c_uint(seed)
    _devo.run_all(s)


print(run_all(101))
