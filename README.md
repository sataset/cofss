# cofss
C++ based optical fiber system simulator

# DSLaser (cofss)

Structure:
- Core
 - System
  - Modules container
 - Module
  - Abstract class to define other modules easily and give them OOP features
 - Field
  - dt, df, dw, f, w, fft, ifft
  - peak / average power
  - Polarizations
   - right, left elliptic polarizations
   - linear and orthogonal polarizations
 - Executor (in progress)
  - Singleton pattern to create decentralized system for easier lazy / parallel processing
- Modules
 - Fiber
 - TDFA (bad model. Needs fixing)
 - QWP + HWP
 - PD ISO / PBS
 - DWNT SA (Simple model)
 - Coupler
 - Counter (needs system upgrade)
 - Logger (part of Coupler module)
- Utility (converters and additional functions)
- Modulations (currently only sech)
