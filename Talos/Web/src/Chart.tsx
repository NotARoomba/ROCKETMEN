import React, { useEffect, useMemo, useRef, useState } from "react";
import UplotReact from "uplot-react";
import "uplot/dist/uPlot.min.css";
import { Data } from "./Types"; // Asegúrate de definir tu tipo "Data"

type Props = {
  data: Data[];
  isDarkMode: boolean;
};

const AccelerationChart: React.FC<Props> = ({ data, isDarkMode }) => {
  const plotRef = useRef<any>(null);

  // Transforma tu arreglo `data[]` a formato `uPlot`
  const uPlotData = useMemo(() => {
    const times: number[] = [];
    const accelX: number[] = [];
    const accelY: number[] = [];
    const accelZ: number[] = [];

    for (const entry of data) {
      times.push(entry.time / 1000); // si deseas, puedes dejar time en ms
      accelX.push(entry.accel_x);
      accelY.push(entry.accel_y);
      accelZ.push(entry.accel_z);
    }

    return [
      Float64Array.from(times),
      Float64Array.from(accelX),
      Float64Array.from(accelY),
      Float64Array.from(accelZ),
    ];
  }, [data]);

  const options = useMemo(() => {
    return {
      width: 450,
      height: 200,
      title: "Aceleración en 3 ejes",
      scales: {
        x: {
          time: false,
        },
        y: {
          auto: true,
        },
      },
      axes: [
        {
          stroke: isDarkMode ? "white" : "black",
        },
        {
          stroke: isDarkMode ? "white" : "black",
        },
      ],
      series: [
        {}, // eje X
        {
          label: "X",
          stroke: "red",
        },
        {
          label: "Y",
          stroke: "green",
        },
        {
          label: "Z",
          stroke: "blue",
        },
      ],
    };
  }, [isDarkMode]);

  return <UplotReact options={options} data={uPlotData} />;
};

export default AccelerationChart;
