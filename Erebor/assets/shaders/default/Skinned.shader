Properties:
  u_Color:
    Type: Color
    Default: [1, 1, 1]
  u_Metallic:
    Type: Range
    Min: 0
    Max: 1
    Default: 0
  u_Roughness:
    Type: Range
    Min: 0
    Max: 1
    Default: 0.5
Passes:
  GEOMETRY:
    Defines:
      - GEOMETRY
    Includes:
      - assets/shaders/default/Skinned.glsl
  SHADOW:
    Defines:
      - SHADOW
    Includes:
      - assets/shaders/default/Skinned.glsl