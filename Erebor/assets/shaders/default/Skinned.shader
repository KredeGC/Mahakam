Properties:
  u_Color:
    Type: Color
    Default: [1, 1, 1]
  u_MetallicMul:
    Type: Range
    Min: 0
    Max: 1
    Default: 0
  u_RoughnessMul:
    Type: Range
    Min: 0
    Max: 1
    Default: 0.5
  u_EmissionColor:
    Type: HDR
    Default: [0, 0, 0]
Passes:
  GEOMETRY:
    Defines:
      - GEOMETRY
      - SKIN
    Includes:
      - assets/shaders/default/Lit.glsl
  SHADOW:
    Defines:
      - SHADOW
      - SKIN
    Includes:
      - assets/shaders/default/Lit.glsl