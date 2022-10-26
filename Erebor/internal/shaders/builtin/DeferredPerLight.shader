Passes:
  DIRECTIONAL:
    Defines:
      - DIRECTIONAL
      - BILINEAR_SHADOWS
      - PCF_SHADOWS
    Includes:
      - internal/shaders/builtin/DeferredPerLight.glsl
  POINT:
    Defines:
      - POINT
    Includes:
      - internal/shaders/builtin/DeferredPerLight.glsl
  SPOT:
    Defines:
      - SPOT
      - BILINEAR_SHADOWS
      - PCF_SHADOWS
    Includes:
      - internal/shaders/builtin/DeferredPerLight.glsl