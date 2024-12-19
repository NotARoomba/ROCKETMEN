export default function App() {
  return (
    <div className="h-[100vh] w-full bg-anti-flash_white dark:bg-eerie_black text-eerie_black dark:text-french_gray grid grid-rows-3 grid-cols-3 p-8 gap-6 ">
      <div className="bg-onyx rounded-2xl flex w-[150%] p-4">
        <div className="mx-auto flex flex-col">
          <p className="text-4xl mx-auto mb-4">Angles</p>

          <p className="m-auto text-2xl">X 1.0040°</p>

          <p className="m-auto text-2xl">Y 56.32°</p>
          <p className="m-auto text-2xl">Z 90°</p>
        </div>
        <div className="mx-auto flex flex-col">
          <p className="text-4xl mx-auto mb-4">Top</p>
        </div>
        <div className="mx-auto flex flex-col">
          <p className="text-4xl mx-auto mb-4">Front</p>
        </div>
        <div className="mx-auto flex flex-col">
          <p className="text-4xl mx-auto mb-4">Side</p>
        </div>

        {/* <div className=" bg-onyx rounded-2xl w-1/4  p-4 flex flex-col">
          <p className="text-4xl mx-auto">Top</p>
        </div>
        <div className=" bg-onyx rounded-2xl w-1/4  p-4 flex flex-col">
          <p className="text-4xl mx-auto">Front</p>
        </div> */}
      </div>
      <div className=" bg-onyx rounded-2xl w-1/2 translate-x-full p-4 flex flex-col">
        <p className="text-4xl mx-auto">Pressure</p>
      </div>
      <div className=" bg-onyx rounded-2xl h-3/4 p-4 flex flex-col">
        <p className="text-4xl mx-auto mb-4">Acceleration</p>
      </div>
      <div className=" bg-onyx rounded-2xl h-3/4  p-4 flex flex-col">
        <p className="text-4xl mx-auto mb-4">Velocity</p>
      </div>
      <div className=" bg-onyx rounded-2xl text-8xl flex">
        <p className="m-auto">Talos</p>
      </div>
      <div className=" bg-onyx rounded-2xl -translate-y-[20%] h-[125%]">06</div>
      <div className=" bg-onyx rounded-2xl h-[125%] -translate-y-[20%]">07</div>
      <div className=" bg-onyx rounded-2xl w-[125%]">08</div>
      <div className=" bg-onyx rounded-2xl translate-x-1/3 w-3/4">09</div>
    </div>
  );
}
