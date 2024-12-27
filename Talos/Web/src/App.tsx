import { useEffect, useState } from "react";
import { DarkModeSwitch } from "react-toggle-dark-mode";
import {
  CartesianGrid,
  Line,
  LineChart,
  ResponsiveContainer,
  Tooltip,
  XAxis,
  YAxis,
} from "recharts";
import { Observable, bufferTime } from "rxjs";
import Model from "./Model";
import { Axis, Data } from "./Types";

const API_URL = "wss://talos-api.notaroomba.dev";
const TEST = false;

export default function App() {
  const [data, setData] = useState<Data[]>([]);
  const [isConnected, setIsConnected] = useState(false);
  // const [velocityData, setVelocityData] = useState<
  //   { velocity: number; time: string }[]
  // >([]);
  // const [accelerationData, setAccelerationData] = useState<
  //   { acceleration: number; time: string }[]
  // >([]);
  const [isDarkMode, setDarkMode] = useState(false);

  const toggleDarkMode = (checked: boolean) => {
    setDarkMode(checked);
    document.body.classList.toggle("dark");
  };

  useEffect(() => {
    // toggleDarkMode(true);
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
    const buffered$ = message$.pipe(bufferTime(10));
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
            avg_accel: 0 + 0.000005 * (Date.now() - startDate),
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
            avg_vel:
              prevData.length > 0
                ? prevData[prevData.length - 1].avg_vel +
                  (prevData[prevData.length - 1].avg_accel *
                    (Date.now() - startDate)) /
                    100000
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
      }, 10);
    }

    return () => subscription.unsubscribe();
  }, []);

  // Derive velocity and acceleration from the data
  //update every second

  // useEffect(() => {
  //   if (data.length > 0 && data.length % 3 == 0) {
  //     const accelerationData = data.map((d) => ({
  //       time: `${(d.time - data[0].time) / 1000}`,
  //       acceleration: Math.sqrt(
  //         d.accel_x ** 2 + d.accel_y ** 2 + d.accel_z ** 2
  //       ),
  //     }));
  //     setAccelerationData(accelerationData);
  //   }

  //   //updat the velocity data so that it is the integral of the acceleration data using a for loop
  //   let velocityData: { velocity: number; time: string }[] = [];
  //   for (let i = 0; i < accelerationData.length; i++) {
  //     if (i === 0) {
  //       velocityData.push({
  //         time: accelerationData[i].time,
  //         velocity: 0,
  //       });
  //     } else {
  //       velocityData.push({
  //         time: accelerationData[i].time,
  //         velocity:
  //           velocityData[i - 1].velocity +
  //           ((accelerationData[i].acceleration +
  //             accelerationData[i - 1].acceleration) /
  //             2) *
  //             0.01,
  //       });
  //     }
  //   }
  //   setVelocityData(velocityData);
  // }, [data]);

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
    <div className="h-full xl:h-screen  w-full bg-anti-flash_white text-center dark:bg-eerie_black text-eerie_black dark:text-french_gray grid xl:grid-rows-3 grid-cols-1 xl:grid-cols-3 p-8 gap-6">
      <div className="flex  gap-6 col-span-1 xl:col-span-2 xl:flex-nowrap flex-wrap">
        <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl py-12 text-8xl w-full xl:hidden flex">
          <p className="m-auto">Talos</p>
        </div>
        <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl  py-12 text-5xl xl:hidden flex">
          <p
            className={
              "m-auto text-center " +
              (isConnected || TEST
                ? " text-marian_blue dark:text-vista_blue"
                : " text-amaranth")
            }
          >
            {isConnected || TEST
              ? data.length == 0
                ? "Status No Data"
                : "Status Connected"
              : "Status Disconnected"}
          </p>
        </div>
        <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl w-full xl:w-1/4 xl:min-w-48 mx-auto p-4 flex flex-col ">
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
        <div className="flex w-full gap-6 sm:flex-nowrap flex-wrap">
          <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl w-full xl:max-w-56 p-4 flex flex-col mx-auto">
            <p className="text-4xl mx-auto">Top</p>
            <Model data={data[data.length - 1]} axis={Axis.TOP} />
          </div>
          <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl w-full xl:max-w-56  p-4 flex flex-col mx-auto">
            <p className="text-4xl mx-auto">Front</p>
            <Model data={data[data.length - 1]} axis={Axis.FRONT} />
          </div>
          <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl w-full xl:max-w-56 p-4 flex flex-col mx-auto">
            <p className="text-4xl mx-auto">Side</p>
            <Model data={data[data.length - 1]} axis={Axis.SIDE} />
          </div>
        </div>
      </div>
      <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl h-3/4 p-4 xl:flex hidden flex-col">
        <p className="text-4xl mx-auto mb-4">Acceleration</p>
        <p className="text-6xl m-auto">
          {data.length > 0 ? data[data.length - 1].avg_accel.toFixed(2) : 0}{" "}
          m/s²
        </p>
      </div>
      <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl h-3/4 p-4 xl:flex hidden flex-col">
        <p className="text-4xl mx-auto mb-4">Velocity</p>
        <p className="text-6xl m-auto">
          {data.length > 0 ? data[data.length - 1].avg_vel.toFixed(2) : 0} m/s
        </p>
      </div>
      <div className="flex gap-6 mx-auto w-full h-full xl:hidden flex-wrap sm:flex-nowrap">
        <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl w-full p-4 flex flex-col">
          <p className="text-4xl mx-auto mb-4">Acceleration</p>
          <p className="text-5xl sm:text-6xl m-auto">
            {data.length > 0 ? data[data.length - 1].avg_accel.toFixed(2) : 0}{" "}
            m/s²
          </p>
        </div>
        <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl w-full p-4 flex flex-col">
          <p className="text-4xl mx-auto mb-4">Velocity</p>
          <p className="text-5xl sm:text-6xl m-auto">
            {data.length > 0 ? data[data.length - 1].avg_vel.toFixed(2) : 0} m/s
          </p>
        </div>
      </div>
      <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl py-12 xl:p-0 text-8xl xl:text-9xl hidden xl:flex">
        <p className="m-auto">Talos</p>
      </div>
      <div className="flex-row w-full gap-6 md:flex-nowrap flex-wrap xl:hidden flex">
        <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl min-h-64 w-full xl:hidden flex">
          {(isConnected || TEST) && data.length != 0 ? (
            <ResponsiveContainer width="100%" height="100%">
              <LineChart
                width={500}
                height={300}
                data={data.filter((_, index) => {
                  return index % 20 === 0;
                })}
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
                  tickFormatter={(time) => `${(time - data[0].time) / 1000}s`}
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
                  dataKey="accel_x"
                  stroke="#ff0000"
                  activeDot={{ r: 4 }}
                  dot={false}
                />
                <Line
                  type="monotone"
                  dataKey="accel_y"
                  stroke="#00ff00"
                  activeDot={{ r: 4 }}
                  dot={false}
                />
                <Line
                  type="monotone"
                  dataKey="accel_z"
                  stroke="#0000ff"
                  activeDot={{ r: 4 }}
                  dot={false}
                />
              </LineChart>
            </ResponsiveContainer>
          ) : (
            <p className="text-6xl m-auto text-amaranth">No Data</p>
          )}
        </div>
        <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl w-full min-h-64 xl:h-[125%] xl:hidden flex xl:-translate-y-[20%]">
          {(isConnected || TEST) && data.length != 0 ? (
            <ResponsiveContainer width="100%" height="100%">
              <LineChart
                width={500}
                height={300}
                data={data.filter((_, index) => {
                  return index % 20 === 0;
                })}
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
                  tickFormatter={(time) => `${(time - data[0].time) / 1000}s`}
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
                  dataKey="vel_x"
                  stroke="#ff0000"
                  activeDot={{ r: 4 }}
                  dot={false}
                />
                <Line
                  type="monotone"
                  dataKey="vel_y"
                  stroke="#00ff00"
                  activeDot={{ r: 4 }}
                  dot={false}
                />
                <Line
                  type="monotone"
                  dataKey="vel_z"
                  stroke="#0000ff"
                  activeDot={{ r: 4 }}
                  dot={false}
                />
              </LineChart>
            </ResponsiveContainer>
          ) : (
            <p className="text-6xl m-auto text-amaranth">No Data</p>
          )}
        </div>
      </div>
      <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl min-h-64 xl:-translate-y-[20%] xl:h-[125%] xl:flex hidden">
        {(isConnected || TEST) && data.length != 0 ? (
          <ResponsiveContainer width="100%" height="100%">
            <LineChart
              width={500}
              height={300}
              data={data.filter((_, index) => {
                return index % 20 === 0;
              })}
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
                tickFormatter={(time) => `${(time - data[0].time) / 1000}s`}
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
                dataKey="accel_x"
                stroke="#ff0000"
                activeDot={{ r: 4 }}
                dot={false}
              />
              <Line
                type="monotone"
                dataKey="accel_y"
                stroke="#00ff00"
                activeDot={{ r: 4 }}
                dot={false}
              />
              <Line
                type="monotone"
                dataKey="accel_z"
                stroke="#0000ff"
                activeDot={{ r: 4 }}
                dot={false}
              />
            </LineChart>
          </ResponsiveContainer>
        ) : (
          <p className="text-6xl m-auto text-amaranth">No Data</p>
        )}
      </div>
      <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl w-full min-h-64 xl:h-[125%] xl:-translate-y-[20%] xl:flex hidden">
        {(isConnected || TEST) && data.length != 0 ? (
          <ResponsiveContainer width="100%" height="100%">
            <LineChart
              width={500}
              height={300}
              data={data.filter((_, index) => {
                return index % 20 === 0;
              })}
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
                tickFormatter={(time) => `${(time - data[0].time) / 1000}s`}
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
                dataKey="vel_x"
                stroke="#ff0000"
                activeDot={{ r: 4 }}
                dot={false}
              />
              <Line
                type="monotone"
                dataKey="vel_y"
                stroke="#00ff00"
                activeDot={{ r: 4 }}
                dot={false}
              />
              <Line
                type="monotone"
                dataKey="vel_z"
                stroke="#0000ff"
                activeDot={{ r: 4 }}
                dot={false}
              />
            </LineChart>
          </ResponsiveContainer>
        ) : (
          <p className="text-6xl m-auto text-amaranth">No Data</p>
        )}
      </div>
      <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl  py-12 xl:p-0 xl:w-[125%] text-5xl hidden xl:flex">
        <p
          className={
            "m-auto text-center " +
            (isConnected || TEST
              ? " text-marian_blue dark:text-vista_blue"
              : " text-amaranth")
          }
        >
          {isConnected || TEST
            ? data.length == 0
              ? "Status No Data"
              : "Status Connected"
            : "Status Disconnected"}
        </p>
      </div>
      <div className="dark:bg-onyx shadow-2xl bg-white rounded-2xl xl:translate-x-1/3 w-full xl:w-3/4 p-4 flex flex-col gap-y-4">
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
