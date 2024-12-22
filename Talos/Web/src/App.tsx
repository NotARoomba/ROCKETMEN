import { Graphics, Stage } from "@pixi/react";
import { useCallback, useEffect, useState } from "react";
import { DarkModeSwitch } from "react-toggle-dark-mode";
import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  ResponsiveContainer,
} from "recharts";
import { fromEvent, interval, buffer, Observable, bufferTime } from "rxjs";
import Model from "./Model";
import { velocity } from "three/tsl";
import { Data, Axis } from "./Types";

const API_URL = "ws://localhost:3001";
const TEST = true;

export default function App() {
  const [data, setData] = useState<Data[]>([]);
  const [isConnected, setIsConnected] = useState(false);
  const [velocityData, setVelocityData] = useState<
    { velocity: number; time: string }[]
  >([]);
  const [accelerationData, setAccelerationData] = useState<
    { acceleration: number; time: string }[]
  >([]);
  const [isDarkMode, setDarkMode] = useState(false);

  const toggleDarkMode = (checked: boolean) => {
    setDarkMode(checked);
    document.body.classList.toggle("dark");
  };

  useEffect(() => {
    // Create the WebSocket connection
    const socket = new WebSocket(API_URL);

    // Observable for WebSocket messages
    const message$ = new Observable<any>((observer) => {
      socket.onmessage = (message) => {
        observer.next(JSON.parse(message.data));
      };
      socket.onerror = (error) => observer.error(error);
      socket.onclose = () => {
        observer.complete();
        console.log("Disconnected from the server");
        setIsConnected(false);
      };

      return () => socket.close();
    });

    // Buffer incoming data and update state every second
    const buffered$ = message$.pipe(bufferTime(1000));
    const subscription = buffered$.subscribe((bufferedData) => {
      if (bufferedData.length > 0) {
        console.log(bufferedData);
        setData((prevData) => [...prevData, ...bufferedData]);
      }
    });

    socket.onopen = () => {
      console.log("Connected to the server");
      setIsConnected(true);
    };

    // add test data every 10 ms and base the angle on sine and cosine functions
    if (TEST) {
      const startDate = Date.now();
      setInterval(() => {
        setData((prevData) => [
          ...prevData,
          {
            accel_x: 1 + 0.000005 * (Date.now() - startDate),
            accel_y: 1 + 0.000005 * (Date.now() - startDate),
            accel_z: 1 + 0.000005 * (Date.now() - startDate),
            vel_x:
              prevData.length > 0
                ? prevData[prevData.length - 1].vel_x +
                  prevData[prevData.length - 1].accel_x
                : 0,
            vel_y:
              prevData.length > 0
                ? prevData[prevData.length - 1].vel_y +
                  prevData[prevData.length - 1].accel_y
                : 0,
            vel_z:
              prevData.length > 0
                ? prevData[prevData.length - 1].vel_z +
                  prevData[prevData.length - 1].accel_z
                : 0,
            angle_x: (Math.sin(Date.now() / 1000) * 180) / Math.PI,
            angle_y: 0,
            angle_z: (Math.cos(Date.now() / 1000) * 180) / Math.PI,
            temperature: 22 + 0.00002 * (Date.now() - startDate),
            //decrease the pressure exponentially
            pressure:
              (101325 * Math.exp(-0.0000005 * (Date.now() - startDate))) / 1000,
            time: Date.now(),
          },
        ]);
      }, 100);
    }

    return () => subscription.unsubscribe();
  }, []);

  // Derive velocity and acceleration from the data
  //update every second

  useEffect(() => {
    if (data.length > 0 && data.length % 3 == 0) {
      const accelerationData = data.map((d) => ({
        time: `${(d.time - data[0].time) / 1000}`,
        acceleration: Math.sqrt(
          d.accel_x ** 2 + d.accel_y ** 2 + d.accel_z ** 2
        ),
      }));
      setAccelerationData(accelerationData);
    }

    //updat the velocity data so that it is the integral of the acceleration data using a for loop
    let velocityData: { velocity: number; time: string }[] = [];
    for (let i = 0; i < accelerationData.length; i++) {
      if (i === 0) {
        velocityData.push({
          time: accelerationData[i].time,
          velocity: 0,
        });
      } else {
        velocityData.push({
          time: accelerationData[i].time,
          velocity:
            velocityData[i - 1].velocity +
            ((accelerationData[i].acceleration +
              accelerationData[i - 1].acceleration) /
              2) *
              0.01,
        });
      }
    }
    setVelocityData(velocityData);
  }, [data]);

  // const accelerationData = data.map((d) => ({
  //   time: `${(d.time - data[0].time) / 1000}`,
  //   acceleration: Math.sqrt(d.accel_x ** 2 + d.accel_y ** 2 + d.accel_z ** 2),
  // }));
  // //updat the velocity data so that it is the integral of the acceleration data using a for loop
  // let velocityData: { velocity: number; time: string }[] = [];
  // for (let i = 0; i < accelerationData.length; i++) {
  //   if (i === 0) {
  //     velocityData.push({
  //       time: accelerationData[i].time,
  //       velocity: 0,
  //     });
  //   } else {
  //     velocityData.push({
  //       time: accelerationData[i].time,
  //       velocity:
  //         velocityData[i - 1].velocity +
  //         ((accelerationData[i].acceleration +
  //           accelerationData[i - 1].acceleration) /
  //           2) *
  //           0.01,
  //     });
  //   }
  // }
  return (
    <div className="h-[100vh] w-full bg-anti-flash_white dark:bg-eerie_black text-eerie_black dark:text-french_gray grid grid-rows-3 grid-cols-3 p-8 gap-6">
      <div className="flex gap-x-6 col-span-2">
        <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl w-1/4 p-4 flex flex-col">
          <div className="m-auto flex flex-col">
            <p className="m-auto text-2xl">X</p>
            <p className="m-auto text-4xl">
              {data.length > 0 ? data[data.length - 1].angle_x.toFixed(2) : 0}°
            </p>
          </div>
          <div className="m-auto flex flex-col">
            <p className="m-auto text-2xl">Y</p>
            <p className="m-auto text-4xl">
              {data.length > 0 ? data[data.length - 1].angle_y.toFixed(2) : 0}°
            </p>
          </div>
          <div className="m-auto flex flex-col">
            <p className="m-auto text-2xl">Z</p>
            <p className="m-auto text-4xl">
              {data.length > 0 ? data[data.length - 1].angle_z.toFixed(2) : 0}°
            </p>
          </div>
        </div>
        <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl w-1/4 p-4 flex flex-col">
          <p className="text-4xl mx-auto">Top</p>
          <Model data={data[data.length - 1]} axis={Axis.TOP} />
        </div>
        <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl w-1/4 p-4 flex flex-col">
          <p className="text-4xl mx-auto">Front</p>
          <Model data={data[data.length - 1]} axis={Axis.FRONT} />
        </div>
        <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl w-1/4 p-4 flex flex-col">
          <p className="text-4xl mx-auto">Side</p>
          <Model data={data[data.length - 1]} axis={Axis.SIDE} />
        </div>
      </div>
      <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl h-3/4 p-4 flex flex-col">
        <p className="text-4xl mx-auto mb-4">Acceleration</p>
        <p className="text-7xl m-auto">
          {accelerationData.length > 0
            ? accelerationData[
                accelerationData.length - 1
              ].acceleration.toFixed(2)
            : 0}{" "}
          m/s²
        </p>
      </div>
      <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl h-3/4 p-4 flex flex-col">
        <p className="text-4xl mx-auto mb-4">Velocity</p>
        <p className="text-7xl m-auto">
          {velocityData.length > 0
            ? velocityData[velocityData.length - 1].velocity.toFixed(2)
            : 0}{" "}
          m/s
        </p>
      </div>
      <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl text-9xl flex">
        <p className="m-auto">Talos</p>
      </div>
      <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl -translate-y-[20%] h-[125%] flex">
        {isConnected || TEST ? (
          <ResponsiveContainer width="100%" height="100%">
            <LineChart
              width={500}
              height={300}
              data={accelerationData}
              margin={{
                top: 30,
                right: 35,
                left: 20,
                bottom: 20,
              }}
            >
              <CartesianGrid strokeDasharray="3 3" />
              <XAxis
                dataKey="time"
                label={{
                  value: `Time`,
                  style: { textAnchor: "middle" },
                  position: "bottom",
                  offset: 0,
                }}
              />
              <YAxis
                label={{
                  value: `Acceleration (m/s^2)`,
                  style: { textAnchor: "middle" },
                  angle: -90,
                  position: "left",
                  offset: 0,
                }}
              />
              <Tooltip />
              <Line
                type="monotone"
                dataKey="acceleration"
                stroke="#7B8CDE"
                activeDot={{ r: 4 }}
                dot={false}
              />
            </LineChart>
          </ResponsiveContainer>
        ) : (
          <p className="text-6xl m-auto text-amaranth">No Data</p>
        )}
      </div>
      <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl h-[125%] flex -translate-y-[20%]">
        {isConnected || TEST ? (
          <ResponsiveContainer width="100%" height="100%">
            <LineChart
              width={500}
              height={300}
              data={velocityData}
              margin={{
                top: 30,
                right: 35,
                left: 20,
                bottom: 20,
              }}
            >
              <CartesianGrid strokeDasharray="3 3" />
              <XAxis
                dataKey="time"
                label={{
                  value: `Time`,
                  style: { textAnchor: "middle" },
                  position: "bottom",
                  offset: 0,
                }}
              />
              <YAxis
                label={{
                  value: `Velocity (m/s)`,
                  style: { textAnchor: "middle" },
                  angle: -90,
                  position: "left",
                  offset: 0,
                }}
              />
              <Tooltip />
              <Line
                type="monotone"
                dataKey="velocity"
                stroke="#7B8CDE"
                activeDot={{ r: 4 }}
                dot={false}
              />
            </LineChart>
          </ResponsiveContainer>
        ) : (
          <p className="text-6xl m-auto text-amaranth">No Data</p>
        )}
      </div>
      <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl w-[125%] text-5xl flex">
        <p
          className={
            "m-auto text-center " +
            (isConnected || TEST ? " text-sea_green" : " text-amaranth")
          }
        >
          {isConnected || TEST ? "Status Connected" : "Status Disconnected"}
        </p>
      </div>
      <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl translate-x-1/3 w-3/4 p-4 flex flex-col gap-y-4">
        <div className="m-auto flex flex-col">
          <p className="m-auto text-2xl">Temperature</p>
          <p className="m-auto text-6xl">
            {data.length > 0 ? data[data.length - 1].temperature.toFixed(1) : 0}
            ° C
          </p>
        </div>
        <div className="m-auto flex flex-col">
          <p className="m-auto text-2xl">Pressure</p>
          <p className="m-auto text-6xl">
            {data.length > 0 ? data[data.length - 1].pressure.toFixed(1) : 0}{" "}
            kPa
          </p>
        </div>
      </div>
      <div className="absolute top-2 right-2">
        <DarkModeSwitch
          checked={isDarkMode}
          onChange={toggleDarkMode}
          size={40}
        />
      </div>
    </div>
  );
}
