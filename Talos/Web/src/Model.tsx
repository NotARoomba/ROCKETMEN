// use threejs and react-three-fiber to create a 3D model of a line over a 3d plane and rotate it based on the x, y, and z angles

import { Canvas } from "@react-three/fiber";
import { Suspense, useEffect, useRef } from "react";
import { Camera, Euler, Group, Line, Object3DEventMap, Vector3 } from "three";
import { CameraControls, CubeCamera, OrbitControls } from "@react-three/drei";
import Rocket from "./Rocket";
import { Axis, Data } from "./Types";

export default function Model({ data, axis }: { data: Data; axis: Axis }) {
  if (data === undefined) {
    return <div className="m-auto">Loading...</div>;
  }
  const unitVector =
    axis == Axis.TOP
      ? new Vector3(0, 1, 0)
      : axis == Axis.FRONT
        ? new Vector3(0, 0, 1)
        : new Vector3(1, 0, 0);
  const vector = new Vector3(
    (data.angle_x * Math.PI) / 180,
    (data.angle_y * Math.PI) / 180,
    (data.angle_z * Math.PI) / 180,
  );

  return (
    //the camera should be in the top view
    <div className="w-full h-full">
      <Canvas
        camera={{
          position: new Vector3().multiplyVectors(
            new Vector3(2, 2, 2),
            unitVector,
          ),
          //   rotation: new Euler(180, 0, 0),
        }}
      >
        <ambientLight />
        <pointLight position={[10, 10, 10]} />
        <axesHelper rotation={[0, Math.PI / 2, 0]} />

        <Rocket
          rotation={
            new Euler(
              ...new Vector3().addVectors(new Vector3(Math.PI, 0, 0), vector),
            )
          }
          scale={0.018}
        />
        <OrbitControls rotation={[0, 0, 0]} />
      </Canvas>
    </div>
  );
}
