import ctypes

quadraticsolver = ctypes.CDLL('./quadricsolver.dll')
quadraticsolver.SolveQuadratic.argtypes = [
    ctypes.c_double,
    ctypes.c_double,
    ctypes.c_double,
    ctypes.POINTER(ctypes.c_double),
    ctypes.POINTER(ctypes.c_double),
]
quadraticsolver.SolveQuadratic.restype = ctypes.c_bool

a, b, c = map(float, input("Input coefs: ").split())
root1 = ctypes.c_double()
root2 = ctypes.c_double()

status = quadraticsolver.SolveQuadratic(a, b, c, ctypes.byref(root1), ctypes.byref(root2))

messages = {
    -1: "Incorrect input.",
    0: "No roots.",
    1: f"Root: {root1.value}",
    2: f"Roots: {root1.value}, {root2.value}"
}
print(messages.get(status, "Unknown error."))
