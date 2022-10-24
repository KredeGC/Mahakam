Properties:
  Uniforms.UVTransform:
    Type: Drag
    Default: [1, 1, 0, 0]
  u_Albedo:
    Type: Texture
    Default: White
  Uniforms.Color:
    Type: Color
    Default: [1, 1, 1]
  u_Bump:
    Type: Normal
    Default: Bump
  Uniforms.MetallicMul:
    Type: Range
    Min: 0
    Max: 1
    Default: 0
  u_Roughness:
    Type: Texture
    Default: White
  Uniforms.RoughnessMul:
    Type: Range
    Min: 0
    Max: 1
    Default: 1
  Uniforms.EmissionColor:
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