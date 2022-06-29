Properties:
  u_UVTransform:
    Type: Drag
    Default: [1, 1, 0, 0]
  u_Albedo:
    Type: Texture
    Default: White
  u_Color:
    Type: Color
    Default: [1, 1, 1]
  u_Bump:
    Type: Normal
    Default: Bump
  u_MetallicMul:
    Type: Range
    Min: 0
    Max: 1
    Default: 0
  u_Roughness:
    Type: Texture
    Default: White
  u_RoughnessMul:
    Type: Range
    Min: 0
    Max: 1
    Default: 1
  u_EmissionColor:
    Type: HDR
    Default: [0, 0, 0]
Passes:
  GEOMETRY:
    Defines:
      - GEOMETRY
      - USE_ALBEDO
      - USE_BUMP
      - USE_ROUGHNESS
    Includes:
      - assets/shaders/default/Lit.glsl