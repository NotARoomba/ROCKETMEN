<h1 align="center">
  <br>
  <a href="https://talos.notaroomba.dev"><img src="https://raw.githubusercontent.com/NotARoomba/ROCKETMEN/master/Talos/Web/public/assets/icon.png" alt="ROCKETMEN" width="200"></a>
  <br>
  ROCKETMEN
  <br>
</h1>

<h4 align="center">Project files for a custom model rocket and embedded flight controller.</h4>

<p align="center">

![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)
![Rust](https://img.shields.io/badge/rust-%23000000.svg?style=for-the-badge&logo=rust&logoColor=white)
![React](https://img.shields.io/badge/react-%2320232a.svg?style=for-the-badge&logo=react&logoColor=%2361DAFB)
![TailwindCSS](https://img.shields.io/badge/tailwindcss-%2338B2AC.svg?style=for-the-badge&logo=tailwind-css&logoColor=white)
![Threejs](https://img.shields.io/badge/threejs-black?style=for-the-badge&logo=three.js&logoColor=white)

</p>

<p align="center">
  <a href="#key-features">Key Features</a> •
  <a href="#how-to-use">How To Use</a> •
  <a href="#credits">Credits</a> •
  <a href="#license">License</a>
</p>

![screenshot](https://raw.githubusercontent.com/NotARoomba/ROCKETMEN/master\Research\Talos\images\website_data.png)

## Key Features

- Small (12cm x 5cm) microcontroller with
  - STM32 MCU
  - USB-C with USB-Serial CH340 chip
  - LoRa communication at 433 Mhz from the LLCC68 chip
  - IMU that proides gyro/accelerometer data
  - Pressure and temperature sensors
- Website that provides data in real time (websockets)
- Client that sends data from the ground station to the website

## How To Use

To clone and run this application, you'll need [Git](https://git-scm.com) and [Rust](https://www.rust-lang.org/) installed on your computer. From your command line:

```bash
# Clone this repository
$ git clone https://github.com/NotARoomba/ROCKETMEN

# Go into the repository
$ cd ROCKETMEN

# Go into the server
$ cd Talos/Server

# Run the app
$ cargo run
```

## Credits

This software uses the following open source packages:

- [Rust](https://www.rust-lang.org/)
- [Node.js](https://nodejs.org/)

## You may also like...

- [Niveles De Niveles](https://github.com/NotARoomba/NivelesDeNiveles) - A real-time flood alert app
- [SupDoc](https://github.com/NotARoomba/SupDoc) - An app facilitating doctor/patient communication

## License

MIT

---

> [notaroomba.dev](https://notaroomba.dev) &nbsp;&middot;&nbsp;
> GitHub [@NotARoomba](https://github.com/NotARoomba) &nbsp;&middot;&nbsp;
