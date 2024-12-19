import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  Legend,
  ResponsiveContainer,
} from "recharts";

const velocityData = [
  {
    time: "0s",
    velocity: 0,
  },
  {
    time: "1s",
    velocity: 10,
  },
  {
    time: "2s",
    velocity: 25,
  },
  {
    time: "3s",
    velocity: 50,
  },
  {
    time: "4s",
    velocity: 80,
  },
  {
    time: "5s",
    velocity: 120,
  },
  {
    time: "6s",
    velocity: 160,
  },
];

const accelerationData = [
  {
    time: "0s",
    acceleration: 0,
  },
  {
    time: "1s",
    acceleration: 10,
  },
  {
    time: "2s",
    acceleration: 15,
  },
  {
    time: "3s",
    acceleration: 25,
  },
  {
    time: "4s",
    acceleration: 30,
  },
  {
    time: "5s",
    acceleration: 40,
  },
  {
    time: "6s",
    acceleration: 0, // Assuming thrust cutoff
  },
];

export default function App() {
  return (
    <div className="h-[100vh] w-full bg-anti-flash_white dark:bg-eerie_black text-eerie_black dark:text-french_gray grid grid-rows-3 grid-cols-3 p-8 gap-6 ">
      <div className="flex gap-x-6 col-span-2">
        <div className=" bg-onyx rounded-2xl w-1/4  p-4 flex flex-col">
          {/* <p className="text-4xl mx-auto mb-4">Inclination</p> */}

          <div className="m-auto flex flex-col">
            <p className="m-auto text-2xl">X</p>
            <p className="m-auto text-4xl">1.302°</p>
          </div>
          <div className="m-auto flex flex-col">
            <p className="m-auto text-2xl">Y</p>
            <p className="m-auto text-4xl">-2.0211°</p>
          </div>
          <div className="m-auto flex flex-col">
            <p className="m-auto text-2xl">Z</p>
            <p className="m-auto text-4xl">0.32°</p>
          </div>
        </div>
        <div className=" bg-onyx rounded-2xl w-1/4  p-4 flex flex-col">
          <p className="text-4xl mx-auto">Top</p>
        </div>
        <div className=" bg-onyx rounded-2xl w-1/4  p-4 flex flex-col">
          <p className="text-4xl mx-auto">Front</p>
        </div>
        <div className=" bg-onyx rounded-2xl w-1/4 p-4 flex flex-col">
          <p className="text-4xl mx-auto">Side</p>
        </div>
      </div>
      <div className=" bg-onyx rounded-2xl h-3/4 p-4 flex flex-col">
        <p className="text-4xl mx-auto mb-4">Acceleration</p>
        <p className="text-7xl m-auto">5 m/s²</p>
      </div>
      <div className=" bg-onyx rounded-2xl h-3/4  p-4 flex flex-col">
        <p className="text-4xl mx-auto mb-4">Velocity</p>
        <p className="text-7xl m-auto">120 m/s</p>
      </div>
      <div className=" bg-onyx rounded-2xl text-9xl flex">
        <p className="m-auto">Talos</p>
      </div>
      <div className=" bg-onyx rounded-2xl -translate-y-[20%] h-[125%]">
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
      <div className=" bg-onyx rounded-2xl h-[125%] -translate-y-[20%]">
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
      <div className=" bg-onyx rounded-2xl w-[125%] text-6xl flex">
        <p className="m-auto text-sea_green">Status Nominal</p>
      </div>
      <div className=" bg-onyx rounded-2xl translate-x-1/3 w-3/4 p-4 flex flex-col gap-y-4">
        <div className="m-auto flex flex-col">
          <p className="m-auto text-2xl">Temperature</p>
          <p className="m-auto text-6xl">32°C</p>
        </div>
        <div className="m-auto flex flex-col">
          <p className="m-auto text-2xl">Pressure</p>
          <p className="m-auto text-6xl">24 Pa</p>
        </div>
      </div>
    </div>
  );
}
