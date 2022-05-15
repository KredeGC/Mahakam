Properties:
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
Passes:
  GEOMETRY:
    Defines:
      - GEOMETRY
    Includes:
      - assets/shaders/default/LitTexture.glsl