Passes:
  DIRECTIONAL:
    Defines:
      - DIRECTIONAL
      - BILINEAR_SHADOWS
      - PCF_SHADOWS
    Includes:
      - internal/shaders/DeferredPerLight.glsl
  POINT:
    Defines:
      - POINT
    Includes:
      - internal/shaders/DeferredPerLight.glsl
  SPOT:
    Defines:
      - SPOT
      - BILINEAR_SHADOWS
      - PCF_SHADOWS
    Includes:
      - internal/shaders/DeferredPerLight.glsl