--dofile ("findOpenCL.lua")

if _OPTIONS["double"] then
    defines {"BT_USE_DOUBLE_PRECISION"}
end

if _OPTIONS["enable_multithreading"] then
    defines {"BT_THREADSAFE=1"}
end

include "../src/Bullet3Common"
include "../src/Bullet3Geometry"
include "../src/Bullet3Collision"
include "../src/Bullet3Dynamics"
--include "../src/Bullet3OpenCL"
include "../src/Bullet3Serialize/Bullet2FileLoader"

include "../src/BulletInverseDynamics"
include "../src/BulletSoftBody"
include "../src/BulletDynamics"
include "../src/BulletCollision"
include "../src/LinearMath"