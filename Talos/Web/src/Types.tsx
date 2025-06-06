export interface Data {
  accel_x: number;
  accel_y: number;
  accel_z: number;
  avg_accel: number;
  vel_x: number;
  vel_y: number;
  vel_z: number;
  avg_vel: number;
  angle_x: number;
  angle_y: number;
  angle_z: number;
  temperature: number;
  pressure: number;
  time: number; // In seconds
}

export enum Axis {
  TOP,
  FRONT,
  SIDE,
}
