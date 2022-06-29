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
  u_Metallic:
    Type: Texture
    Default: Black
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
  u_Occlussion:
    Type: Texture
    Default: White
  u_Emission:
    Type: Texture
    Default: White
  u_EmissionColor:
    Type: HDR
    Default: [0, 0, 0]
Passes:
  GEOMETRY:
    Defines:
      - GEOMETRY
      - USE_ALBEDO
      - USE_BUMP
      - USE_METALLIC
      - USE_ROUGHNESS
      - USE_OCCLUSSION
      - USE_EMISSION
      - USE_STOCHASTIC
      - USE_TRIPLANAR
    Includes:
      - assets/shaders/default/Lit.glsl