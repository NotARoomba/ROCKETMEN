import { useEffect, useState } from "react";
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

interface Data {
  accel_x: number;
  accel_y: number;
  accel_z: number;
  angle_x: number;
  angle_y: number;
  angle_z: number;
  temperature: number;
  pressure: number;
  time: number; // In seconds
}

const API_URL = "ws://localhost:3001";

export default function App() {
  const [data, setData] = useState<Data[]>([]);

  useEffect(() => {
    // Create the WebSocket connection
    const socket = new WebSocket(API_URL);

    // Observable for WebSocket messages
    const message$ = new Observable<any>((observer) => {
      socket.onmessage = (message) => {
        observer.next(JSON.parse(message.data));
      };
      socket.onerror = (error) => observer.error(error);
      socket.onclose = () => observer.complete();

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
    };

    return () => subscription.unsubscribe();
  }, []);

  // Derive velocity and acceleration from the data
  const accelerationData = data.map((d) => ({
    time: `${d.time}s`,
    acceleration: Math.sqrt(d.accel_x ** 2 + d.accel_y ** 2 + d.accel_z ** 2),
  }));

  const velocityData = accelerationData.map((point, index) => {
    if (index === 0) return { time: point.time, velocity: 0 };
    const previous = accelerationData[index - 1];
    const velocity = (previous.acceleration + point.acceleration) / 2; // Simple approximation
    return { time: point.time, velocity };
  });

  return (
    <div className="h-[100vh] w-full bg-anti-flash_white dark:bg-eerie_black text-eerie_black dark:text-french_gray grid grid-rows-3 grid-cols-3 p-8 gap-6">
      <div className="flex gap-x-6 col-span-2">
        <div className="bg-onyx rounded-2xl w-1/4 p-4 flex flex-col">
          <div className="m-auto flex flex-col">
            <p className="m-auto text-2xl">X</p>
            <p className="m-auto text-4xl">
              {data.length > 0 ? data[data.length - 1].angle_x.toFixed(3) : 0}°
            </p>
          </div>
          <div className="m-auto flex flex-col">
            <p className="m-auto text-2xl">Y</p>
            <p className="m-auto text-4xl">
              {data.length > 0 ? data[data.length - 1].angle_y.toFixed(3) : 0}°
            </p>
          </div>
          <div className="m-auto flex flex-col">
            <p className="m-auto text-2xl">Z</p>
            <p className="m-auto text-4xl">
              {data.length > 0 ? data[data.length - 1].angle_z.toFixed(3) : 0}°
            </p>
          </div>
        </div>
        <div className="bg-onyx rounded-2xl w-1/4 p-4 flex flex-col">
          <p className="text-4xl mx-auto">Top</p>
        </div>
        <div className="bg-onyx rounded-2xl w-1/4 p-4 flex flex-col">
          <p className="text-4xl mx-auto">Front</p>
        </div>
        <div className="bg-onyx rounded-2xl w-1/4 p-4 flex flex-col">
          <p className="text-4xl mx-auto">Side</p>
        </div>
      </div>
      <div className="bg-onyx rounded-2xl h-3/4 p-4 flex flex-col">
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
      <div className="bg-onyx rounded-2xl h-3/4 p-4 flex flex-col">
        <p className="text-4xl mx-auto mb-4">Velocity</p>
        <p className="text-7xl m-auto">
          {velocityData.length > 0
            ? velocityData[velocityData.length - 1].velocity.toFixed(2)
            : 0}{" "}
          m/s
        </p>
      </div>
      <div className="bg-onyx rounded-2xl text-9xl flex">
        <p className="m-auto">Talos</p>
      </div>
      <div className="bg-onyx rounded-2xl -translate-y-[20%] h-[125%]">
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
            />
          </LineChart>
        </ResponsiveContainer>
      </div>
      <div className="bg-onyx rounded-2xl h-[125%] -translate-y-[20%]">
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
            />
          </LineChart>
        </ResponsiveContainer>
      </div>
      <div className="bg-onyx rounded-2xl w-[125%] text-6xl flex">
        <p className="m-auto text-sea_green">Status Nominal</p>
      </div>
      <div className="bg-onyx rounded-2xl translate-x-1/3 w-3/4 p-4 flex flex-col gap-y-4">
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
            {data.length > 0 ? data[data.length - 1].pressure.toFixed(1) : 0} Pa
          </p>
        </div>
      </div>
    </div>
  );
}
