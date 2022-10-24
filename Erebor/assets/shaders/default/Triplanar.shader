Properties:
  Uniforms.UVTransform:
    Type: Drag
    Default: [1, 1, 0, 0]
  u_Albedo:
    Type: Texture
    Default: White
  u_Bump:
    Type: Normal
    Default: Bump
  u_Metallic:
    Type: Texture
    Default: Black
  u_Roughness:
    Type: Texture
    Default: White
  u_Occlussion:
    Type: Texture
    Default: White
  u_Emission:
    Type: Texture
    Default: Black
  Uniforms.EmissionColor:
    Type: HDR
    Default: [0, 0, 0]
Passes:
  GEOMETRY:
    Defines:
      - GEOMETRY
    Includes:
      - assets/shaders/default/Triplanar.glsl