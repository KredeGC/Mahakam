Properties:
  Uniforms.Color:
    Type: Color
    Default: [1, 1, 1]
  Uniforms.MetallicMul:
    Type: Range
    Min: 0
    Max: 1
    Default: 0
  Uniforms.RoughnessMul:
    Type: Range
    Min: 0
    Max: 1
    Default: 0.5
  Uniforms.EmissionColor:
    Type: HDR
    Default: [0, 0, 0]
Passes:
  GEOMETRY:
    Defines:
      - GEOMETRY
    Includes:
      - internal/shaders/default/Lit.glsl